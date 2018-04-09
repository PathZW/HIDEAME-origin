#include "adc.h"
#include "sys.h"
#include "delay.h"
#include "stdio.h"
#include "sys_function.h"

static unsigned short  AD_DMABuffer[10] = {0};
static float  accept_value[10] = {0};  //AD通过DMA后处理值

/*******************************************************************************************************
* 函数名: ADC_Init_Config
* 功能	: 内部ADC初始化
* 参数	: None
* 返回值: None    
*******************************************************************************************************/
void ADC_Init_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef ADC_InitStructure; 

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_ADC1, ENABLE );	  //使能ADC1通道时钟
    
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	//PA1 作为模拟通道输入引脚                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //复位ADC1,将外设 ADC1 的全部寄存器重设为缺省值

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   

  ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1	
	ADC_ResetCalibration(ADC1);	//使能复位校准  	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束	
	ADC_StartCalibration(ADC1);	 //开启AD校准 
	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
}

unsigned short Get_Adc(unsigned char ch)   
{
  //设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
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
* 函数名: ADC_DMA_InitConfig
* 功能	: 内部ADC初始化配置
* 参数	: None
* 返回值: None
*******************************************************************************************************/
void ADC_DMA_InitConfig(void)
{
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 , ENABLE );	 
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);  
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	DMA_DeInit(DMA1_Channel1);   //DMA1通道1寄存器设为缺省值
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&ADC1->DR;  //DMA外设基地址Channel1
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&AD_DMABuffer[0]; //DMA内存基地址 
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC; //数据传输方向，内存作为数据的传输目的地，单向传输
	DMA_InitStructure.DMA_BufferSize = 10; 
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //非外设递增模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //DMA内存递增模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //DMA每次访问数据的长度
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; 
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular; //循环模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;  //DMA优先级设置
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable; //DMA中两个Memory中变量相互访问
	DMA_Init(DMA1_Channel1, &DMA_InitStructure); 

	//PA6~PA7
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //使能PA端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	//PB0~PB1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);//使能PA端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//PC0~PC5  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);//使能PA端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_4|GPIO_Pin_3|GPIO_Pin_2|GPIO_Pin_1 |GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz	
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	ADC_DeInit(ADC1); //ADC1通道1寄存器设为缺省值
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent; //独立模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE; //多通道
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE; //连续转换
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right; //ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 10; //顺序进行规则转换的ADC通道的数目
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
	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
	ADC_ResetCalibration(ADC1);	//使能复位校准 
	while(ADC_GetResetCalibrationStatus(ADC1)); //等待复位校准结束
	ADC_StartCalibration(ADC1); //开启AD校准
	while(ADC_GetCalibrationStatus(ADC1)); //等待校准结束

	DMA_Cmd(DMA1_Channel1, ENABLE);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);	 
	// ADC_TempSensorVrefintCmd(ENABLE);
}				  

/*******************************************************************************************************
* 函数名: AD_GetData
* 功能	: 获取AD值
* 参数	: channel：选择获取哪一通道
* 返回值: None
*******************************************************************************************************/
unsigned short AD_GetData(unsigned char channel)
{	
	unsigned short temp_value = 0;

	accept_value[channel] = (float)AD_DMABuffer[channel] * (3300.00 / 4096);  //扩大了100倍
	switch(channel)
	{
		//LED+_AD1  通道1正极电压侦测
		case 0:	temp_value = accept_value[channel] * 31;  //mv
				break;
		//LED+_AD2  通道2正极电压侦测
		case 1:	temp_value = accept_value[channel] * 31;  //mv
				break;
		//CHL_V_AD1 通道1负极电压侦测
		case 2:	temp_value = accept_value[channel] * 2;   //mv
				break;
		//CHL_V_AD2 通道2负极电压侦测
		case 3:	temp_value = accept_value[channel] * 2;   //mv
				break;
		//AD_CURRENTA 通道1正极电流侦测
		case 4:	temp_value = accept_value[channel];       //ma
				break;
		//AD_CURRENTA 通道2正极电流侦测
		case 5:	temp_value = accept_value[channel];       //ma
				break;
		//AD_CURRENT1 通道1负极电流侦测
		case 6:	temp_value = (accept_value[channel] * 1.25) / 49.9;  //ma
				break;
		//AD_CURRENT2 通道2负极电流侦测
		case 7:	temp_value = (accept_value[channel] * 1.25) / 49.9;  //ma
				break;
		//RT_AD1     通道1热敏电阻侦测				
		case 8:	temp_value = 43.00 / 1000 * accept_value[channel] / (5 - accept_value[channel]/1000); //0.1K
//				temp_value = accept_value[channel];
				break;
		//RT_AD2     通道2热敏电阻侦测					
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
* 函数名: AcquireAverage_AD
* 功能	: 获取AD 5次平均值
* 参数	: channel：选择获取哪一通道
* 返回值: None
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



