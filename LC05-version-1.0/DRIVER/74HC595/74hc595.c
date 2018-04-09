#include "74hc595.h"
#include "sys.h"
#include "delay.h"

/************************************************************************************************
* ������: HC595_GPIO_Init
* ����	: HC595�˿ڳ�ʼ��
* ����	: None
* ����ֵ: None
************************************************************************************************/
void HC595_GPIO_Init(void)
{
	 GPIO_InitTypeDef  GPIO_InitStructure;
	
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);//ʹ��PB�˿�ʱ��
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 |  GPIO_Pin_10| GPIO_Pin_11 | GPIO_Pin_12;				 
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	 GPIO_Init(GPIOE, &GPIO_InitStructure);
}

/************************************************************************************************
* ������: HC595_SendCMD
* ����	: HC595���ͨ��ʹ��,��Ƭ595����
* ����	: channel1��ʹ��ͨ��1  channel2��ʹ��ͨ��1
* ����ֵ: None
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
* ������: HC595_SendCMD1
* ����	: HC595���ͨ��ʹ��
* ����	: channel��ʹ��ͨ��
* ����ֵ: None
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
	HC595_RCLK1 = 1; //��������	
}

/************************************************************************************************
* ������: HC595_SendCMD2
* ����	: HC595���ͨ��ʹ��
* ����	: channel��ʹ��ͨ��
* ����ֵ: None
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
	HC595_RCLK2 = 1; //��������	
}

/************************************************************************************************
* ������: CD4051_GPIO_Init
* ����	: CD4051�˿ڳ�ʼ��
* ����	: None
* ����ֵ: None
************************************************************************************************/
void CD4051_GPIO_Init(void)
{
	 GPIO_InitTypeDef  GPIO_InitStructure;
	 //PD.8 ~ 10
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);  //ʹ��PB�˿�ʱ��
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;				 
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		//�������
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//IO���ٶ�Ϊ50MHz
	 GPIO_Init(GPIOD, &GPIO_InitStructure);
	 //PE.13 ~ 15
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);  //ʹ��PB�˿�ʱ��
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;				 
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		//�������
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		//IO���ٶ�Ϊ50MHz
	 GPIO_Init(GPIOE, &GPIO_InitStructure);
}

/************************************************************************************************
* ������: CD4051_VolageChannelSet
* ����	: CD4051���ã����ڵ�ѹ��⣩
* ����	: channel��ͨ��ʹ��
* ����ֵ: None
************************************************************************************************/
void CD4051_ChannelSet1(unsigned char channel)
{
	switch(channel)
	{
		//X0���
		case	0:	CD4051_C0 = 1;
					CD4051_B0 = 1;
					CD4051_A0 = 1;
					break;
		//X1���
		case	1:	CD4051_C0 = 1;
					CD4051_B0 = 1;
					CD4051_A0 = 0;
					break;
		//X2���
		case	2:	CD4051_C0 = 1;
					CD4051_B0 = 0;
					CD4051_A0 = 1;
					break;
		//X3���
		case	3:	CD4051_C0 = 1;
					CD4051_B0 = 0;
					CD4051_A0 = 0;
					break;
		//X4���
		case	4:	CD4051_C0 = 0;
					CD4051_B0 = 1;
					CD4051_A0 = 1;
					break;
		//X5���
		case	5:	CD4051_C0 = 0;
					CD4051_B0 = 1;
					CD4051_A0 = 0;
					break;
		//X6���
		case	6:	CD4051_C0 = 0;
					CD4051_B0 = 0;
					CD4051_A0 = 1;
					break;	
		//X7���
		case	7:	CD4051_C0 = 0;
					CD4051_B0 = 0;
					CD4051_A0 = 0;
					break;
							
		default:	break;
	}	
}

/************************************************************************************************
* ������: CD4051_ChannelSet2
* ����	: CD4051����
* ����	: channel��ͨ��ʹ��
* ����ֵ: None
************************************************************************************************/
void CD4051_ChannelSet2(unsigned char channel)
{
	switch(channel)
	{
		//X0���
		case	0:	CD4051_C1 = 1;
					CD4051_B1 = 1;
					CD4051_A1 = 1;
					break;
		//X1���
		case	1:	CD4051_C1 = 1;
					CD4051_B1 = 1;
					CD4051_A1 = 0;
					break;
		//X2���
		case	2:	CD4051_C1 = 1;
					CD4051_B1 = 0;
					CD4051_A1 = 1;
					break;
		//X3���
		case	3:	CD4051_C1 = 1;
					CD4051_B1 = 0;
					CD4051_A1 = 0;
					break;
		//X4���
		case	4:	CD4051_C1 = 0;
					CD4051_B1 = 1;
					CD4051_A1 = 1;
					break;
		//X5���
		case	5:	CD4051_C1 = 0;
					CD4051_B1 = 1;
					CD4051_A1 = 0;
					break;
		//X6���
		case	6:	CD4051_C1 = 0;
					CD4051_B1 = 0;
					CD4051_A1 = 1;
					break;	
		//X7���
		case	7:	CD4051_C1 = 0;
					CD4051_B1 = 0;
					CD4051_A1 = 0;
					break;
							
		default:	break;
	}
}
	


