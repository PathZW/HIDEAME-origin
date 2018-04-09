#include "adc.h"
#include "sys.h"
#include "delay.h"
#include "stdio.h"
#include "sys_function.h"

static unsigned short  AD_DMABuffer[10] = {0};
static float  accept_value[10] = {0};  //ADͨ��DMA����ֵ

/*******************************************************************************************************
* ������: ADC_Init_Config
* ����	: �ڲ�ADC��ʼ��
* ����	: None
* ����ֵ: None    
*******************************************************************************************************/
void ADC_Init_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure; 

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE );	  //ʹ��ADC1ͨ��ʱ��
    
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //����ADC��Ƶ����6 72M/6=12,ADC���ʱ�䲻�ܳ���14M

	//PA1 ��Ϊģ��ͨ����������                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ģ����������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //��λADC1,������ ADC1 ��ȫ���Ĵ�������Ϊȱʡֵ

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	//����ADC_InitStruct��ָ���Ĳ�����ʼ������ADCx�ļĴ���   

  ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1	
	ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼  	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����	
	ADC_StartCalibration(ADC1);	 //����ADУ׼ 
	while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����
}

unsigned short Get_Adc(unsigned char ch)   
{
  //����ָ��ADC�Ĺ�����ͨ����һ�����У�����ʱ��
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADCͨ��,����ʱ��Ϊ239.5����	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//ʹ��ָ����ADC1�����ת����������	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//�ȴ�ת������

	return ADC_GetConversionValue(ADC1);	//�������һ��ADC1�������ת�����
}

unsigned short Get_Adc_Average(unsigned char ch,unsigned char times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(5);
	}
	printf("----------------\n");
	return temp_val/times;
} 	 


/*******************************************************************************************************
* ������: ADC_DMA_InitConfig
* ����	: �ڲ�ADC��ʼ������
* ����	: None
* ����ֵ: None
*******************************************************************************************************/
void ADC_DMA_InitConfig(void)
{
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 , ENABLE );	 
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);  
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	DMA_DeInit(DMA1_Channel1);   //DMA1ͨ��1�Ĵ�����Ϊȱʡֵ
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR;  //DMA�������ַChannel1
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&AD_DMABuffer[0]; //DMA�ڴ����ַ 
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //���ݴ��䷽���ڴ���Ϊ���ݵĴ���Ŀ�ĵأ�������
	DMA_InitStructure.DMA_BufferSize = 10; 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //���������ģʽ
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //DMA�ڴ����ģʽ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //DMAÿ�η������ݵĳ���
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; 
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; //ѭ��ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;  //DMA���ȼ�����
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //DMA������Memory�б����໥����
	DMA_Init(DMA1_Channel1, &DMA_InitStructure); 

	//PA6~PA7
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //ʹ��PA�˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//PB0~PB1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//ʹ��PA�˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//PC0~PC5  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//ʹ��PA�˿�ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_4|GPIO_Pin_3|GPIO_Pin_2|GPIO_Pin_1 |GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz	
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	ADC_DeInit(ADC1); //ADC1ͨ��1�Ĵ�����Ϊȱʡֵ
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //����ģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE; //��ͨ��
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; //����ת��
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 10; //˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);   

	ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 1, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_7, 2, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_8, 3, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_9, 4, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 5, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 6, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 7, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 8, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 9, ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 10, ADC_SampleTime_55Cycles5);

	ADC_DMACmd(ADC1, ENABLE); 
	ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1
	ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼ 
	while(ADC_GetResetCalibrationStatus(ADC1)); //�ȴ���λУ׼����
	ADC_StartCalibration(ADC1); //����ADУ׼
	while(ADC_GetCalibrationStatus(ADC1)); //�ȴ�У׼����

	DMA_Cmd(DMA1_Channel1, ENABLE);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	 
	// ADC_TempSensorVrefintCmd(ENABLE);
}				  

/*******************************************************************************************************
* ������: AD_GetData
* ����	: ��ȡADֵ
* ����	: channel��ѡ���ȡ��һͨ��
* ����ֵ: None
*******************************************************************************************************/
unsigned short AD_GetData(unsigned char channel)
{	
	unsigned short temp_value = 0;

	accept_value[channel] = (float)AD_DMABuffer[channel] * (3300.00 / 4096);  //������100��
	switch(channel)
	{
		//LED+_AD1  ͨ��1������ѹ���
		case 0:	temp_value = accept_value[channel] * 31;  //mv
				break;
		//LED+_AD2  ͨ��2������ѹ���
		case 1:	temp_value = accept_value[channel] * 31;  //mv
				break;
		//CHL_V_AD1 ͨ��1������ѹ���
		case 2:	temp_value = accept_value[channel] * 2;   //mv
				break;
		//CHL_V_AD2 ͨ��2������ѹ���
		case 3:	temp_value = accept_value[channel] * 2;   //mv
				break;
		//AD_CURRENTA ͨ��1�����������
		case 4:	temp_value = accept_value[channel];       //ma
				break;
		//AD_CURRENTA ͨ��2�����������
		case 5:	temp_value = accept_value[channel];       //ma
				break;
		//AD_CURRENT1 ͨ��1�����������
		case 6:	temp_value = (accept_value[channel] * 1.25) / 49.9;  //ma
				break;
		//AD_CURRENT2 ͨ��2�����������
		case 7:	temp_value = (accept_value[channel] * 1.25) / 49.9;  //ma
				break;
		//RT_AD1     ͨ��1�����������				
		case 8:	temp_value = 43.00 / 1000 * accept_value[channel] / (5 - accept_value[channel]/1000); //0.1K
//				temp_value = accept_value[channel];
				break;
		//RT_AD2     ͨ��2�����������					
		case 9:	temp_value = 43.00 / 1000 * accept_value[channel] / (5 - accept_value[channel]/1000); //0.1K
				break;		
		default:break;
	}
//	printf("--AD_GetData--\n");
//	printf("AD_DMABuffer[%d]:%x\t%d\n",channel,AD_DMABuffer[channel],AD_DMABuffer[channel]);
//	printf("accept_value[%d]:%x\t%f\n",channel,(unsigned short)accept_value[channel],accept_value[channel]);
//	printf("channel:%d\t%x\t%d\n",channel,temp_value,temp_value);

	return temp_value;
}

/*******************************************************************************************************
* ������: AcquireAverage_AD
* ����	: ��ȡAD 5��ƽ��ֵ
* ����	: channel��ѡ���ȡ��һͨ��
* ����ֵ: None
*******************************************************************************************************/
unsigned short AcquireAverage_AD(unsigned char channel)
{	
	unsigned char i = 0;
	unsigned short temp_value = 0;
	float temp[5] = {0};
	for(i = 0;i < 5;i ++)
	{
		temp[i] = AD_GetData(channel);
	}
	temp_value = Avr_Get((unsigned int *)&(temp[0]),5);
//	printf("-----AcquireAverage_AD-------\n");
//	printf("temp_value:%x\t%d\n",temp_value,temp_value);
	return temp_value;
}



