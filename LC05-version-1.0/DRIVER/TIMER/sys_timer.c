#include "sys_timer.h"
#include "sys.h"
#include "sys_function.h"

unsigned char count_100ms = 0,count_200ms = 0; 	//��ʱ������
unsigned char count_300ms = 0,count_400ms = 0,count_500ms = 0; 	//��ʱ������

extern unsigned char light_timer;     			//������ʱ
extern unsigned char start_times_flag;		//������ʱ��־

extern unsigned char timer_protect_flag;    	//��ʱ������ʱ��ʶ
//extern unsigned char timer_read_vol_cur_flag; 	//����ѹ������ʱ��ʶ

/************************************************************************************************
* ������: TIM3_Int_Init
* ����	: TIM3��ʼ������
* ����	: arr���Զ���װֵ psc��ʱ��Ԥ��Ƶ��
* ����ֵ: None
************************************************************************************************/
void TIM3_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);  

	TIM_TimeBaseStructure.TIM_Period = arr;  
	TIM_TimeBaseStructure.TIM_Prescaler = psc;  
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;   
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);  

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure); 

	TIM_Cmd(TIM3, ENABLE); 					 
}

/************************************************************************************************
* ������: TIM3_IRQHandler
* ����	: TIM3�жϴ�����
* ����	: None
* ����ֵ: None
************************************************************************************************/
void TIM3_IRQHandler(void)  /*�ж�����Ϊ10ms*/
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)   
	{
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); 
		count_100ms ++;
		count_200ms ++;
		count_300ms ++;
		count_500ms ++;
		if(count_400ms > 49)
		{
			count_400ms = 0;
			if(GPIO_ReadOutputDataBit(GPIOC,GPIO_Pin_6) == 0)
			{
				LED13_OFF;				
			}
			else 
			{
				LED13_ON;
			}
		}
		
		if((start_times_flag == 1) && (count_100ms > 9))		 
		{
			count_100ms = 0;
			if(light_timer > 0)
			{
				light_timer --;
			}
			else
			{
				light_timer = 0;
				start_times_flag = 0;
			}
		}
	
		if(count_200ms > 49)
		{
			count_200ms = 0;
			timer_protect_flag = 1;
		}
		
		if(count_300ms > 9)
		{
			count_300ms = 0;
			//�Ȳ��
			Hot_Plug();
		}
	} 	 
}	

/************************************************************************************************
* ������: TIM3_ENABLE
* ����	: TIM3ʹ��
* ����	: None
* ����ֵ: None
************************************************************************************************/
void TIM3_ENABLE(void)
{	
	TIM_Cmd(TIM3, ENABLE);
}

/************************************************************************************************
* ������: TIM3_DISABLE
* ����	: TIM3��ʹ��
* ����	: None
* ����ֵ: None
************************************************************************************************/
void TIM3_DISABLE(void)
{	
	TIM_Cmd(TIM3, DISABLE);
}
