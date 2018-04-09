#ifndef __DAC_H__
#define __DAC_H__

#define DAC_OUT1  1
#define DAC_OUT2  2

void DAC_Init_Config(void);
void DAC_Start_Init(void);
void DAC_WriteData(unsigned char channel,unsigned short value);
#endif
