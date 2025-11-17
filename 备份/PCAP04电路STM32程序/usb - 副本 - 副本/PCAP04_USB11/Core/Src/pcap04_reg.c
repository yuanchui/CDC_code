/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : pcap04_reg.c
  * @brief          : PCAP04 register configuration using register definitions
  ******************************************************************************
  * @attention
  *
  * This file provides PCAP04_Config[] array built using register definitions
  * from pcap04_reg.h, allowing bit-level configuration of registers.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "pcap04_reg.h"

// PC环境编译支持（用于在PC上测试配置数组）
#ifdef __GNUC__
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
// PC环境不需要STM32特定的头文件
#else
// STM32环境
#include "pcap04.h"  // For SPI interface definitions
#include "spi.h"     // For hspi2
#include "main.h"    // For GPIO definitions
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/**
  * @brief  PCAP04 default configuration items (not in order)
  *         使用功能位宏构建，不按寄存器顺序排列  
  */
static const PCAP04_RegConfigItem_t PCAP04_DefaultConfigItems[] = {
    //******************CDC配置相关（0-19）******************** */
    // Register 0 (0x00) - IIC地址与低频时钟配置
    // 计算: I2C_A=1(0x40) | OLF_FTUNE=5(0x14) | OLF_CTUNE=3(0x03) = 0x57
    {PCAP04_REG_ADDR_0,
     PCAP04_REG0_VALUE(
         0,                              // 地址位I2C_A = 1 (修改后)
         5,                              // 低频时钟微调 OLF_FTUNE = 5
         PCAP04_REG0_OLF_CTUNE_200k     // 低频时钟粗调 OLF_CTUNE = 200kHz (3)
     )},  // = 0x57 (I2C_A=1时) 或 0x17 (I2C_A=0时)
    
    // Register 1 (0x01) - 外部晶振时钟OX配置
    // 注意：OX_AUTOSTOP_DIS和OX_STOP为ams内部位，使用默认值0，无需配置
    {PCAP04_REG_ADDR_1,
     PCAP04_REG1_VALUE(
         PCAP04_REG1_OX_DIS_ENABLE,        // 禁用OX时钟 OX_DIS = 启用
         PCAP04_REG1_OX_DIV4_NO_DIV,       // OX时钟4分频 OX_DIV4 = 不分频
         PCAP04_REG1_OX_RUN_OFF      // 控制OX发生器的持续性或延迟 OX_RUN = 永久运行
     )},  // = 0x00
    
    // Register 2 (0x02) - 端口放电电阻配置
    {PCAP04_REG_ADDR_2,
     PCAP04_REG2_VALUE(
         PCAP04_REG2_RDCHG_INT_SEL0_180K,     // PC4~PC5的片上放电电阻选择 RDCHG_INT_SEL1 = 180kΩ
         PCAP04_REG2_RDCHG_INT_SEL0_10K,      // PC0~PC3和内部端口PC6的片上放电电阻选择 RDCHG_INT_SEL0 = 10kΩ
         PCAP04_REG2_RDCHG_INT_EN_ENABLE,     // 内部放电电阻使能 RDCHG_INT_EN = 开启
         PCAP04_REG2_RDCHG_EXT_EN_DISABLE     // 外部放电电阻使能 RDCHG_EXT_EN = 关闭
     )},  // = 0x38
    
    // Register 3 (0x03) - 端口充放电电阻配置
    {PCAP04_REG_ADDR_3,
     PCAP04_REG3_VALUE(
         PCAP04_REG3_AUX_PD_DIS_DISABLE,      // 禁用PCAUX下拉电阻 AUX_PD_DIS = 激活 (下拉激活)
         0,                                  // 仅在内部参考转换期间激活辅助端口PCAUX AUX_CINT = 0 (正常)
         0,                                  // 保持芯片内部放电电阻永久连接 RDCHG_PERM_EN = 0 (关闭)
         0,                                  // 永久激活辅助端口PCAUX RDCHG_EXT_PERM = 0 (关闭)
         PCAP04_REG3_RCHG_SEL_180K          // 选择充电电阻 RCHG_SEL = 180kΩ
     )},  // = 0x10
    
    // Register 4 (0x04) - 端口电容连接方式
    {PCAP04_REG_ADDR_4,
     PCAP04_REG4_VALUE(
         PCAP04_REG4_C_REF_INT_EXTERNAL,     // 在PC0/GND或PC0/PC1使用片上参比电容 C_REF_INT = 外部参考
         PCAP04_REG4_C_COMP_EXT_ACTIVE,      // 激活外部寄生电容补偿机制 C_COMP_EXT = 激活
         PCAP04_REG4_C_COMP_INT_ACTIVE,      // 激活片上寄生电容补偿机制和增益补偿 C_COMP_INT = 激活
         PCAP04_REG4_C_DIFFERENTIAL_SINGLE_ENDED,  // 在单端测量和差分测量之间选择 C_DIFFERENTIAL = 单端
         PCAP04_REG4_C_FLOATING_FLOATING     // 选择接地测量或浮动测量 C_FLOATING = 浮动
     )},  // = 0x31
    
    // Register 5 (0x05) - CDC时钟与端口模式配置
    {PCAP04_REG_ADDR_5,
     PCAP04_REG5_VALUE(
         PCAP04_REG5_CY_PRE_MR1_SHORT_NORMAL,  // 减少内部时钟路径之间的延迟 CY_PRE_MR1_SHORT = 正常
         PCAP04_REG5_C_PORT_PAT_NORMAL,        // 端口测量顺序交替 C_PORT_PAT = 正常
         PCAP04_REG5_CY_HFCLK_SEL_OLF,        // 选择CDC的时钟源 CY_HFCLK_SEL = OLF
         PCAP04_REG5_CY_DIV4_DIS_DISABLE,      // 改变时间周期 CY_DIV4_DIS = 关闭
         PCAP04_REG5_CY_PRE_LONG_DISABLE,      // 在内部时钟路径之间添加安全延迟 CY_PRE_LONG = 关闭
         PCAP04_REG5_C_DC_BALANCE_DISABLE      // 仅用于差分浮动模式 C_DC_BALANCE = 关闭
     )},  // = 0x00
    
    // Register 6 (0x06) - 测量端口使能配置
    // 启用PC0~PC3的CDC端口
    {PCAP04_REG_ADDR_6,
     PCAP04_REG6_VALUE(
         PCAP04_REG6_C_PORT_EN_PC0 |
         PCAP04_REG6_C_PORT_EN_PC1 |
         PCAP04_REG6_C_PORT_EN_PC2 |
         PCAP04_REG6_C_PORT_EN_PC3
     )},  // = 0x0F
    
    // Register 7-8 (0x07-0x08) - C_AVRG采样平均配置（13位，跨两个寄存器）
    // C_AVRG[12:0]: 寄存器7[7:0]和寄存器8[4:0]，设置CDC测量的平均采样数（范围: 0-8191）值越大采样值越平稳
    PCAP04_C_AVRG_CONFIG(2),  // C_AVRG = 2，影响寄存器7[7:0]和寄存器8[4:0]
    
    // Register 9-11 (0x09-0x0B) - CONV_TIME转换时间配置（23位，三个寄存器）
    // CONV_TIME[22:0]: 寄存器9[7:0]、寄存器10[7:0]和寄存器11[6:0]，转换触发周期或序列周期（范围: 0-0x7FFFFF）
    // 公式: Tconv/seq = 2 × CONV_TIME / fOLF，例如: CONV_TIME = 500, OLF = 200 kHz, 则 Tconv = 2 × 500 / 200000 = 5 ms
    // 目标值: Reg9=0xF4, Reg10=0x01, Reg11=0x00 -> CONV_TIME = 0x01F4 = 500
    PCAP04_CONV_TIME_CONFIG(500),  //转换时间
    
    // Register 12-13 (0x0C-0x0D) - DISCHARGE_TIME放电时间配置（10位，跨两个寄存器）
    // DISCHARGE_TIME[9:0]: 设置CDC放电时间（范围: 0-1023）1023表示关闭
    // OLF模式: Tdischarge = (DISCHARGE_TIME + 1) × Tcycleclock OHF模式: Tdischarge = DISCHARGE_TIME × Tcycleclock
    // 目标值: Reg12=0x7F, Reg13=0x08 -> DISCHARGE_TIME = 127, C_TRIG_SEL = 定时器触发(0x02), C_STARTONPIN = PG0(0x00)
    PCAP04_DISCHARGE_TIME_AND_REG13_CONFIG(
        127,                                    // 放电时间 寄存器12[7:0]和寄存器13[1:0] = 0x7F
        PCAP04_REG13_C_STARTONPIN_PG0,         // 允许触发CDC启动的GPIO端口
        PCAP04_REG13_C_TRIG_SEL_TIMER_TRIGGER  // CDC触发模式 = 定时器触发 (0x02)
    ),
    
    // Register 14-15 (0x0E-0x0F) - PRECHARGE_TIME预充电时间配置（10位，跨两个寄存器）
    // PRECHARGE_TIME[9:0]: 设置CDC预充电时间（范围: 0-1023）1023表示关闭
    // OLF模式: Tprecharge = (PRECHARGE_TIME + 1) × Tcycleclock
    // OHF模式 & FULLCHARGE_TIME = 1023: Tprecharge = (PRECHARGE_TIME + 2) × Tcycleclock
    //OLF模式 & FULLCHARGE_TIME != 0x3FF: Tprecharge = (PRECHARGE_TIME + 1) × Tcycleclock
    // 目标值: Reg14=0xFF, Reg15=0x03 -> PRECHARGE_TIME = 1023 (关闭), C_FAKE = 0
    PCAP04_PRECHARGE_TIME_AND_REG15_CONFIG(
        1023,  // 预充电时间 寄存器14[7:0]和寄存器15[1:0] = 0x3FF (关闭)
        0      // 无假测量
    ),
    
    // Register 16-17 (0x10-0x11) - FULLCHARGE_TIME满充电时间配置（10位，跨两个寄存器）
    // FULLCHARGE_TIME[9:0]: 设置CDC满充电时间（范围: 0-1023）
    // OLF模式: Tfullcharge = (FULLCHARGE_TIME + 1) × Tcycleclock
    // OHF模式: Tfullcharge = (FULLCHARGE_TIME + 2) × Tcycleclock
    // 注意：只需输入一次FULLCHARGE_TIME值，宏会自动设置到寄存器16[7:0]和寄存器17[1:0]，同时设置C_REF_SEL
    // Reg 17 = 0x28 = 0b00101000, C_REF_SEL[6:2] = 0b00101 = 5, FULLCHARGE_TIME[1:0] = 0b00 = 0
    // 所以 FULLCHARGE_TIME = 0, C_REF_SEL = 5
    PCAP04_FULLCHARGE_TIME_AND_REG17_CONFIG(
        0,   // 满充电时间 寄存器16[7:0]和寄存器17[1:0]
        5    // 内部参考电容大小 步长从0.3 pF到1.5 pF不等
    ),
    
    // Register 18 (0x12) - 保护驱动配置
    // 位[7] C_G_OP_RUN: 保护运算放大器模式 (0: 永久, 1: 脉冲)
    // 位[6] C_G_OP_EXT: 保护外部运算放大器 (0: 内部OP, 1: 外部OP)
    // 位[5:0] C_G_EN: 保护使能，每位对应一个端口 (位0: PC0, 位1: PC1, ..., 位5: PC5)
    {PCAP04_REG_ADDR_18,
     PCAP04_REG18_VALUE(
         PCAP04_REG18_C_G_OP_RUN_PERMANENT,  // 保护运算放大器模式 = 永久
         PCAP04_REG18_C_G_OP_EXT_INTERNAL,   // 保护外部运算放大器 = 内部OP
         PCAP04_REG18_C_G_EN_NONE            // 保护使能 = 未启用（所有端口禁用）
     )},  // = 0x00 (保护功能未启用)
    
    // Register 19 (0x13) - 保护运算放大器配置
    // 位[7:6] C_G_OP_VU: 保护运算放大器增益 (0: ×1.00, 1: ×1.01, 2: ×1.02, 3: ×1.03)
    // 位[5:4] C_G_OP_ATTN: 保护运算放大器衰减 (0: 0.5 aF, 1: 1.0 aF, 2: 1.5 aF, 3: 2.0 aF)
    // 位[3:0] C_G_TIME: 保护端口切换时间 (t = C_G_TIME × 500 ns)
    {PCAP04_REG_ADDR_19,
     PCAP04_REG19_VALUE(
         PCAP04_REG19_C_G_OP_VU_1_00,        // 保护运算放大器增益 = ×1.00
         PCAP04_REG19_C_G_OP_ATTN_0_5_AF,    // 保护运算放大器衰减 = 0.5 aF
         0                                   // 保护端口切换时间 = 0 (默认值)
     )},  // = 0x00 (所有位为默认值)
    
//****************RDC配置相关（20-39）**************** */
    // Register 20 (0x14) - RDC时间配置
    // 位[7] R_CY: RDC测定的循环时间（取决于OLF频率，R_CY=0: 较短循环时间, R_CY=1: 较长循环时间）
    // 位[2:0] C_G_OP_TR: 保护运算放大器电流调整 (范围: 0-7，推荐值)
    {PCAP04_REG_ADDR_20,
     PCAP04_REG20_VALUE(
         PCAP04_REG20_R_CY_SHORT,            // RDC循环时间 = 较短循环时间
         0                                   // 保护运算放大器电流调整 = 0 (默认值)
     )},  // = 0x00
    
    // Register 21-22 (0x15-0x16) - R_TRIG_PREDIV RDC预分频配置（10位，跨两个寄存器）
    // R_TRIG_PREDIV[9:0]: 预分频器（范围: 0-1023）
    // 允许温度测量比电容测量慢，0/1=每次信号触发，2=每2次信号触发，...，1023=最大因子
    // 目标值: Reg21=0x01, Reg22=0x00 -> R_TRIG_PREDIV = 1, R_AVRG = 不平均(0x00), R_TRIG_SEL = 关闭(0x00)
    PCAP04_R_TRIG_PREDIV_AND_REG22_CONFIG(
        1,                                      // RDC预分频 寄存器21[7:0]和寄存器22[1:0] = 0x01
        PCAP04_REG22_R_AVRG_NO_AVG,            // RDC测定的平均值配置 R_AVRG[3:2] = 不平均
        PCAP04_REG22_R_TRIG_SEL_DISABLE        // RDC触发源选择 R_TRIG_SEL[6:4] = 关闭 (0x00)
    ),
    
    // Register 23 (0x17) - RDC端口与参考配置
    {PCAP04_REG_ADDR_23,
     PCAP04_REG23_VALUE(
         PCAP04_REG23_R_PORT_EN_DISABLE,        // RDC部分的端口激活 R_PORT_EN = 禁用
         PCAP04_REG23_R_PORT_EN_IMES_DISABLE,    // 内部铝温度传感器的端口激活 R_PORT_EN_IMES = 禁用
         PCAP04_REG23_R_PORT_EN_IREF_ACTIVE,    // 内部参考电阻的端口激活 R_PORT_EN_IREF = 激活
         PCAP04_REG23_R_FAKE_2_CYCLES,          // RDC的"假"或"预热"测量数量 R_FAKE = 2次周期
         PCAP04_REG23_R_STARTONPIN_PG0          // 选择允许触发RDC启动的GPIO端口 R_STARTONPIN = PG0
     )},  // = 0x30
    
    // ============================================================================
    // 固定值寄存器（ams内部固定配置，必须按照数据手册设置，无需更改）
    // Register 24 (0x18) - TDC配置（固定值，必须为0x73
    {PCAP04_REG_ADDR_24, PCAP04_TDC_REG24_DEFAULT},  // = 0x73 (固定值，无需更改)
    // Register 25 (0x19) - TDC配置（固定值，必须为0x04
    {PCAP04_REG_ADDR_25, PCAP04_TDC_REG25_DEFAULT},  // = 0x04 (固定值，无需更改)
    // Register 26 (0x1A) - TDC配置（固定值，必须为0x50）
    {PCAP04_REG_ADDR_26, PCAP04_TDC_REG26_DEFAULT},  // = 0x50 (固定值，无需更改)
    // ============================================================================

    // Register 27 (0x1B) - DSP配置
    {PCAP04_REG_ADDR_27,
     PCAP04_REG27_VALUE(
         PCAP04_REG27_DSP_MOFLO_EN_DISABLE,      // GPIO输出数据使能的动态范围溢出 DSP_MOFLO_EN = 关闭
         PCAP04_REG27_DSP_SPEED_FASTEST,         // DSP速度 DSP_SPEED = 最快
         PCAP04_REG27_PG1xPG3_NORMAL,           // PDM/PWM输出引脚为PG3
         PCAP04_REG27_PG0xPG2_NORMAL            // PDM/PWM输出引脚为PG2
     )},  // = 0x08
    
    // Register 28 (0x1C) - 看门狗配置
    {PCAP04_REG_ADDR_28, PCAP04_REG28_WD_DIS_bit(PCAP04_REG28_WD_DIS_DISABLE)},  //看门狗禁用
    
    // Register 29 (0x1D) - DSP触发配置
    {PCAP04_REG_ADDR_29,
     PCAP04_REG29_VALUE(
         0,  // 启动DSP的引脚掩码 DSP_STARTONPIN = 0 (无引脚)
         0   // 触发器激活的引脚掩码 DSP_FF_IN = 0 (无输入)
     )},  // = 0x00
    
    // Register 30 (0x1E) - 中断配置
    {PCAP04_REG_ADDR_30,
     PCAP04_REG30_VALUE(
         PCAP04_REG30_PG5_INTN_EN_ROUTE,        // 将INTN信号路由到PG5 PG5_INTN_EN = 路由
         PCAP04_REG30_PG4_INTN_EN_NORMAL,       // 将INTN信号路由到PG4 PG4_INTN_EN = 正常
         PCAP04_REG30_DSP_START_EN_RDC_END      // DSP触发器启用 DSP_START_EN = 由RDC结束触发
     )},  // = 0x82
    
    // Register 31 (0x1F) - 脉冲接口0配置
    {PCAP04_REG_ADDR_31,
     PCAP04_REG31_VALUE(
         PCAP04_REG31_PI1_TOGGLE_EN_NORMAL,     // 在脉冲接口1输出激活切换触发器
         PCAP04_REG31_PIO_TOGGLE_EN_NORMAL,     // 在脉冲接口0输出激活切换触发器
         PCAP04_REG31_PIO_RES_10BIT,            // 脉冲编码接口0的分辨率
         PCAP04_REG31_PIO_PDM_SEL_PWM,          // PWM/PDM
         PCAP04_REG31_PIO_CLK_SEL_OFF           // 脉冲接口0时钟选择
     )},  // = 0x08
    
    // Register 32 (0x20) - 脉冲接口1配置
    {PCAP04_REG_ADDR_32,
     PCAP04_REG32_VALUE(
         PCAP04_REG32_PI1_RES_10BIT,            // 脉冲接口1的分辨率
         PCAP04_REG32_PI1_PDM_SEL_PWM,          // 脉冲接口1 PWM / PDM开关
         PCAP04_REG32_PI1_CLK_SEL_OFF           // 脉冲接口1时钟选择
     )},  // = 0x08
    
    // Register 33 (0x21) - GPIO配置
    // 位[7:4] PG_DIR_IN: 通用端口方向切换（输入/输出）
    //  位4: PG0方向 (0=输出, 1=输入)
    //  位5: PG1方向 (0=输出, 1=输入)
    //  位6: PG2方向 (0=输出, 1=输入)
    //  位7: PG3方向 (0=输出, 1=输入)
    // 位[3:0] PG_PU: 在通用端口激活保护上拉电阻
    //  位0: PG0上拉 (0=禁用, 1=激活)
    //  位1: PG1上拉 (0=禁用, 1=激活)
    //  位2: PG2上拉 (0=禁用, 1=激活)
    //  位3: PG3上拉 (0=禁用, 1=激活)
    // 注意：pg_dir_in和pg_pu参数可以是位掩码的组合（使用 | 操作符）
    // 例如：设置PG0和PG1为输入，PG0上拉启用：
    //   pg_dir_in = PCAP04_REG33_PG0_DIR_INPUT | PCAP04_REG33_PG1_DIR_INPUT
    //   pg_pu = PCAP04_REG33_PG0_PU_ENABLE
    {PCAP04_REG_ADDR_33,
     PCAP04_REG33_VALUE(
         0,  // PG_DIR_IN = 0 (所有端口为输出模式，默认值)
         0   // PG_PU = 0 (所有端口上拉禁用，默认值)
     )},  // = 0x00 (所有GPIO端口为输出模式，上拉禁用)
    
    // Register 34 (0x22) - 带隙与自动启动配置
    // 注意：AMS_INTERNAL[3:0] = 7 (必须为7，固定值，自动设置，无需用户输入)
    {PCAP04_REG_ADDR_34,
     PCAP04_REG34_VALUE(
         PCAP04_REG34_INT_TRIG_BG_DISABLE,      // 读取结束触发带隙
         PCAP04_REG34_DSP_TRIG_BG_ENABLE,       // 带隙刷新由DSP位设置触发
         PCAP04_REG34_BG_PERM_PULSE_MODE,       // 激活带隙永久启用
         PCAP04_REG34_AUTOSTART_DISABLE         // 在上电后触发CDC
     )},  // = 0x47 (AMS_INTERNAL自动设置为7)
    
    // Register 35 (0x23) - 增益缩放因子
    //**公式**: 增益 = 1 + n/256
    {PCAP04_REG_ADDR_35, PCAP04_REG35_CDC_GAIN_CORR_bit(PCAP04_REG35_CDC_GAIN_CORR_1_25)},  
    
    // Register 36 (0x24) - 未使用（固定值，必须为0x00，无需更改）
    {PCAP04_REG_ADDR_36, 0x00},
    
    // Register 37 (0x25) - 未使用（固定值，必须为0x00，无需更改）
    {PCAP04_REG_ADDR_37, 0x00},
    
    // Register 38 (0x26) - 带隙时间配置
    {PCAP04_REG_ADDR_38, 0x00},
    
    // Register 39 (0x27) - 脉冲选择配置
    {PCAP04_REG_ADDR_39,
     PCAP04_REG39_VALUE(
         PCAP04_REG39_PULSE_SEL1_RES7,          // 为脉冲接口1选择源
         PCAP04_REG39_PULSE_SEL0_RES1           // 为脉冲接口0选择源
     )},  // = 0x71
    //******************* 固件和DSP相关（40-63）********************* */
    // Register 40 (0x28) - 电容感测选择
    {PCAP04_REG_ADDR_40, 0x00},
    
    // Register 41 (0x29) - 电阻感测选择
    {PCAP04_REG_ADDR_41, 0x00},
    
    // Register 42 (0x2A) - 固件配置与报警选择
    // 注意：HS_MODE_SEL[2] = 0 (必须为0，固定值)
    {PCAP04_REG_ADDR_42,
     PCAP04_REG42_VALUE(
         PCAP04_REG42_ALARM1_SELECT_Z,           // 报警1选择
         PCAP04_REG42_ALARM0_SELECT_Z,           // 报警0选择
         PCAP04_REG42_EN_ASYNC_READ_DISABLE,     // 禁用异步读取结果寄存器Res0到Res7中的值仅在读取先前值后才更新
         PCAP04_REG42_R_MEDIAN_EN_ENABLE,        // 启用R的中值滤波器
         PCAP04_REG42_C_MEDIAN_EN_ENABLE         // 启用C的中值滤波器
     )},  // = 0x03 (HS_MODE_SEL自动设置为0)
    
    // Register 43-46 (0x2B-0x2E) - 保留寄存器（固定值，必须为0x00，无需更改）
    // 注意: 目标值要求Reg43=0x08，但这是保留寄存器，通常应为0x00。根据目标配置设置为0x08
    {PCAP04_REG_ADDR_43, 0x08},  // 目标值要求为0x08
    {PCAP04_REG_ADDR_44, 0x00},
    {PCAP04_REG_ADDR_45, 0x00},
    {PCAP04_REG_ADDR_46, 0x00},
    
    // Register 47 (0x2F) - 运行位配置
    //在编程和任何寄存器修改期间应设置为"关闭"，以保护芯片免受任何不期望/未指定状态的影响
    {PCAP04_REG_ADDR_47, PCAP04_REG47_RUNBIT_bit(PCAP04_REG47_RUNBIT_ON)},  // = 0x01
    
    // Register 48 (0x30) - 内存锁配置
    {PCAP04_REG_ADDR_48, 0x00},
    // Register 49-50 (0x31-0x32) - 序列号（只读寄存器，默认0x00）
    {PCAP04_REG_ADDR_49, 0x00},
    {PCAP04_REG_ADDR_50, 0x00},
    // Register 51-53 (0x33-0x35) - ams内部寄存器（固定值，必须为0x00，无需更改）
    {PCAP04_REG_ADDR_51, 0x00},
    {PCAP04_REG_ADDR_52, 0x00},
    {PCAP04_REG_ADDR_53, 0x00},
    // Register 54 (0x36) - 内存控制（固定值，必须为0x00，无需更改）
    {PCAP04_REG_ADDR_54, 0x00},
    // Register 55-61 (0x37-0x3D) - ams内部寄存器（固定值，必须为0x00，无需更改）
    {PCAP04_REG_ADDR_55, 0x00},
    {PCAP04_REG_ADDR_56, 0x00},
    {PCAP04_REG_ADDR_57, 0x00},
    {PCAP04_REG_ADDR_58, 0x00},
    {PCAP04_REG_ADDR_59, 0x00},
    {PCAP04_REG_ADDR_60, 0x00},
    {PCAP04_REG_ADDR_61, 0x00},
    // Register 62-63 (0x3E-0x3F) - 电荷泵配置（固定值，设备特定设置，禁止更改）
    {PCAP04_REG_ADDR_62, 0x00},  // 电荷泵配置低字节（固定值，禁止更改）
    {PCAP04_REG_ADDR_63, 0x00}   // 电荷泵配置高字节（固定值，禁止更改）
};

/**
 * @brief  PCAP04 configuration array (Reg 0-63)
 *         通过PCAP04_BuildConfigArray函数从不按顺序的配置项构建
 *         配置项不按寄存器顺序排列，通过PCAP04_BuildConfigArray函数自动排序到对应位置
 */
unsigned char PCAP04_Config[64];

/**
 * @brief  初始化PCAP04默认配置数组
 *         从不按顺序的配置项构建完整的配置数组
 * 
 * @note 此函数应在系统初始化时调用一次
 * @note 配置项使用功能位宏构建，自动移位到对应位置并组合
 */
void PCAP04_InitConfig(void)
{
    // 从不按顺序的配置项构建配置数组
    PCAP04_BuildConfigArray(PCAP04_DefaultConfigItems,
                            sizeof(PCAP04_DefaultConfigItems) / sizeof(PCAP04_DefaultConfigItems[0]),
                            PCAP04_Config,
                            NULL);
}

/**
 * @brief  获取PCAP04默认配置数组（向后兼容函数）
 * @return 指向配置数组的指针（64字节）
 * 
 * @note 此函数在首次调用时自动初始化配置数组
 */
const unsigned char* PCAP04_GetConfig(void)
{
    static uint8_t initialized = 0;
    
    if (!initialized) {
        PCAP04_InitConfig();
        initialized = 1;
    }
    
    return PCAP04_Config;
}

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  将寄存器配置项数组转换为PCAP04_Config数组
 * @param  config_items 寄存器配置项数组（不要求按顺序）
 * @param  item_count 配置项数量
 * @param  output_config 输出的PCAP04_Config数组（64字节）
 * @param  base_config 基础配置数组（可选，如果为NULL则使用默认值0x00）
 * @retval 0表示成功，非0表示错误
 */
int32_t PCAP04_BuildConfigArray(const PCAP04_RegConfigItem_t *config_items, 
                                 uint8_t item_count,
                                 uint8_t *output_config,
                                 const uint8_t *base_config)
{
    uint8_t i;
    
    // 参数检查
    if (config_items == NULL || output_config == NULL) {
        return -1;  // 参数错误
    }
    
    // 如果提供了基础配置，先复制基础配置
    if (base_config != NULL) {
        for (i = 0; i < 64; i++) {
            output_config[i] = base_config[i];
        }
    } else {
        // 否则初始化为0x00
        for (i = 0; i < 64; i++) {
            output_config[i] = 0x00;
        }
    }
    
    // 应用配置项（不按顺序）
    for (i = 0; i < item_count; i++) {
        // 检查寄存器地址是否有效
        if (config_items[i].reg_addr < 64) {
            // 将配置值写入对应位置
            output_config[config_items[i].reg_addr] = config_items[i].value;
        }
    }
    
    return 0;  // 成功
}

/**
 * @brief  打印PCAP04配置数组内容（用于PC环境调试）
 *         输出所有64个寄存器的地址和值
 * 
 * @note 使用标准C库printf输出，可在PC环境编译运行
 * @note 调用示例：PCAP04_PrintConfig();
 * 
 * @warning 重要提示：
 *   - 此函数会调用 PCAP04_InitConfig() 重新构建配置数组
 *   - 如果您修改了 PCAP04_DefaultConfigItems[]，确保已重新编译项目
 *   - 此函数显示的是从 PCAP04_DefaultConfigItems[] 构建的 PCAP04_Config[64] 数组
 */
void PCAP04_PrintConfig(void)
{
    uint8_t i;
    
    // 确保配置已初始化（重新构建配置数组）
    PCAP04_InitConfig();
    
#ifdef __GNUC__
    
    // 同时以16字节一行的格式输出（便于对比原始数组）
    printf("\nRaw Data (16 bytes per line):\n");
    for (i = 0; i < 64; i += 16) {
        printf("0x%02X-0x%02X: ", i, i + 15);
        for (uint8_t j = 0; j < 16 && (i + j) < 64; j++) {
            printf("%02X ", PCAP04_Config[i + j]);
        }
        printf("\n");
    }
    printf("\n");
    
    // 输出为C数组格式，便于复制
    printf("C Array Format:\n");
    printf("unsigned char PCAP04_Config[64] = {\n");
    for (i = 0; i < 64; i += 16) {
        printf("    ");
        for (uint8_t j = 0; j < 16 && (i + j) < 64; j++) {
            printf("0x%02X", PCAP04_Config[i + j]);
            if ((i + j) < 63) {
                printf(",");
            }
            if (j < 15 && (i + j) < 63) {
                printf(" ");
            }
        }
        if (i + 16 < 64) {
            printf("\n");
        }
    }
    printf("\n};\n");
#else
    // STM32环境可以留空或使用其他输出方式
    (void)i;  // 避免未使用变量警告
#endif
}

