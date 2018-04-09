#ifndef __ADC_H__
#define __ADC_H__



void ADC_Init_Config(void);
unsigned short Get_Adc(unsigned char ch);
unsigned short Get_Adc_Average(unsigned char ch,unsigned char times);

void ADC_DMA_InitConfig(void);
unsigned short AD_GetData(unsigned char channel);
unsigned short AcquireAverage_AD(unsigned char channel);

#endif
