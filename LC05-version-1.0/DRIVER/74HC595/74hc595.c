#include "74hc595.h"
#include "sys.h"
#include "delay.h"

/************************************************************************************************
* 函数名: HC595_GPIO_Init
* 功能	: HC595端口初始化
* 参数	: None
* 返回值: None
************************************************************************************************/
void HC595_GPIO_Init(void)
{
	 GPIO_InitTypeDef  GPIO_InitStructure;
	
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);//使能PB端口时钟
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 |  GPIO_Pin_10| GPIO_Pin_11 | GPIO_Pin_12;				 
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	 GPIO_Init(GPIOE, &GPIO_InitStructure);
}

/************************************************************************************************
* 函数名: HC595_SendCMD
* 功能	: HC595输出通道使能,两片595级联
* 参数	: channel1：使能通道1  channel2：使能通道1
* 返回值: None
************************************************************************************************/
/* void HC595_SendCMD(unsigned char channel1,unsigned char channel2)
{
	unsigned char i = 0;
	unsigned short temp_value;
	
	temp_value = (unsigned char)(channel2 << 8) + channel1;
	
	for(i = 0; i < 16; i++)
	{
		if(((temp_value << i) & 0x8000) != 0)
		{
			HC595_SER = 1;		
		}
		else
		{
			HC595_SER = 0;
		}
		HC595_RCLK = 0;
		delay_us(100);
		HC595_RCLK = 1;
	}	
	HC595_SRCLK = 0;
	delay_us(100);
	HC595_SRCLK = 1;
} */


/************************************************************************************************
* 函数名: HC595_SendCMD1
* 功能	: HC595输出通道使能
* 参数	: channel：使能通道
* 返回值: None
************************************************************************************************/
void HC595_SendCMD1(unsigned char channel)
{
	unsigned char i = 0;
	
	HC595_RCLK1 = 0;
	HC595_SRCLK1 = 0;
	HC595_SER1 = 0;
	for(i = 0; i<8; i++)
	{		
		if(((channel << i) & 0x80))
		{
			HC595_SER1 = 1;
		}
		else 
		{
			HC595_SER1 = 0;
		}
		HC595_SRCLK1 = 1;

		HC595_SRCLK1 = 0;	
	}		
	HC595_RCLK1 = 1; //更新数据	
}

/************************************************************************************************
* 函数名: HC595_SendCMD2
* 功能	: HC595输出通道使能
* 参数	: channel：使能通道
* 返回值: None
************************************************************************************************/
void HC595_SendCMD2(unsigned char channel)
{
	unsigned char i = 0;
	
	HC595_RCLK2 = 0;
	HC595_SRCLK2 = 0;
	HC595_SER2 = 0;
	for(i = 0; i<8; i++)
	{		
		if(((channel << i) & 0x80))
		{
			HC595_SER2 = 1;
		}
		else 
		{
			HC595_SER2 = 0;
		}
		HC595_SRCLK2 = 1;

		HC595_SRCLK2 = 0;	
	}		
	HC595_RCLK2 = 1; //更新数据	
}

/************************************************************************************************
* 函数名: CD4051_GPIO_Init
* 功能	: CD4051端口初始化
* 参数	: None
* 返回值: None
************************************************************************************************/
void CD4051_GPIO_Init(void)
{
	 GPIO_InitTypeDef  GPIO_InitStructure;
	 //PD.8 ~ 10
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);  //使能PB端口时钟
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;				 
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		//推挽输出
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//IO口速度为50MHz
	 GPIO_Init(GPIOD, &GPIO_InitStructure);
	 //PE.13 ~ 15
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);  //使能PB端口时钟
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;				 
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		//推挽输出
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//IO口速度为50MHz
	 GPIO_Init(GPIOE, &GPIO_InitStructure);
}

/************************************************************************************************
* 函数名: CD4051_VolageChannelSet
* 功能	: CD4051配置（用于电压侦测）
* 参数	: channel：通道使能
* 返回值: None
************************************************************************************************/
void CD4051_ChannelSet1(unsigned char channel)
{
	switch(channel)
	{
		//X0输出
		case	0:	CD4051_C0 = 1;
					CD4051_B0 = 1;
					CD4051_A0 = 1;
					break;
		//X1输出
		case	1:	CD4051_C0 = 1;
					CD4051_B0 = 1;
					CD4051_A0 = 0;
					break;
		//X2输出
		case	2:	CD4051_C0 = 1;
					CD4051_B0 = 0;
					CD4051_A0 = 1;
					break;
		//X3输出
		case	3:	CD4051_C0 = 1;
					CD4051_B0 = 0;
					CD4051_A0 = 0;
					break;
		//X4输出
		case	4:	CD4051_C0 = 0;
					CD4051_B0 = 1;
					CD4051_A0 = 1;
					break;
		//X5输出
		case	5:	CD4051_C0 = 0;
					CD4051_B0 = 1;
					CD4051_A0 = 0;
					break;
		//X6输出
		case	6:	CD4051_C0 = 0;
					CD4051_B0 = 0;
					CD4051_A0 = 1;
					break;	
		//X7输出
		case	7:	CD4051_C0 = 0;
					CD4051_B0 = 0;
					CD4051_A0 = 0;
					break;
							
		default:	break;
	}	
}

/************************************************************************************************
* 函数名: CD4051_ChannelSet2
* 功能	: CD4051配置
* 参数	: channel：通道使能
* 返回值: None
************************************************************************************************/
void CD4051_ChannelSet2(unsigned char channel)
{
	switch(channel)
	{
		//X0输出
		case	0:	CD4051_C1 = 1;
					CD4051_B1 = 1;
					CD4051_A1 = 1;
					break;
		//X1输出
		case	1:	CD4051_C1 = 1;
					CD4051_B1 = 1;
					CD4051_A1 = 0;
					break;
		//X2输出
		case	2:	CD4051_C1 = 1;
					CD4051_B1 = 0;
					CD4051_A1 = 1;
					break;
		//X3输出
		case	3:	CD4051_C1 = 1;
					CD4051_B1 = 0;
					CD4051_A1 = 0;
					break;
		//X4输出
		case	4:	CD4051_C1 = 0;
					CD4051_B1 = 1;
					CD4051_A1 = 1;
					break;
		//X5输出
		case	5:	CD4051_C1 = 0;
					CD4051_B1 = 1;
					CD4051_A1 = 0;
					break;
		//X6输出
		case	6:	CD4051_C1 = 0;
					CD4051_B1 = 0;
					CD4051_A1 = 1;
					break;	
		//X7输出
		case	7:	CD4051_C1 = 0;
					CD4051_B1 = 0;
					CD4051_A1 = 0;
					break;
							
		default:	break;
	}
}
	


