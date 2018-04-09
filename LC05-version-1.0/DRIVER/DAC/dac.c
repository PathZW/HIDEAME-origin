#include "dac.h"
#include "sys.h"
#include "delay.h"
/*******************************************************************************************************
* ������: DAC_Init_Config
* ����	: �ڲ�DAC��ʼ��
* ����	: None
* ����ֵ: None    
*******************************************************************************************************/
void DAC_Init_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	DAC_InitTypeDef DAC_InitStructure;
	
	//PA.4~5
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA, GPIO_Pin_4 | GPIO_Pin_5);
	
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	DAC_Init(DAC_Channel_2, &DAC_InitStructure);
	
	DAC_Cmd(DAC_Channel_1, ENABLE);
	DAC_Cmd(DAC_Channel_2, ENABLE);	
	
	/* DAC BUFF CLEAR */
	DAC_SetChannel1Data(DAC_Align_12b_R, 0); 
	DAC_SetChannel2Data(DAC_Align_12b_R, 0); 
}

/*******************************************************************************************************
* ������: DAC_WriteData
* ����	: ��DACдֵ
* ����	: channel��DACͨ��   value��Ҫд��ֵ
* ����ֵ: None  
* Notice��//voltage:0~3300,����: 0~3.3V
*******************************************************************************************************/
void DAC_WriteData(unsigned char channel,unsigned short value)
{
	float temp = value;
	temp /= 1000;
	temp = temp * 4096 / 3.3;
	//ϵ��У��
//	temp = temp * 1.5;   
	switch(channel)
	{
		case DAC_OUT1: 	
//						temp = temp * 1.5; //ϵ��У��
						DAC_SetChannel1Data(DAC_Align_12b_R,temp);
						break;		
		case DAC_OUT2:  
//						temp = temp * 1.5; //ϵ��У��
						DAC_SetChannel2Data(DAC_Align_12b_R,temp);
						break;		
		default:
						break;
	}
}

/*******************************************************************************************************
* ������: DAC_Start_Init
* ����	: ����DAC��ʼֵ
* ����	: None
* ����ֵ: None    
*******************************************************************************************************/
void DAC_Start_Init(void)
{
	DAC_WriteData(DAC_OUT1,0);
	DAC_WriteData(DAC_OUT2,0);
}

