
/******************************************************************
**	  金龙107开发板（V1.0）
**	  w25q16.c读写文件
**
**	  论    坛：bbs.openmcu.com
**	  旺    宝：www.openmcu.com
**	  邮    箱：support@openmcu.com
**
**    版    本：V1.1
**	  作    者：openmcu
**	  完成日期:	2014.3.14
********************************************************************/#include "w25q16.h"

#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "pcap.h"

/**************************************************

*函数名称：u8 SPIx_ReadWriteByte(u8 TxData)
*
*入口参数：TxData 要写入的字节
*
*出口参数：RxData 读取到的字节
*
*功能说明：
***************************************************/
u8 SPIx_ReadWriteByte(u8 TxData)
{		
	uint8_t RxData = 0;
	while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);	 //等待发送缓冲区空 
	SPI_I2S_SendData(SPI1, TxData);	                                 //发一个字节

    while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);	 //等待数据接收	 
	RxData = SPI_I2S_ReceiveData(SPI1);	                             //返回接收到的数据*/

    return (uint8_t)RxData;				    
}

/**************************************************

*函数名称：void W25Q16_Configuration(void)
*
*入口参数：无
*
*出口参数：无
*
*功能说明：端口和SPI的配置
***************************************************/
void SPI_Configuration(void)
{ 
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOC |RCC_APB2Periph_GPIOB | RCC_APB2Periph_SPI1, ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOD, ENABLE);

    GPIO_InitStructure.GPIO_Pin = PCAP_SCK_PIN | PCAP_MOSI_PIN |PCAP_SSN_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(PCAP_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PCAP_MISO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(PCAP_PORT, &GPIO_InitStructure);

	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
    
}


u8 SPIx_WritereadWORD(u16 TxData)
{		
    u8 tmp = 0;
    SPIx_ReadWriteByte((u8)((TxData)>>8));   
    tmp = SPIx_ReadWriteByte((u8)TxData); 
	return tmp;
}

u8 SPIx_WritereadDWORD(u32 TxData)
{	
    u8 tmp = 0;
    SPIx_ReadWriteByte((u8)((TxData)>>24));   
    SPIx_ReadWriteByte((u8)((TxData)>>16));   
    SPIx_ReadWriteByte((u8)((TxData)>>8));   
    tmp = SPIx_ReadWriteByte((u8)TxData); 
	return tmp;
}

void SPI1_WriteByte(u8 TxData)
{
   while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET){}//等待发送区空  
	
	SPI_I2S_SendData(SPI1, TxData); //通过外设SPIx发送一个byte  数据
}

u8 SPI1_ReadByte(void)
{
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET){} //等待接收完一个byte  
 
	return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据
}

void SPI1_SetSpeed(u8 SPI_BaudRatePrescaler)
{
  assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));//判断有效性
	SPI1->CR1&=0XFFC7;//位3-5清零，用来设置波特率
	SPI1->CR1|=SPI_BaudRatePrescaler;	//设置SPI1速度 
	SPI_Cmd(SPI1,ENABLE); //使能SPI1
} 



