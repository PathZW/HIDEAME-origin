#ifndef __SYS_TIMER_H__
#define __SYS_TIMER_H__


void TIM3_Int_Init(unsigned short arr,unsigned short psc);
void TIM3_IRQHandler(void); 
void TIM3_ENABLE(void);
void TIM3_DISABLE(void);


#endif

