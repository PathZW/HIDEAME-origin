#include "sys_timer.h"
#include "sys.h"
#include "sys_function.h"

unsigned char count_100ms = 0,count_200ms = 0; 	//定时器计数
unsigned char count_300ms = 0,count_400ms = 0,count_500ms = 0; 	//定时器计数

extern unsigned char light_timer;     			//启动延时
extern unsigned char start_times_flag;		//启动延时标志

extern unsigned char timer_protect_flag;    	//定时保护计时标识
//extern unsigned char timer_read_vol_cur_flag; 	//读电压电流计时标识

/************************************************************************************************
* 函数名: TIM3_Int_Init
* 功能	: TIM3初始化配置
* 参数	: arr：自动重装值 psc：时钟预分频数
* 返回值: None
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
* 函数名: TIM3_IRQHandler
* 功能	: TIM3中断处理函数
* 参数	: None
* 返回值: None
************************************************************************************************/
void TIM3_IRQHandler(void)  /*中断周期为10ms*/
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
			//热插拔
			Hot_Plug();
		}
	} 	 
}	

/************************************************************************************************
* 函数名: TIM3_ENABLE
* 功能	: TIM3使能
* 参数	: None
* 返回值: None
************************************************************************************************/
void TIM3_ENABLE(void)
{	
	TIM_Cmd(TIM3, ENABLE);
}

/************************************************************************************************
* 函数名: TIM3_DISABLE
* 功能	: TIM3关使能
* 参数	: None
* 返回值: None
************************************************************************************************/
void TIM3_DISABLE(void)
{	
	TIM_Cmd(TIM3, DISABLE);
}
