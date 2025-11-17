#include "pcap.h"
#include "stm32f10x_conf.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "STM32_VSPI.h"

 

 
 
//******************************************************************************
// 函数功能: 初始化SPC接口 
// 输入参数: 无
// 输出参数: 无
// 说    明: 无
//******************************************************************************
void VSPI_Configuration(void)
{					   
	// PB14----SPI1_SCK,PB15---SPI1_MISO,PB13  ----SPI1_MOSI  
  // 复位后，端口默认为浮空状态，置为高，起到稳定端口的作用 

  // PB13-SCK,PB14-MOSI,输出 
//  GPIO_SetPinMode(VSPI_MOSI_PORT, VSPI_MOSI_PIN, GPIO_Mode_Out_PP, GPIO_Speed_50MHz);
//  GPIO_SetPinMode(VSPI_SCL_PORT,  VSPI_SCL_PIN,  GPIO_Mode_Out_PP, GPIO_Speed_50MHz);

//  // PB15--输入 --MISO
//  GPIO_SetPinMode(VSPI_MISO_PORT, VSPI_MISO_PIN, GPIO_Mode_IPU, GPIO_Speed_50MHz);
	
  LED_Init();
  GPIO_SetBits(VSPI_MOSI_PORT, VSPI_MOSI_PIN);
  GPIO_ResetBits(VSPI_SCL_PORT,  VSPI_SCL_PIN);
   
}


//*****************************************************************************
// 函数功能: 发送一个字节数据
// 输入参数: 无
// 输出参数: 无
// 说    明: 必须在SCK的上升沿写入数据 
//******************************************************************************
u8 VSPI_SendReadByte(u8  sdata)
{
    u8 i,rda;
     
	for(i=0; i<8; i++)
	{	
		VSPI_SCL_H();
//		
		if(sdata & 0x80)VSPI_MOSI_H();	  	 	//高位在前，上升沿准备数据，待下降沿写入数据
		else 			VSPI_MOSI_L();
		sdata<<=1;
		rda<<=1;
//		delay_us(1);
		if(VSPI_MISO_READ())rda |= 0x01;	//上升沿读取数据
		else 				rda &= 0xfe;
		VSPI_SCL_L(); 
//		delay_us(200); 		 		
	}
	return rda;
}

void VSPI_SendByte(u8  sdata)
{
	VSPI_SendReadByte(sdata);
}


//******************************************************************************
// 函数功能: 读取一个字节数据 
// 输入参数: 无
// 输出参数: 无
// 说    明: 无
//******************************************************************************
u8 VSPI_ReadByte(void)
{
	u8 rda=0;

	rda=VSPI_SendReadByte(0x00);
    return rda;
}
