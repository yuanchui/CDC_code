/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include <stdio.h>

/* ============================================================================
 * SPI片选(CS)控制宏定义
 * ============================================================================
 * 说明：PCAP04芯片通过SPI接口通信，需要片选信号(CS)来控制通信
 * CS引脚：PA4 (GPIO_PIN_4)
 * CS低电平有效：拉低CS使能通信，拉高CS结束通信
 * ============================================================================ */
#define FLASH_SPI_CS_CLK_ENABLE()                  __HAL_RCC_GPIOA_CLK_ENABLE()    // 使能GPIOA时钟
#define FLASH_SPI_CS_PORT                          GPIOA                          // CS引脚所在端口：GPIOA
#define FLASH_SPI_CS_PIN                           GPIO_PIN_4                     // CS引脚：PA4

// CS使能/禁用宏定义（低电平有效）
#define FLASH_SPI_CS_ENABLE()                      HAL_GPIO_WritePin(FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN, GPIO_PIN_RESET)  // 拉低CS，使能SPI通信
#define FLASH_SPI_CS_DISABLE()                     HAL_GPIO_WritePin(FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN, GPIO_PIN_SET)    // 拉高CS，禁用SPI通信

// 备用定义（高电平有效，当前未使用）
//#define FLASH_SPI_CS_ENABLE()                      HAL_GPIO_WritePin(FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN, GPIO_PIN_SET)
//#define FLASH_SPI_CS_DISABLE()                     HAL_GPIO_WritePin(FLASH_SPI_CS_PORT, FLASH_SPI_CS_PIN, GPIO_PIN_RESET)

/* ============================================================================
 * PCAP04芯片操作码(OPCODE)定义
 * ============================================================================
 * 说明：PCAP04通过SPI接口接收操作码来执行不同的操作
 * ============================================================================ */
// Standard Device Address 0 | 1 | 0 | 1 | 0 | A1 | A0 | R/W
#define PCAP04_I2C  0x28                           // I2C地址（当前使用SPI，此定义未使用）
// Opcode 0x40 + Result Register RES01 0x04
#define REG         0x44                           // 寄存器地址（未使用）
// OPCODES - PCAP04操作码定义
#define WR_MEM      0xA0                           // 写内存操作码：用于写入固件到芯片内存
#define WR_CONFIG   0xA3                           // 写配置操作码：用于写入配置寄存器
#define POR         0x88                           // 上电复位(Power-On Reset)：复位所有状态
#define INITIALIZE  0x8A                           // 初始化操作码：初始化芯片
#define CDC_START   0x8C                           // CDC启动操作码：启动电容数字转换
#define RDC_START   0x8E                           // RDC启动操作码：启动电阻数字转换
#define NV_STORE    0x96                           // 非易失性存储操作码：保存配置到NVRAM
#define TEST        0x7e                           // 测试操作码：用于测试通信，返回0x11表示正常
#define PCAP04_READ_RESULT        0x40            // 读结果操作码：读取芯片转换结果（基地址，需加上通道偏移）


uint32_t Value[6];			

/* ============================================================================
 * PCAP04配置寄存器数组（64个寄存器，地址0x00-0x3F）
 * ============================================================================
 * 说明：此数组包含PCAP04芯片的完整配置参数
 * 每个寄存器对应PCAP04芯片的一个配置寄存器（地址0x00-0x3F）
 * 配置包括：时钟设置、端口配置、测量参数、保护功能等
 * ============================================================================ */
uint8_t PCap02_config_reg[64] = 
{
	// ========== 寄存器0 (地址0x00) ==========
	0x1D,   
					 // 寄存器0，IIC地址设置、低频时钟：
					 // 0Bxx，I2C_A：0~3 = 设备IIC地址设置；
					 // 											

					 // 0Bxxx，OLF_CTUNE：：微调低速时钟，0~15；
					/* OLF_CTUNE			OLF_FTUNE			OLF Frequency
						 11:= (10kHz) 		0001 						5 kHz
						 11:= (10kHz) 		0111 						10 kHz
						 10:= (50kHz) 		0000 						28 kHz
						 10:= (50kHz) 		0011 						48 kHz
						 01:= (100kHz) 		0100 						100 kHz
						 00:= (200kHz) 		0101 						200 kHz						
					*/	
					 // 0Bxxx，OLF_CTUNE：粗调低速时钟；
					 //									00 = 200kHz；
					 //									01 = 100kHz；
					 //									10 = 50kHz（推荐）；
					 //									11 = 10kHz；					
	        0x00,   
					// 寄存器1，外部高速时钟OX配置：
					// 0Bx，OX_DIS：禁用OX时钟；
					//							0 = 开启；
					//							1 = 禁用；
					// 0Bx，OX_DIV4：时钟4分频；
					//							 0 = 不分频；
					//							 1 = 4分频；
					// 0Bx，OX_AUTOSTOP_DIS：芯片内部固定参数为0
					// 0Bx，OX_STOP：芯片内部固定参数为0
					// 0Bxx，OX_RUN：控制脉冲模式的持久性或延迟，延迟是指测量开始前的振荡器预热时间；
					//								000 = 关闭OX时钟；
					//								001 = OX永恒运行；
					//								010 = OX延迟31/低速时钟频率（31倍低速时钟周期）；
					//								011 = OX延迟2/低速时钟频率（2倍低速时钟周期）；
					//								110 = OX延迟1/低速时钟频率（1倍低速时钟周期）；
	        0x58,   
					 // 寄存器2：端口放电电阻值配置
					// 0Bxx，RDCHG_INT_1：选择PC4~PC5的放电电阻；
					/*											00 = 180 kΩ
																01 = 90 kΩ
																10 = 30 kΩ
																11 = 10 kΩ
					*/
					// 0Bxx，RDCHG_INT_0：选择PC0~PC3和PC6的4个片上放电电阻中选择一个；
					/*											00 = 180 kΩ
																01 = 90 kΩ
																10 = 30 kΩ
																11 = 10 kΩ				
					*/
					// 0Bx，RDCHG_INT_EN：					0 = 关闭内部放电电阻；
					//										1 = 启用内部放电电阻；
					// 0Bx，RDCHG_EXT_EN：					0 = 关闭外部放电电阻；
					//										1 = 启用外部放电电阻；
			0x10,   
					// 寄存器3，端口充放电电阻值配置：
					// 0Bx，AUX_PD_DIS：取消内部下拉电阻来启用PC_AUX；
					//									0 = 不启用
					//									1 = 启用
					// 0Bx，AUX_CINT：仅在内部引用转换期间激活辅助端口PCAUX；
					//									0 = 正常（默认值）
					//									1 = 内部激活期间的辅助功能				
					// 0Bxx，RDCHG_EARLY_OPEN：2 = 提前打开芯片内部放电电阻
					//		
					// 0Bx，RDCHG_PERM_EN：1 = 保持芯片内部放电电阻永久连接
					//									0 = 不永久连接(默认)
					//									1 = 启用					
					// 0Bx，RDCHG_EXT_PERM：1 = 启用外部放电电阻
					//									0 = 不启用(默认)
					//									1 = 启用					
					// 0Bx，RCHG_SEL：选择充电电阻；
					/*									00 = 180 kΩ
														01 = 10 kΩ

					*/
					
	        0x10,   
					// 寄存器4：端口电容连接方式
					// 0Bx，C_REF_INT：在PC0/GND或PC0/PC1上使用片上参比电容；
					//									0 = 外部参比电容，PC0/GND或PC0/PC1；
					//									1 = 内部参比电容；
					// 0Bx，C_COMP_EXT：1 = 启动芯片外寄生电容的补偿机制
					//									0 = 不激活
					//									1 = 激活					
					// 0Bx，C_COMP_INT：1 = 激活片上寄生电容的补偿机制并进行增益补偿；
					//									0 = 不激活
					//									1 = 激活
					// 0Bx，C_DIFFERENTIAL：在单个传感器或差动传感器之间进行选择
					//									0 = 单端传感器
					//									1 = 差分传感器
					// 0Bx，C_FLOATING：选择接地传感器或浮动传感器
					//									0 = 接地测量
					//									1 = 浮动测量					
			0x00,	
					// 寄存器5：周期时钟配置
					// 0Bx，CY_PRE_MR1_SHORT：减少内部时钟路径之间的延迟；
					//									0 = 正常（推荐）
					//									1 = 减少					
					// 0Bx，C_PORT_PAT：测量端口的顺序将在每个序列后颠倒；
					//									0 = 正常
					//									1 = 顺序测量后，每个端口的顺序颠倒
					// 0Bxx，CY_HFCLK_SEL：选择CDC的时钟源；
					/*									0 = OLF_CLK
														1 = OHF_CLK
					*/
					// 0Bx，CY_DIV4_DIS：四倍时钟周期；
					//									 0 = 不分频；
					//							 		 1 = 4时钟周期；
					// 0Bx，CY_PRE_LONG：增加内部时钟路径之间的安全延迟；
					//									 0 = 关闭
					//									 1 = 开启
					// 0Bx，C_DC_BALANCE：保持传感器无直流状态；（仅浮动单传感器或者差动传感器）
					//									 0 = 关闭
					//	
					
			0x0F,   
					// 寄存器6，引脚配置启用：
					// 0Hxx，C_PORT_EN：逐位启动CDC端口; 
					/*	16进制							00 = 全部关闭    0x3F：所有端口打开
														01 = PC0
														02 = PC1
														04 = PC2
														08 = PC3
														10 = PC4
														20 = PC5
														40 = PC6
														80 = PC7
						PC0~PC3低4位，PC4~PC7高4位，打开多通道相加即可
						例如，打开PC0和PC1，为01+02=03
									打开PC0,PC1,PC3,为01+02+08=0B
					*/

			0x20, 0x00,   
					// 寄存器7 & 寄存器8，结果平均：
					// 0Bxxxxxxxxxxxxx，C_AVRG：寄存器7开始低12位设置一次平均后输出结果的采样次数；
					/*								0 or 1 = 无平均
													2 = 2个结果平均
													3 = 3个结果平均
													最多8191个采样结果取平均
					*/

			0xD0, 0x07, 0x00, 
					// 寄存器9、寄存器10&寄存器11，：
					// 0Bxxxxxxxxxx...，CONV_TIME：转换触发周期或序列周期，Tcon=2*CONV_TIME/fOLF,
					//								Tcon不可小于单次测量的周期
					
			0x00 ,  
					// 寄存器12， 设置CDC放电时间T放电.时间间隔预留用于放电时间的测量
					// 0Hxx，DISCHARGE_TIME：h00最快，不稳定就向上调整；
					//					放电时间Tdischarge = （DISCHARGE_TIME + 1）*Tcycleclock
					//					0 ： Tdischarge = 1*Tcycleclock					

			0x08,
					// 寄存器13，GPIO3引脚上升沿触发CDC测量；
					// 0Bxx，C_STARTONPIN：选择允许通过硬件触发CDC的GPIO端口；
					/*											00 = PG0
																01 = PG1
																10 = PG2
																11 = PG3					
					*/
					// 0Bxx，C_TRIG_SEL：选择可用于启动CDC的触发源；
					/*										000 = 连续模式
															001 = 已触发读取
															010 = 定时器触发
															011 = 定时器触发
															000 = 拉伸模式 
															000 = 无
															011 = GPIO上升沿触发，硬件触发
															011 = 连续模式输出 （不推荐）
					*/
					// 0Bxx，DISCHARGE_TIME：放电时间：
					//										查看 寄存器12
					
			0xFF,		
					// 寄存器14，设置CDC放电时间 Tprecharge为放电时间测量预留的时间间隔；
					// 0Bxxxxxxxxxx，PRECHARGE_TIME：
															/*Tprecharge = (PRECHARGE_TIME + 1) * Tcycleclock
															OHF & FULLCHARGE _TIME = 1023: 
															Tprecharge = (PRECHARGE_TIME + 2) * Tcycleclock
															OLF & FULLCHARGE _TIME!= 0x3FF: 
															Tprecharge = (PRECHARGE_TIME + 1) * Tcycleclock
															1023: 关闭
															*/					
			0x03,		// 寄存器15，预假采样次数；
					// 0Bxxxx，C_FAKE：真实采样前的预热的假采样次数，是舍弃不要的次数，设为0；
					
					// 0Bxx，PRECHARGE_TIME：设置CDC放电时间：
					//										查看 寄存器14
					
			0x00,		// 寄存器16，设置CDC放电时间；					
					// 0Bxxxxxxxxxx，FULLCHARGE_TIME：27开始到28低2位的10位，设置满充电时间；
					//					OLF:Tfullcharge = (FULLCHARGE_TIME + 1) * Tcycleclock
					//					OHF:Tfullcharge = (FULLCHARGE_TIME + 2) * Tcycleclock	
					
			0x24,		// 寄存器17，内部参考电容大小；
					
					// FULLCHARGE_TIME						查看 寄存器16	
					// 0Bxxxxx，C_REF_SEL：置内部参考电容大小，步宽0.3pF~1.5pF，精度较低；
					//										0D0~0D31 = 0~31pF
					
			0x00,		// 寄存器18，保护功能；					
					// 0Bx，C_G_OP_RUN：防护：OP模式
					/*											00 = 永久的
																01 = 脉冲（在转换之间将OP设置为睡眠模式）					
					*/					
					// 0Bx，C_G_OP_RUN：保护：激活外部OP
					/*											00 = 内部OP
																01 = 外部OP，PG3为C_G_MUX_SEL					
					*/										
					// 0Bxxxxx，C_G_EN：保护：为每个端口启用保护功能					
					/*											b‘xxxxx 1：激活端口PC0
																b‘xxxx 1x：激活端口PC1
																b‘xxx1xx：激活端口PC2
																b‘xx1xxx：激活端口PC3
																b‘x1 xxxx：激活端口PC4
																b‘1 xxxxx：激活端口PC5					
					*/					
					
			0x00,		
					// 寄存器19，防护装置；					
					// 0Bx， C_G_OP_VU：防护装置：OP增益（从检测端口到防护装置）
					/*											0 : x 1.00
																1 : x 1.01
																2 : x 1.02
																3 : x 1.03					
					*/					
					// 0Bx，C_G_OP_ATTN：防护：OP衰减
					/*											0 : 0.5aF
																1 : 1.0aF
																2 : 1.5aF
																3 : 2.0aF					
					*/										
					// 0Bx，C_G_EN：保护：预充电期间将保护端口从“直接连接”切换到OP的时间					
					/*											C_G_TIME * 500ns				
					*/						
					
			0x00,   
					// 寄存器20，RDC时间：
					// 0Bx，R_CY：RDC部分的循环时间；
					/*									
												OLFf		R_CY=0		R_CY=1 
												10Kz		100us		200us
												50Kz		20us		40us			
												100Kz		10us		20us			
												200Kz		20us		40us			
			
					*/
					
					// 0Bxxx，C_G_OP_TR：防护：保护OP电流削减
					/*											0 : ...
																1 : ...
																2 : ...
																7 : 推荐						
					*/
					
			0x01,   // 寄存器21：					
					// 0Dx，R_TRIG_PREDIV：预分频器，允许进行比电容测量更少的温度测量这是CDC与RDC的测量率之间的一个因素。如果OLF被用作触发源，那么它也被用作OLF时钟分频器；
					//										如果在寄存器29设置OLF时钟为触发源，则可作为OLF的分频；
					/*										0 or 1 = 每一次CDC触发后触发
																2 = 每两次CDC触发后触发
																......
																最大2的21次方倍分频
					*/					
					
			0x50,   // 寄存器22：					
					// 0Bxxx，R_TRIG_SEL：触发RDC 5和6的触发源选择：由CDC转换结束时触发；					
					/* 							 				0 : 关闭
																1 : 定时器触发
																3 : 引脚触发
																5 : CDC异步(推荐的)
																6 : CDC同步					
					*/					

					// 0Bx，R_AVRG：RDC部分的平均值计算（平均值）的样本量
								/*								00 = 无平均
																01 = 4个采样点
																10 = 8个采样点
																11 = 16个采样点
					*/
					
					// 0Dx，R_TRIG_PREDIV：预分频器，允许进行比电容测量更少的温度测量这是CDC与RDC的测量率之间的一个因素。如果OLF被用作触发源，那么它也被用作OLF时钟分频器；
					//										如果在寄存器29设置OLF时钟为触发源，则可作为OLF的分频；
					/*										0 or 1 = 每一次CDC触发后触发
																2 = 每两次CDC触发后触发
																......
																最大2的21次方倍分频
					*/
					
			0x30,   // 寄存器23，外置参考电阻器，温度传感器：						
					// 0Bx，r_port_en：RDC部分的端口激活情况					
					/*											00 = 禁用内部参考
																01 = 激活端口PT0REF
																10 = 使能内部参考
																11 = 激活端口PT1															
					*/
					// 0Bx，R_PORT_EN_IMES：内部芯片温度传感器的端口激活					
					/*											0 = 禁用内部温度传感器
																1 = 使能内部温度传感器					
					*/					
					// 0Bx，r_port_en_iref：内参考电阻器的端口激活
					/*											0 = 禁用内部电阻器
																1 = 使能内部电阻器				
					*/	
					// 0Bx，R_FAKE：真实测量前的预热假测量次数；
					//							0 = 每次平均有2次假测量
					//							1 = 每次平均有8次假测量
					// 0Bxx，R_STARTONPIN：选择允许触发RDC启动的GPIO端口
					/*											0 = PG0
																1 = PG1
																2 = PG2
																3 = PG3															
					*/	

			0x73,   // 寄存器24：						
					// 0Bx，TDC_CHAN_EN：    芯片内部固定参数		
					// 0Bx，TDC_ALUPERMOPEN：芯片内部固定参数	
					// 0Bx，TDC_NOISE_DIS：  芯片内部固定参数	
					// 0Bx，TDC_MUPU_SPEED： 芯片内部固定参数	

			0x04,   // 寄存器25：						
					// 0Bx，TDC_MUPU_NO：    芯片内部固定参数		

			0x50,   // 寄存器26：						
					// 0Bx，TDC_QHA_SEL：    芯片内部固定参数		
					// 0Bx，TDC_NOISE_CY_DIS：芯片内部固定参数	
					
			0x08,   // 寄存器27：
					// 0Bxx，DSP_MOFLO_EN：在GPIO脉冲线上使能单稳态触发器；
					//										00 = 关闭
					//										11 = 开启

					// 0Bxx，DSP_SPEED：DSP速度设置；
					//									00 = 最快
					//									01 = 快
					//									01 = 慢（推荐）
					//									01 = 最慢
					// 0Bx，PG1xPG3：			0 = PDM/PWM 脉冲输出端为PG3
					//							1 = PDM/PWM 脉冲输出端为PG1
					// 0Bx，PG0xPG2：			0 = PDM/PWM 脉冲输出端为PG2
					//							1 = PDM/PWM 脉冲输出端为PG0
			0x5A,   // 寄存器28：
					// 0Bxx，WD_DIS：要禁用看门狗0x5A必须被写入这个寄存器
					/*					
												0x5A：看门狗禁用（关闭）
												0x00（推荐）/其他：监视器启用						
					*/	

			0x00,   // 寄存器29：
					// 0Bxxxx，DSP_STARTONPIN：启动DSP的引脚掩码-此掩码允许分配一个或多个GPIO引脚来启动DSP；
					//												逐位，从PG0~PG3
					
					// 0Bxxxx，DSP_FF_IN：触发寄存器；
					//												逐位，从DSP_IN_0~DSP_IN_3

			0x82,   // 寄存器30：
					// 0Bx，PG5_INTN_EN：通道INTN 信号连接至PG5；
					//							0 = PG5正常运行
					//							1 = PG5 <== INTN
					// 0Bx，PG4_INTN_EN：通道INTN 信号连接至PG4；
					//							0 = PG4正常运行
					//							1 = PG4 <== INTN

					// 0Bx，DSP_START_EN：DSP触发器启用
					//							0001 = 在结束时触发CDC
					//							0010 = 在RDC结束时触发（推荐）
					//							0100 = 定时器触发

			0x08,   // 寄存器31：
					// 0Bx，PI1_TOGGLE_EN：激活脉冲接口1输出处的开关触发器 特别是为PDM创建1：1的责任系数
					//							00 = 正常运行
					//							01 = 切换触发器激活状态
					
					// 0Bx，PI0_TOGGLE_EN：激活脉冲接口0输出处的开关触发器 特别是为PDM创建1：1的责任系数
					//							00 = 正常运行
					//							01 = 切换触发器激活状态
					
					// 0Bxx，PI0_RES：脉冲码接口的分辨率;
					/*									0 = 10 bit
														1 = 12 bit
														2 = 14 bit
														3 = 16 bit
					*/
					// 0Bxx，PI0_PDM_SEL：脉冲接口0 PWM / PDM开关：
					/*									0 = PWM
														1 = PDM
					*/
			0x08,   // 寄存器32：					
					// 0Bxxxx，PI1_CLK_SEL：脉冲接口0时钟选择
					/*									0 = 关闭
														1 = OLF / 1
														2 = OLF / 2
														3 = OLF / 4
														4 = OX / 1
														5 = OX / 2
														6 = OX / 4
														7 = n.d
					*/
					// 0Bxx，PI1_PDM_SEL：脉冲接口0 PWM / PDM开关：
					/*									0 = PWM
														1 = PDM
					*/									
					// 0Bxx，PI1_RES：脉冲码接口的分辨率;
					/*									0 = 10 bit
														1 = 12 bit
														2 = 14 bit
														3 = 16 bit
					*/

					0x00,   // 寄存器33：配置PG0~PG3为输入或输出，启动内部保护上拉电阻；
					// 0Bxxxx，PG_DIR_IN：在输入和输出之间切换通用端口方向；
					/*		0 = 输出						1 = 输入
							 0000 OR 0100 = PG0输出				 1000 OR 1100 = PG0输入
							 0001 OR 0101 = PG1输出				 1001 OR 1101 = PG1输入
							 0010 OR 0110 = PG2输出				 1010 OR 1110 = PG2输入
							 0011 OR 0111 = PG3输出				 1011 OR 1111 = PG3输入
					*/
					// 0Bxxxx，PG_PULLUP：配置引脚上拉；
					/*		0 = 悬空						1 = 上拉
							 0000 OR 0100 = PG0悬空				 1000 OR 1100 = PG0上拉
							 0001 OR 0101 = PG1悬空				 1001 OR 1101 = PG1上拉
							 0010 OR 0110 = PG2悬空				 1010 OR 1110 = PG2上拉
							 0011 OR 0111 = PG3悬空				 1011 OR 1111 = PG3上拉
					*/	
					0x47,   // 寄存器34：
					// 0Bxxxx，INT_TRIG_BG：读取结束将触发带隙；
					/*									0 = 失能
														1 = 使能
					*/												
					// 0Bxxxx，DSP_TRIG_BG：带隙刷新由DSP位设置触发；
					/*									0 = 失能
														1 = 使能
					*/							
					// 0Bxxxx，BG_PERM：永久激活带宽。使用BG_PERM = 1，当前的消耗增加了大约。20μA；
					/*									0 = 带隙永久启用
														1 = 带隙脉冲
					*/			
					// 0Bxxxx，AUTOSTART：对于独立操作，开机后触发CDC；
					/*									0 = 失能
														1 = 开机后的CDC触发器
					*/								
					// 0Bxxxx，芯片内部固定参数；					
					
					0x40,   // 寄存器35：
					// 0Bxxxx，AUTOSTART：固件定义的增益校正因子的配置。8fpp中的0到7位；
					/*									0x00 = 0
														0xN  = 1 + N /256
														推荐 使用1.25==>0x40
					*/													

			
					0x00,   // 寄存器36：
					// 0Bxxxxxxxx，未使用

					0x00,   // 寄存器37：
					// 0Bxxxxxxxx，未使用

					0x00,   // 寄存器38：
					// 0Bxxxxxxxx，未使用

					0x71,   // 寄存器39：
					// 0Bxxxx，PULSE_SEL1：固件定义，选择脉冲的源IF1；

					0x00,   // 寄存器40：
					// 0Bxxxx，PULSE_SEL0：固件定义，选择脉冲的源IF0；					
					
					0x03,   // 寄存器40：
					// 0Bxxxx，C_SENSE_SEL：选择线性化的C 比率0..5 ：C0到5 /Cref   固件定义；
					
					0x08,   // 寄存器41：
					// 0Bxxxx，C_SENSE_SEL：选择R比率进行温度测定  固件定义；					
				
					0x00,   // 寄存器42：
					// 0Bxxxx，ALARM1_SELECT：固件定义。选择PG0和PG1处的报警信号源活动高；					
					
					// 0Bxxxx，ALARM0_SELECT：固件定义。选择PG0和PG1处的报警信号源活动高；					
					
					// 0Bxxxx，EN_ASYNC_READ：固件定义。结果寄存器Res0到Res7中的值只在读取了前面的值时才会更新
					
					// 0Bxxxx，HS_ MODE_SEL：芯片内部固定参数；					
					
					// 0Bxxxx，R_ MEDIAN_EN：固件定义。在线性化固件中启用ci/ri的中值滤波器
					// 0Bxxxx，C_ MEDIAN_EN：固件定义。在线性化固件中启用ci/ri的中值滤波器					
					
					0x00,   // 寄存器47：					
					// 0Bxxxxxxxx，Not used 未使用					
					// 0Bxxxx，RUNBIT：前端和DSP的开/关开关：在编程和任何注册表修改期间应该“关闭”；					
					//							0 : 关闭=芯片系统空闲且受保护
					//							1 : 打开=保护功能已被删除，系统也可以运行
					
					0x00,   // 寄存器48：					
					// 0Bxxxxxxxx，Not used 未使用					
					// 0Bxxxx，MEM_LOCK：数据安全功能，使NVRAM的安全部分不能通过SIF进行读写；					
					//							bxxx1 : NVRAM 区间 `d0 to 703
					//							bxx1x : NVRAM 区间 `d704 to 831
					//							bxx1x : NVRAM 区间 `d832 to 959					
					//							bxx1x : NVRAM 区间 `d960 to 1007 and NVRAM range `d1022 to 1023
					
					0x00,   // 寄存器49：
					// 0Bxxxxxxxx，SERIAL_NUMBER 芯片序列号低位	

					0x01,   // 寄存器50：
					// 0Bxxxxxxxx，SERIAL_NUMBER 芯片序列号高位	

					0x00,   // 寄存器51 ~ 存器53：芯片内部固定参数

					0x00,   // 寄存器54： 
					// 0Bxxxxxxxx，MEM_CTRL 内存控制
					/*									0x2d：启用NVRAM存储区
														0x59： NVRAM召回功能已启用
														0xb8： NVRAM擦除寄存器在以下SIF活动后自动重置
					*/
					0x00,   // 寄存器55 ~ 存器61： 芯片内部固定参数

					0x00,   // 寄存器62
					// 0Bxxxxxxxx，CHARGE_PUMP 不允许进行更改NVRAM充电泵低字节

					0x00,   // 寄存器63
					// 0Bxxxxxxxx，CHARGE_PUMP 不允许进行更改NVRAM充电泵高字节
				}












/* ============================================================================
 * PCAP04标准配置数组（64个寄存器，简化版）
 * ============================================================================
 * 说明：这是PCAP04芯片的标准配置参数数组
 * 包含64个寄存器的配置值，用于快速初始化芯片
 * 此数组与PCap02_config_reg数组功能相同，但格式更紧凑
 * 
 * 寄存器地址映射：
 * - 索引0-63 对应 寄存器地址0x00-0x3F
 * - 每个字节对应一个8位配置寄存器
 * ============================================================================ */
unsigned char PCAP04_Config[] = {
 0x1D, 0x00, 0x58, 0x10,  // 寄存器0-3:  时钟配置、端口放电电阻
 0x10, 0x00, 0x3F, 0x20,  // 寄存器4-7:  端口配置、结果平均
 0x00, 0xD0, 0x07, 0x00,  // 寄存器8-11: 转换时间配置
 0x00, 0x08, 0xFF, 0x03,  // 寄存器12-15: 放电时间、预充电时间
 0x00, 0x24, 0x00, 0x00,  // 寄存器16-19: 满充电时间、保护功能
 0x00, 0x01, 0x50, 0x30,  // 寄存器20-23: RDC时间、温度传感器
 0x73, 0x04, 0x50, 0x08,  // 寄存器24-27: TDC配置、DSP配置
 0x5A, 0x00, 0x82, 0x08,  // 寄存器28-31: 看门狗、GPIO配置、脉冲接口
 0x08, 0x00, 0x47, 0x40,  // 寄存器32-35: 脉冲接口、自动启动
 0x00, 0x00, 0x00, 0x71,  // 寄存器36-39: 未使用、脉冲选择
 0x00, 0x00, 0x08, 0x00,  // 寄存器40-43: 线性化配置、报警选择
 0x00, 0x00, 0x00, 0x01,  // 寄存器44-47: 未使用、运行位
 0x00, 0x00, 0x00, 0x00   // 寄存器48-51: 序列号、未使用
};


static const  char PCAP04_Firmware[] =
{
	//Read Memory
 0x24,0x05,0xA0,0x01,0x20,0x55,0x42,0x5C,0x48,0xB1,0x07,0x92,0x02,0x20,0x13,0x02  //0
,0x20,0x93,0x02,0xB2,0x02,0x78,0x20,0x54,0xB3,0x06,0x91,0x00,0x7F,0x20,0x86,0x20  //16
,0x54,0xB6,0x03,0x72,0x62,0x20,0x54,0xB7,0x00,0x00,0x42,0x5C,0xA1,0x00,0x49,0xB0  //32
,0x00,0x49,0x40,0xAB,0x5D,0x92,0x1C,0x90,0x02,0x7F,0x20,0x86,0x66,0x67,0x76,0x77  //48
,0x66,0x7A,0xCF,0xCD,0xE6,0x43,0xF1,0x44,0x29,0xE0,0x7A,0xDC,0xE7,0x41,0x32,0xAA  //64
,0x01,0x99,0xFD,0x7B,0x01,0x7A,0xCF,0xEB,0xE6,0x43,0xF1,0x44,0x29,0xE0,0x7A,0xC1  //80
,0xE7,0x41,0x32,0x6A,0xDE,0x44,0x7A,0xCF,0xEA,0xE6,0x43,0xF1,0x44,0x29,0xE0,0x6A  //96
,0xDF,0x44,0x7A,0xC4,0xE7,0x41,0x32,0xAB,0x05,0x7A,0xC1,0xE1,0x43,0xE0,0x3A,0x7A  //112
,0xC0,0xE1,0x43,0xE0,0x3A,0x02,0x7A,0xCF,0xE6,0xE6,0x43,0xF1,0x44,0x29,0xE0,0x7A  //128
,0xEF,0x44,0x02,0x20,0x9D,0x84,0x01,0x21,0x2E,0x21,0x74,0x20,0x37,0xC8,0x7A,0xE7  //144
,0x43,0x49,0x11,0x6A,0xD4,0x44,0x7A,0xC1,0xD8,0xE6,0x43,0xE9,0x44,0x1C,0x43,0x13  //160
,0xAB,0x63,0x6A,0xDE,0x41,0xAB,0x0B,0x46,0x46,0x46,0x7A,0xDF,0xFF,0xFF,0xFF,0xFF  //176
,0xE3,0x41,0x32,0x1C,0x44,0xE9,0x13,0x6A,0xD4,0x13,0x41,0xAA,0xDF,0x7A,0xC5,0xE1  //192
,0x43,0x49,0xE0,0x34,0x7A,0xCF,0xE3,0xE6,0x43,0xF1,0x44,0x29,0xE0,0xDB,0xC0,0x27  //208
,0xE5,0x6A,0xDF,0x43,0x7A,0xC8,0xE7,0x41,0x30,0xAB,0x03,0x86,0x01,0x92,0x37,0x7A  //224
,0xC6,0xE7,0x41,0x7A,0xFA,0xE7,0x43,0xEA,0x44,0x7A,0xC1,0xE1,0xE6,0x43,0xE9,0x44  //240
,0x25,0xE0,0x7A,0xC6,0xE7,0x41,0x7A,0xFA,0xE7,0x43,0xEA,0x44,0x7A,0xC0,0xE7,0x43  //256
,0xE9,0x44,0x25,0xE0,0x92,0x10,0x7A,0xE1,0x44,0xE2,0x44,0xE3,0x44,0xE4,0x44,0xE5  //272
,0x44,0xE6,0x44,0xE7,0x44,0xE8,0x44,0xC1,0xD8,0x24,0x3E,0x92,0xFF,0x02,0x7A,0xCF  //288
,0xD7,0xE6,0x43,0xF1,0x44,0x7A,0xD0,0xE7,0x43,0x2A,0x2A,0x32,0xAB,0x03,0x42,0x5C  //304
,0x92,0x03,0x7A,0xC0,0xE1,0x43,0xD9,0x27,0x90,0x6A,0xDF,0x43,0x7A,0xC8,0xE7,0x41  //320
,0x32,0xAB,0x03,0x86,0x01,0x92,0x11,0x7A,0xC2,0x43,0x7A,0xE7,0x44,0x6A,0xC6,0x44  //336
,0x7A,0xC3,0x43,0x7A,0xE8,0x44,0x6A,0xC7,0x44,0xC1,0xD4,0x24,0x57,0x7A,0xC8,0xE1  //352
,0x43,0xE0,0x3A,0x02,0x7A,0xCF,0xE7,0xE6,0x43,0xF1,0x44,0x29,0xE0,0x7A,0xC7,0xE1  //368
,0x41,0x6A,0xD4,0x45,0x5A,0x25,0x36,0x46,0x46,0x46,0x46,0x7A,0xE9,0x44,0x7A,0xC0  //384
,0xE7,0x43,0x55,0x7A,0xEA,0x45,0x7A,0xE9,0x51,0x1C,0x43,0x6A,0xCA,0x44,0x1D,0x43  //400
,0x6A,0xCB,0x44,0x74,0xC1,0xCA,0xE6,0x43,0xE9,0x44,0x7A,0xC1,0xE1,0x43,0x7A,0xCC  //416
,0xE0,0xE6,0x41,0x2C,0x42,0x7A,0xC5,0xE1,0x43,0x49,0xE0,0x34,0x7A,0xC1,0xCC,0xE6  //432
,0x43,0xE9,0x44,0x7A,0xC1,0xE1,0x43,0x2C,0x70,0x7A,0xCC,0x43,0x7A,0xCF,0x44,0x7A  //448
,0xCD,0x43,0x7A,0xCE,0x44,0x6A,0xCA,0x43,0xC1,0xCA,0x7A,0xE6,0x41,0xE9,0x45,0x2B  //464
,0xAE,0xEE,0x44,0x7A,0xC1,0xCA,0xE6,0x43,0xE9,0x44,0x7A,0xC1,0xE1,0x43,0x7A,0xCC  //480
,0xEC,0xE6,0x41,0x2C,0x42,0x7A,0xC5,0xE1,0x43,0x49,0xE0,0x34,0x7A,0xC1,0xCC,0xE6  //496
,0x43,0xE9,0x44,0x7A,0xC1,0xE1,0x43,0x2C,0x70,0x7A,0xCC,0x43,0x7A,0xCF,0x44,0x7A  //512
,0xCD,0x43,0x7A,0xCE,0x44,0x6A,0xCB,0x43,0xC1,0xCA,0x7A,0xE6,0x41,0xE9,0x45,0x2B  //528
,0xAE,0xED,0x44,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  //544
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  //560
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  //576
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  //592
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  //608
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  //624
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  //640
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  //656
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  //672
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  //688
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  //704
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  //720
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  //736
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  //752
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  //768
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  //784
,0x35,0x33,0x33,0x07,0xCD,0xCC,0xCC,0x08,0x01,0x00,0xFE,0x03,0x66,0x66,0x66,0x01  //800
,0x33,0x33,0x33,0x02,0x01,0x00,0xFE,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  //816
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  //832
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  //848
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  //864
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  //880
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  //896
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  //912
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  //928
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  //944
,0x1D,0x00,0x58,0x10,0x10,0x00,0x0F,0x20,0x00,0xD0,0x07,0x00,0x00,0x08,0xFF,0x03  //960  配置参数开始
,0x00,0x24,0x00,0x00,0x00,0x01,0x50,0x30,0x73,0x04,0x50,0x08,0x5A,0x00,0x82,0x08  //976
,0x08,0x00,0x47,0x40,0x00,0x00,0x00,0x71,0x00,0x03,0x08,0x00,0x00,0x00,0x00,0x01  //992
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00  //1008
//0    1		 2    3    4    5    6    7    8    9    10   11   12   13  14    15   16
};




uint8_t temp_val = 0x00;
uint8_t re_val[32] = {0};
uint8_t	k;

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


/**
 * @brief  PCAP04通信测试函数
 * @note   通过发送TEST操作码(0x7E)测试与PCAP04芯片的SPI通信是否正常
 * @retval 1-通信正常(返回0x11), 0-通信失败
 * 
 * SPI通信流程：
 * 1. 拉低CS使能通信
 * 2. 发送TEST操作码(0x7E)
 * 3. 同时接收返回值（使用TransmitReceive实现全双工）
 * 4. 拉高CS结束通信
 * 5. 检查返回值是否为0x11（正常通信的标志）
 */
char PCap04_Test(void)
{
	uint8_t text_data = TEST;                      // 发送测试操作码
	uint8_t text_receive = 0x00;                   // 接收缓冲区
	
	// SPI通信开始
	FLASH_SPI_CS_ENABLE();                         // 拉低CS，使能SPI通信
	HAL_Delay(2);                                  // 等待CS稳定
	
	// 发送测试命令并接收响应（全双工模式）
	HAL_SPI_Transmit(&hspi1,&text_data,1,1000);   // 发送TEST操作码
	// HAL_SPI_Receive(&hspi1,&text_receive,1,1000);  // 单独接收模式（已注释）
	HAL_SPI_TransmitReceive(&hspi1,&text_receive,&text_receive,1,1000);  // 全双工：发送同时接收
	
	// SPI通信结束
	FLASH_SPI_CS_DISABLE();                        // 拉高CS，结束SPI通信
	HAL_Delay(2);                                  // 等待CS稳定
	
	// 调试输出
	printf("返回值：%X\n",text_receive);
	
	// 判断通信是否正常：返回0x11表示通信正常
	if(text_receive == 0x11)
	{
		return 1;  // 通信正常
	}
	return 0;  // 通信失败
}


/**
 * @brief  PCAP04上电复位函数
 * @note   发送POR操作码(0x88)复位PCAP04芯片的所有状态
 * 
 * SPI通信流程：
 * 1. 拉低CS使能通信
 * 2. 发送POR操作码
 * 3. 拉高CS结束通信
 * 
 * 注意：此操作会复位芯片的所有寄存器状态，通常在初始化前调用
 */
void PCap04_PowerON_RESET()
{
	uint8_t text_data = POR;                       // POR操作码：0x88
	FLASH_SPI_CS_ENABLE();                        // 拉低CS，使能SPI通信
	HAL_SPI_Transmit(&hspi1,&text_data,1,10);     // 发送POR命令
	FLASH_SPI_CS_DISABLE();                       // 拉高CS，结束SPI通信
}

/**
 * @brief  PCAP04启动CDC转换函数
 * @note   发送CDC_START操作码(0x8C)启动电容数字转换(CDC)
 * 
 * SPI通信流程：
 * 1. 拉低CS使能通信
 * 2. 发送CDC_START操作码
 * 3. 拉高CS结束通信
 * 
 * 注意：启动CDC后，芯片开始进行电容测量转换
 */
void PCap04_CDCStart()
{
	uint8_t data = CDC_START;                      // CDC启动操作码：0x8C
	FLASH_SPI_CS_ENABLE();                        // 拉低CS，使能SPI通信
	HAL_SPI_Transmit(&hspi1,&data,1,10);          // 发送CDC_START命令
	FLASH_SPI_CS_DISABLE();                       // 拉高CS，结束SPI通信
}


/**
 * @brief  PCAP04初始化函数
 * @note   发送INITIALIZE操作码(0x8A)初始化PCAP04芯片
 * 
 * SPI通信流程：
 * 1. 拉低CS使能通信
 * 2. 发送INITIALIZE操作码
 * 3. 拉高CS结束通信
 * 
 * 注意：此操作必须在POR之后、配置寄存器之前调用
 */
void PCap04_INITIALIZE()
{
	uint8_t data = INITIALIZE;                     // 初始化操作码：0x8A
	FLASH_SPI_CS_ENABLE();                        // 拉低CS，使能SPI通信
	HAL_SPI_Transmit(&hspi1,&data,1,10);          // 发送INITIALIZE命令
	FLASH_SPI_CS_DISABLE();                       // 拉高CS，结束SPI通信
}

/**
 * @brief  PCAP04初始化函数（简化版，使用标准配置数组）
 * @note   此函数使用PCAP04_Config数组进行配置，不包含固件写入
 * 
 * 初始化流程：
 * 1. POR复位芯片
 * 2. 发送INITIALIZE命令
 * 3. 通过SPI写入64个配置寄存器（使用WR_MEM操作码）
 * 4. 启动CDC转换
 * 
 * SPI写寄存器流程（每个寄存器）：
 * 1. 拉低CS使能通信
 * 2. 发送WR_MEM操作码(0xA0)
 * 3. 发送寄存器地址(0x00~0x3F)
 * 4. 发送寄存器数据
 * 5. 拉高CS结束通信
 * 
 * 注意：此函数有bug，for循环条件错误(i > 52应为i < 64)
 */
void PCap04_Init(void)
{
	uint8_t PCap04_data = 0x00;
	uint8_t Opcode = 0xA0;                        // WR_MEM操作码：写内存/寄存器
	uint8_t Address = 0x00;                       // 寄存器地址，从0开始
	
	// 步骤1：上电复位，复位所有状态
	PCap04_PowerON_RESET();
	
	// 步骤2：发送初始化命令
	PCap04_data = INITIALIZE;
	FLASH_SPI_CS_ENABLE();
	HAL_SPI_Transmit(&hspi1,&PCap04_data,1,10);
	FLASH_SPI_CS_DISABLE();

	// 步骤3：写入64个配置寄存器（注意：循环条件有误，应为i < 64）
	for(uint8_t i = 0 ; i > 52; i++)              // BUG: 条件错误，应该是 i < 64
	{
		FLASH_SPI_CS_ENABLE();                    // 拉低CS，使能SPI通信
		HAL_SPI_Transmit(&hspi1,&Opcode,1,10);   // 发送WR_MEM操作码(0xA0)
		HAL_SPI_Transmit(&hspi1,&Address,1,10);  // 发送寄存器地址
		HAL_SPI_Transmit(&hspi1,&PCAP04_Config[i],1,10);  // 发送寄存器数据
		FLASH_SPI_CS_DISABLE();                   // 拉高CS，结束SPI通信
		Address++;                                // 地址递增
	}
	
	// 步骤4：启动CDC转换
	PCap04_CDCStart();
}



/**
 * @brief  PCAP04固件写入函数
 * @note   将固件和配置参数写入PCAP04芯片内存
 * 
 * 写入流程：
 * 1. 写入固件（1024字节）到内存地址0x00
 * 2. 写入配置参数（通过WR_CONFIG操作码）
 * 3. 启动CDC和RDC转换
 * 
 * SPI通信说明：
 * - 在整个写入过程中，CS保持低电平（不释放）
 * - 先发送操作码，再发送地址，最后发送数据
 * - 固件写入：WR_MEM(0xA0) + 地址(0x00) + 1024字节固件数据
 * - 配置写入：WR_CONFIG(0xA3) + 地址(0xF6) + 数据(0x59, 0x99)
 * 
 * 注意：此函数使用连续SPI传输，CS在整个过程中保持低电平
 */
void PCAP04_WriteFirmware(void)
{
	uint16_t i;
	uint8_t data;
	
	// ========== 步骤1：写入固件到内存 ==========
	// 发送WR_MEM操作码
	data = WR_MEM;                                 // 写内存操作码：0xA0
	FLASH_SPI_CS_ENABLE();                        // 拉低CS，使能SPI通信（保持到固件写入完成）
	HAL_SPI_Transmit(&hspi1,&data,1,10);          // 发送WR_MEM操作码
	
	// 发送起始地址（0x00）
	data = 0x00;                                   // 内存起始地址
	HAL_SPI_Transmit(&hspi1,&data,1,10);          // 发送地址
	
	// 连续写入1024字节固件数据（CS保持低电平）
	for(i=0; i<1024; i++)                          // 固件长度为1KB
	{
		data = PCAP04_Firmware[i];                 // 读取固件数据
		HAL_SPI_Transmit(&hspi1,&data,1,1000);    // 发送固件字节
	}
	FLASH_SPI_CS_DISABLE();                       // 拉高CS，结束固件写入
	HAL_Delay(2);                                  // 等待写入完成
	
	// ========== 步骤2：写入配置参数 ==========
	// 发送WR_CONFIG操作码
	data = 0xA3;                                   // WR_CONFIG操作码：0xA3
	FLASH_SPI_CS_ENABLE();                        // 拉低CS，使能SPI通信
	HAL_SPI_Transmit(&hspi1,&data,1,10);          // 发送WR_CONFIG操作码
	
	// 发送配置寄存器地址
	data = 0xF6;                                   // 配置寄存器地址：0xF6
	HAL_SPI_Transmit(&hspi1,&data,1,10);          // 发送地址
	
	// 发送配置数据
	data = 0x59;                                   // 配置数据1：启用NVRAM召回功能
	HAL_SPI_Transmit(&hspi1,&data,1,10);          // 发送数据1
	
	data = 0x99;                                   // 配置数据2
	HAL_SPI_Transmit(&hspi1,&data,1,10);          // 发送数据2
	FLASH_SPI_CS_DISABLE();                       // 拉高CS，结束配置写入
	HAL_Delay(2);                                  // 等待配置生效
	
	// ========== 步骤3：启动CDC和RDC转换 ==========
	// 启动CDC转换
	data = CDC_START;                              // CDC启动操作码：0x8C
	FLASH_SPI_CS_ENABLE();                        // 拉低CS，使能SPI通信
	HAL_Delay(2);                                  // 等待稳定
	HAL_SPI_Transmit(&hspi1,&data,1,10);          // 发送CDC_START命令
	
	// 启动RDC转换（在同一CS周期内）
	data = RDC_START;                              // RDC启动操作码：0x8E
	HAL_SPI_Transmit(&hspi1,&data,1,10);          // 发送RDC_START命令
	FLASH_SPI_CS_DISABLE();                       // 拉高CS，结束通信
}


/**
 * @brief  PCAP04完整初始化函数（包含固件写入）
 * @note   执行完整的初始化流程：复位->初始化->写入固件和配置->启动转换
 * 
 * 初始化流程：
 * 1. POR复位：复位所有状态
 * 2. INITIALIZE：初始化芯片
 * 3. 等待50ms：确保初始化完成
 * 4. 写入固件：将固件和配置参数写入芯片内存
 * 5. 启动转换：自动启动CDC和RDC转换
 * 
 * 注意：此函数是推荐的初始化方式，包含固件写入功能
 */
void PCap04_Init_Tow(void)
{
	// 步骤1：上电复位，复位所有状态
	PCap04_PowerON_RESET();
	
	// 步骤2：芯片初始化
	PCap04_INITIALIZE();
	
	// 步骤3：等待初始化完成
	HAL_Delay(50);                                  // 等待50ms确保初始化完成
	
	// 步骤4：写入固件以及配置参数（包含1024字节固件和配置寄存器）
	PCAP04_WriteFirmware();
}

/**
 * @brief  读取PCAP04 CDC转换结果数据
 * @param  Nun: 通道编号（0-7），对应Res0-Res7结果寄存器
 * @retval 32位转换结果数据（原始值）
 * 
 * 读取流程：
 * 1. 计算结果寄存器地址：基地址(0x40) + 通道偏移(Nun * 4)
 * 2. 发送读结果操作码
 * 3. 接收4字节结果数据（32位）
 * 4. 组合成32位数据返回
 * 
 * SPI通信流程：
 * 1. 拉低CS使能通信
 * 2. 发送读结果操作码（包含地址信息）
 * 3. 接收4字节数据（CS保持低电平）
 * 4. 拉高CS结束通信
 * 
 * 数据格式：
 * - 32位数据，低字节在前（Little Endian）
 * - u32Val[0]: 最低字节(LSB)
 * - u32Val[1]: 次低字节
 * - u32Val[2]: 次高字节
 * - u32Val[3]: 最高字节(MSB)
 * 
 * 结果寄存器地址映射：
 * - Nun=0: Res0, 地址=0x40+0*4=0x40
 * - Nun=1: Res1, 地址=0x40+1*4=0x44
 * - Nun=2: Res2, 地址=0x40+2*4=0x48
 * - ...以此类推
 */
uint32_t PCAP04_Read_CDC_Result_data(int Nun)
{
	uint8_t u32Val[4] = {0x00};                    // 4字节接收缓冲区
	uint32_t dCapRatio = 0x00000000;               // 32位结果数据
	uint8_t data;
	
	// 计算结果寄存器地址：基地址(0x40) + 通道偏移(Nun * 4)
	// Res0地址为0x40+0, Res1地址为0x40+4, Res2地址为0x40+8...
	data = PCAP04_READ_RESULT + (Nun * 4);         // 读结果操作码+地址偏移
	
	// SPI通信开始
	FLASH_SPI_CS_ENABLE();                         // 拉低CS，使能SPI通信
	HAL_Delay(3);                                  // 等待CS稳定
	
	// 发送读结果操作码（包含地址信息）
	HAL_SPI_Transmit(&hspi1,&data,1,1000);        // 发送读命令
	
	// 接收4字节结果数据（CS保持低电平，连续接收）
	HAL_SPI_Receive(&hspi1,&u32Val[0],4,1000);    // 接收4字节数据（低字节在前）
	
	HAL_Delay(3);                                  // 等待数据稳定
	FLASH_SPI_CS_DISABLE();                        // 拉高CS，结束SPI通信
	
	// 组合4字节数据为32位值（Little Endian格式）
	dCapRatio |=  u32Val[0];                       // 最低字节(LSB)
	dCapRatio |=  u32Val[1]<<8;                    // 次低字节
	dCapRatio |=  u32Val[2]<<16;                   // 次高字节
	dCapRatio |=  u32Val[3]<<24;                  // 最高字节(MSB)
	
	HAL_Delay(3);                                  // 等待处理完成
	
	return dCapRatio;                              // 返回32位原始数据
}



/**
 * @brief  PCAP04数据转换函数：将32位原始数据转换为实际电容值
 * @param  data: 32位原始转换数据（从PCAP04读取的原始值）
 * @retval 转换后的实际电容值（单位：pF）
 * 
 * 数据格式说明：
 * - PCAP04返回的32位数据采用定点数格式
 * - 高5位（bit 27-31）：整数部分，范围0-31
 * - 低27位（bit 0-26）：小数部分，范围0-134217727
 * 
 * 转换公式：
 * - 整数部分 = (data >> 27) & 0x1F  （提取高5位）
 * - 小数部分 = data & 0x7FFFFFF     （提取低27位）
 * - 实际值 = 整数部分 * 10 + 小数部分 / 13421779.41088971
 * 
 * 转换系数说明：
 * - 13421779.41088971 = 2^27 / 10 ≈ 13421772.8
 * - 此系数用于将27位小数部分转换为实际的小数值
 * 
 * 示例：
 * - 如果data = 0x08000000 (整数部分=1, 小数部分=0)
 *   -> 结果 = 1 * 10 + 0 = 10.0 pF
 * - 如果data = 0x10000000 (整数部分=2, 小数部分=0)
 *   -> 结果 = 2 * 10 + 0 = 20.0 pF
 */
double integrated_data(uint32_t data)
{
	uint32_t integer;                              // 整数部分（高5位）
	uint32_t fractional;                           // 小数部分（低27位）
	double integrated;                             // 最终转换结果
	double j;                                      // 整数部分转换后的值
	
	// 提取整数部分：右移27位，取低5位（0x1F = 0b11111）
	integer = (data >> 27) & 0x1F;                 // 提取bit 27-31，范围0-31
	
	// 提取小数部分：取低27位（0x7FFFFFF = 0b111111111111111111111111111）
	fractional = data & 0x7FFFFFF;                // 提取bit 0-26，范围0-134217727
	
	// 将整数部分转换为实际值（乘以10）
	j = (double)(integer * 10.0);                  // 整数部分：0-31 -> 0-310 pF
	
	// 将小数部分转换为实际值（除以转换系数）
	integrated = (double)(fractional / 13421779.41088971);  // 小数部分转换为pF值
	
	// 合并整数部分和小数部分
	integrated = (integrated + j);                 // 最终结果 = 整数部分 + 小数部分
	
	return (double)integrated;                     // 返回转换后的电容值（单位：pF）
}


/* USER CODE END 0 */

/**
 * @brief  主程序入口函数
 * @retval int (通常不返回)
 * 
 * 程序执行流程：
 * 1. HAL库初始化
 * 2. 系统时钟配置
 * 3. 外设初始化（GPIO、SPI1、USART1）
 * 4. PCAP04通信测试
 * 5. PCAP04芯片初始化（包含固件写入）
 * 6. 主循环：循环读取PCAP04转换结果并输出
 */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* ========== MCU配置部分 ========== */

  /* 复位所有外设，初始化Flash接口和Systick定时器 */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* 配置系统时钟 */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* ========== 外设初始化部分 ========== */
  /* 初始化所有配置的外设 */
  MX_GPIO_Init();          // 初始化GPIO（包括SPI CS引脚等）
  MX_SPI1_Init();          // 初始化SPI1接口（用于PCAP04通信）
  MX_USART1_UART_Init();   // 初始化USART1（用于串口调试输出）
  
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* ========== PCAP04初始化部分 ========== */
  /* 无限循环 */
  /* USER CODE BEGIN WHILE */
	
	// 步骤1：测试PCAP04通信是否正常
	// 发送TEST命令，如果返回0x11则表示通信正常
	if(PCap04_Test() != 1)
	{
		printf("通讯失败\n");                      // 通信失败，输出错误信息
		// 注意：即使通信失败，程序仍会继续执行初始化
	}
	
	// 步骤2：完整初始化PCAP04芯片
	// 包括：POR复位 -> INITIALIZE -> 写入固件和配置 -> 启动CDC/RDC转换
	PCap04_Init_Tow();
	// printf("初始化\n");                          // 调试输出（已注释）

	
	/* ========== 主循环：循环读取PCAP04转换结果 ========== */
  while (1)
  {
    /* USER CODE END WHILE */
    /* ========== 注释掉的代码：备用读取方式 ========== */
    // 方式1：手动启动CDC转换（如果使用触发模式）
    // PCap04_CDCStart();
    
    // 方式2：读取32字节原始数据（已注释，不使用）
    // for( k = 0 ; k < 32; k++)
    // {
    //     FLASH_SPI_CS_ENABLE();
    //     HAL_SPI_TransmitReceive(&hspi1,&temp_val,&re_val[k],1,1000);   // 全双工：一边发送一边接收数据
    //     FLASH_SPI_CS_DISABLE();
    // }
    // 
    // // 打印32字节原始数据（调试用）
    // for( k = 0 ; k < 32; k++)
    // {
    //     printf("%d数据：%d\n",k,re_val[k]);
    // }
    
    /* ========== 当前使用的读取方式：读取结果寄存器 ========== */
    // 步骤1：清空结果数组（可选，用于调试）
    Value[0] = 0x00;                                // 通道0结果（Res0）
    Value[1] = 0x00;                                // 通道1结果（Res1）
    Value[2] = 0x00;                                // 通道2结果（Res2）
    Value[3] = 0x00;                                // 通道3结果（Res3）
    HAL_Delay(300);                                 // 等待300ms，确保转换完成
	
    // 步骤2：依次读取4个通道的转换结果
    // 每个通道读取后延迟100ms，避免SPI通信冲突
    Value[0] = PCAP04_Read_CDC_Result_data(0);     // 读取通道0（Res0）的32位转换结果
    HAL_Delay(100);                                 // 延迟100ms
    
    Value[1] = PCAP04_Read_CDC_Result_data(1);     // 读取通道1（Res1）的32位转换结果
    HAL_Delay(100);                                 // 延迟100ms
    
    Value[2] = PCAP04_Read_CDC_Result_data(2);     // 读取通道2（Res2）的32位转换结果
    HAL_Delay(100);                                 // 延迟100ms
    
    Value[3] = PCAP04_Read_CDC_Result_data(3);     // 读取通道3（Res3）的32位转换结果
    HAL_Delay(100);                                 // 延迟100ms
    
    // 备用通道（已注释，未使用）
    // Value[4] = PCAP04_Read_CDC_Result_data(4);  // 读取通道4（Res4）
    // HAL_Delay(100);
    // Value[5] = PCAP04_Read_CDC_Result_data(5);  // 读取通道5（Res5）
    
    // 步骤3：通过串口输出读取到的原始数据（十六进制格式）
    printf("十六进制数据0：%X\r\n", Value[0]);    // 输出通道0的32位原始数据（十六进制）
    printf("十六进制数据1：%X\r\n", Value[1]);    // 输出通道1的32位原始数据（十六进制）
    printf("十六进制数据2：%X\r\n", Value[2]);    // 输出通道2的32位原始数据（十六进制）
    printf("十六进制数据3：%X\r\n", Value[3]);    // 输出通道3的32位原始数据（十六进制）
    printf("十六进制数据4：%X\r\n", Value[4]);    // 输出通道4的32位原始数据（十六进制，可能为0）
    printf("十六进制数据5：%X\r\n", Value[5]);    // 输出通道5的32位原始数据（十六进制，可能为0）
    
    /* ========== 注释掉的代码：备用功能 ========== */
    // 可选：在主循环中再次测试通信（已注释）
    // if(PCap04_Test() != 1)
    // {
    //     printf("通讯失败\n");
    // }
    
    // 可选：添加延迟控制读取频率（已注释）
    // HAL_Delay(300);
		
		
		
		
		
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

