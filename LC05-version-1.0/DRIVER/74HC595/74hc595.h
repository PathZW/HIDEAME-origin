#ifndef __74HC595_H__
#define __74HC595_H__
#include "sys.h"

//74HC595
#define		HC595_SER   		PEout(9)	//pin	SER		data input
#define		HC595_RCLK  		PEout(8)	//pin	RCLk	store (latch) clock
#define		HC595_SRCLK			PEout(7)	//pin	SRCLK	Shift data clock

#define		HC595_SER1   		PEout(9)	//pin	SER		data input
#define		HC595_RCLK1  		PEout(7)	//pin	RCLk	store (latch) clock  锁存
#define		HC595_SRCLK1		PEout(8)	//pin	SRCLK	Shift data clock  移位

#define		HC595_SER2   		PEout(12)	//pin	SER		data input
#define		HC595_RCLK2  		PEout(10)	//pin	RCLk	store (latch) clock  锁存
#define		HC595_SRCLK2		PEout(11)	//pin	SRCLK	Shift data clock  移位

//CD4051用于电流通道
#define   CD4051_A0				PEout(15)
#define   CD4051_B0				PEout(14)
#define   CD4051_C0				PEout(13)
//CD4051用于电压通道
#define   CD4051_A1				PDout(10)
#define   CD4051_B1				PDout(9)
#define   CD4051_C1				PDout(8)


void HC595_GPIO_Init(void);
void HC595_SendCMD(unsigned char channel1,unsigned char channel2);

void CD4051_GPIO_Init(void);
void CD4051_ChannelSet1(unsigned char channel);
void CD4051_ChannelSet2(unsigned char channel);
void HC595_SendCMD1(unsigned char channel);
void HC595_SendCMD2(unsigned char channel);



#endif
