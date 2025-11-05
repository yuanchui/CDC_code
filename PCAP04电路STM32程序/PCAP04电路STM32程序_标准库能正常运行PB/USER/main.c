#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "pcap.h"
#include "w25q16.h"
#include "PCAP04.h"

//ALIENTEK Mini STM32开发板范例代码3
//串口实验   
unsigned int table[]={0xc04200ff,0xc1201022,0xc20f460b,0xc3090000,0xc4080001,0xc5000000,
	                  0xc6004340,0xc71f0000,0xc8800010,0xc9ff000f,0xca180047,0xd4000001};//寄存器配置

//unsigned long cap_status = 0,cap_reszult0 = 0,cap_reszult6 = 0,cap_reszult7 = 0 ,cap_temp = 0;
//float cap_val = 0.0f;
//unsigned char cap[6] = {'3','5','.','1','2',' '};
//unsigned int capsw,capgw,capzs,capxs1,capxs2;


unsigned long cap_status = 0,cap_reszult0 = 0,cap_reszult6 = 0,cap_reszult7 = 0 ,cap_temp = 0;
float cap_val = 0.0f;
unsigned char cap[6] = {'3','5','.','1','2',' '};
unsigned int capsw,capgw,capzs,capxs1,capxs2;			
unsigned long Inteiro = 0 ,parcela  = 0;
										
u32 Value[6];										
										
	volatile float Res0_content;
	float Capacitance_ratio;
	u32 Capacity=0;
	u32 Resistance=0;
	double *p;



double integrated_data(u32 data)
{
	u32 integer;
	u32 fractional;
	double integrated;
	double j;
	integer = (data >> 27) & 0x1F;
	fractional = data  & 0x7FFFFFF;
	j = (double)(integer*10.0);
	integrated =(double)(fractional/13421779.41088971);
	integrated = (integrated + j);
	return (double)integrated;
}







 int main(void)
 {	

	delay_init();	    	 //延时函数初始化	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
	uart_init(115200);	 //串口初始化为9600
	LED_Init();		  	 //初始化与LED连接的硬件接口 
	SPI_Configuration();
//	SPIx_I2Cx_GPIOs_Init(SPI2);
//	SPIx_I2Cx_Interface_Init(SPI2);
//  Pcap01Init();
	
//	 GPIO_SetBits(GPIOB,GPIO_Pin_9);

//	cap01_init();
//	 while(1)
//	 {
////	 printf("状态：\r\n"); 
//		 spi_test();
//	 }
	 		PCAP04_Init();
	 	while(1)
	{
// printf_integrated();
//					printf("数据：%f\r\n", integrated_data(PCAP04_Read_CDC_Result_data(1*4),p));	
////		printf("数据2：%f\r\n",&p);
//				printf("数据0：%f\r\n", integrated_data(PCAP04_Read_CDC_Result_data(0)));
//				printf("数据1：%f\r\n", integrated_data(PCAP04_Read_CDC_Result_data(1*4)));
//				printf("数据2：%X\r\n", PCAP04_Read_CDC_Result_data(0));
//				printf("数据2：%d\r\n", integrated_data(PCAP04_Read_CDC_Result_data(2*4)));
//				printf("数据3：%d\r\n", integrated_data(PCAP04_Read_CDC_Result_data(3*4)));
//				printf("数据4：%d\r\n", integrated_data(PCAP04_Read_CDC_Result_data(4*4)));
//				printf("数据5：%d\r\n", integrated_data(PCAP04_Read_CDC_Result_data(5*4)));
//				printf("数据6：%X\r\n", integrated_data(PCAP04_Read_RDC_Result_data());
																							 
		
				
				Value[0] = PCAP04_Read_CDC_Result_data(0);
				Value[1] = PCAP04_Read_CDC_Result_data(1);		
				Value[2] = PCAP04_Read_CDC_Result_data(2);		
				Value[3] = PCAP04_Read_CDC_Result_data(3);
				Value[4] = PCAP04_Read_CDC_Result_data(4);
				Value[5] = PCAP04_Read_CDC_Result_data(5);		
//		
//		
				printf("十六进制数据0：%X\r\n", Value[0]);
				printf("十六进制数据1：%X\r\n", Value[1]);
				printf("十六进制数据2：%X\r\n", Value[2]);
				printf("十六进制数据3：%X\r\n", Value[3]);
				printf("十六进制数据4：%X\r\n", Value[4]);
				printf("十六进制数据5：%X\r\n", Value[5]);	


				printf("数据0：%f\r\n", integrated_data(Value[0]));	
				printf("数据1：%f\r\n", integrated_data(Value[1]));	
				printf("数据2：%f\r\n", integrated_data(Value[2]));			
				printf("数据3：%f\r\n", integrated_data(Value[3]));			
				printf("数据4：%f\r\n", integrated_data(Value[4]));	
				printf("数据5：%f\r\n", integrated_data(Value[5]));	
				
						
//				if(PCap04_Test() != 1)
//		{
//			printf("通讯失败\n");
//		}
				
				
//				printf("状态2：%x\r\n", PCAP04_Read_CDC_Result_data(1)); 
//				printf("状态3：%x\r\n", PCAP04_Read_CDC_Result_data(2)); 
//				printf("状态4：%x\r\n", PCAP04_Read_CDC_Result_data(3)); 
//				printf("状态5：%x\r\n", PCAP04_Read_CDC_Result_data(4)); 
//	 PCAP04_Readjcq();
		
		delay_ms(300);
//        SPI_Enable();
//	    __3nop();
//        write_date_8(0x48);   //addr8
//        __3nop();																	
//        cap_status = spi_read();//返回结果应该为0x100000或0x900000(在测量时通讯,准确性 	
//    SPI_Disable();
//		delay_ms(300);
//						printf("状态：%x\r\n",cap_status); 
//	__3nop();
////	write_date_8(0x8c);         //开始电容测量。		
//		
//		
//		  SPI_Enable();
//	    __3nop();
//        write_date_8(0x40);   //addr0 Res0 C0 LSB 0-7.9999  0b01xxxxxx
//        __3nop();
//        cap_reszult0 = spi_read();

//				SPI_Disable();
//				printf("Cap0：%x\r\n",cap_reszult0); 
//		
//				Inteiro = (cap_reszult0 >> 21) & 0x7;
//				parcela = cap_reszult0  & 0x1fffff;
//		

//				printf("Cap0： %d.%d\r\n",Inteiro,parcela); 
//			 delay_ms(100);

//    SPI_Disable();
//	__3nop();
//	write_date_8(0x8c);         //开始电容测量。
//		
//		  SPI_Disable();
//	    __3nop();
//        write_date_8(0x41);   //addr0 Res0 C0 LSB 0-7.9999  0b01xxxxxx
//        __3nop();
//        cap_reszult0 = spi_read();
////				printf("Cap0：%x\r\n",spi_read()); 
//		
//				Inteiro = (cap_reszult0 >> 21) & 0x7;
//				parcela = cap_reszult0  & 0x1fffff;
//		
//				printf("Cap1： %d.%d\r\n",Inteiro,parcela); 
//			 delay_ms(100);
//		
//    SPI_Disable();
//	__3nop();
//	write_date_8(0x8c);         //开始电容测量。		
//		
//		  SPI_Disable();
//	    __3nop();
//        write_date_8(0x42);   //
//        __3nop();
//        cap_reszult0 = spi_read();
////				printf("Cap0：%x\r\n",spi_read()); 
//		
//				Inteiro = (cap_reszult0 >> 21) & 0x7;
//				parcela = cap_reszult0  & 0x1fffff;
//		
//				printf("Cap2： %d.%d\r\n",Inteiro,parcela); 
//			 delay_ms(100);	
//			 
//    SPI_Disable();
//	__3nop();
//	write_date_8(0x8c);         //开始电容测量。			 

//		  SPI_Disable();
//	    __3nop();
//        write_date_8(0x43);   //
//        __3nop();
//        cap_reszult0 = spi_read();
////				printf("Cap0：%x\r\n",spi_read()); 
//		
//				Inteiro = (cap_reszult0 >> 21) & 0x7;
//				parcela = cap_reszult0  & 0x1fffff;
//		
//				printf("Cap3： %d.%d\r\n",Inteiro,parcela); 
//			 delay_ms(100);		


//    SPI_Disable();
//	__3nop();
//	write_date_8(0x8c);         //开始电容测量。


//		  SPI_Disable();
//	    __3nop();
//        write_date_8(0x44);   //
//        __3nop();
//        cap_reszult0 = spi_read();
////				printf("Cap0：%x\r\n",spi_read()); 
//		
//				Inteiro = (cap_reszult0 >> 21) & 0x7;
//				parcela = cap_reszult0  & 0x1fffff;
//		
//				printf("Cap4： %d.%d\r\n",Inteiro,parcela); 
//			 delay_ms(100);		

//    SPI_Disable();
//	__3nop();
//	write_date_8(0x8c);         //开始电容测量。

//			 
//		  SPI_Disable();
//	    __3nop();
//        write_date_8(0x45);   //
//        __3nop();
//        cap_reszult0 = spi_read();
////				printf("Cap0：%x\r\n",spi_read()); 
//		
//				Inteiro = (cap_reszult0 >> 21) & 0x7;
//				parcela = cap_reszult0  & 0x1fffff;
//		
//				printf("Cap5： %d.%d\r\n",Inteiro,parcela); 
//			 delay_ms(100);					 

//    SPI_Disable();
//	__3nop();
//	write_date_8(0x8c);         //开始电容测量。

//		  SPI_Disable();
//	    __3nop();
//        write_date_8(0x46);   //
//        __3nop();
//        cap_reszult0 = spi_read();
////				printf("Cap0：%x\r\n",spi_read()); 
//		
//				Inteiro = (cap_reszult0 >> 21) & 0x7;
//				parcela = cap_reszult0  & 0x1fffff;
//		
//				printf("Cap6： %d.%d\r\n",Inteiro,parcela); 
//			 delay_ms(100);					 
//			 
//    SPI_Disable();
//	__3nop();
//	write_date_8(0x8c);         //开始电容测量。
//			 
//		  SPI_Disable();
//	    __3nop();
//        write_date_8(0x47);   //
//        __3nop();
//        cap_reszult0 = spi_read();
////				printf("Cap0：%x\r\n",spi_read()); 
//		
//				Inteiro = (cap_reszult0 >> 21) & 0x7;
//				parcela = cap_reszult0  & 0x1fffff;
//		
//				printf("Cap7： %d.%d\r\n",Inteiro,parcela); 
//				printf("        \r\n"); 
//			 delay_ms(100);					 
			 
			
////		cap_reszult0 = 0x1b3011;
//    cap_temp = 0x1fffff;
//		cap_val = (double)cap_reszult0/(double)cap_temp;
//		cap_val = cap_val*47;

//		capzs = (unsigned int)cap_val;
//		capsw = capzs/10;
//		capgw = capzs%10;
//		
//		cap_val = cap_val - capzs;
//		cap_val = cap_val*100;
//		capzs = (unsigned int)cap_val;
//		capxs1 = capzs/10;
//		capxs2 = capzs%10;

//printf("Cap1： %d.%d%d\r\n",capzs,capxs1,capxs2); 
//printf("        \r\n");


//		cap[0] = capsw + '0';
//		cap[1] = capgw + '0';
//		cap[3] = capxs1 + '0';
//		cap[4] = capxs2 + '0';
		
		
//				  printf("Cap1： %d pF\r\n",cap_status); 
//				  printf("Cap1： %d pF\r\n",cap[0]); 
//					delay_ms(50);
//					printf("Cap2： %d pF\r\n",cap[1]); 
//					delay_ms(50);
//					printf("Cap3： %d pF\r\n",cap[2]); 
//					delay_ms(50);
//					printf("Cap4： %d pF\r\n",cap[3]); 
//					printf("        \r\n");  //51.9698
//					delay_ms(200);
		
	}	
	 
	 
	 
	 
	 
	 
	 
	 
	while(1)
	{
				
//			Res0_content = Read_Results_32Bit_Opcode(0x40, 0x01, 0); 
//			Capacitance_ratio = Read_Results_32Bit_Opcode(0x40, 0x01, 21); 
//		  printf("Capacity_value= %5.3f pF\r\n",Capacitance_ratio*200);  
//			printf("        \r\n");  //51.9698
////			LED0=!LED0;	delay_ms(200);
		

		
	}	
 
}


