#ifndef __SYS_UART_H__
#define __SYS_UART_H__

typedef union{
	unsigned char bit_all;
	struct{
		unsigned char receive_ok 			:1 ;
		unsigned char run_start   			:1 ;
		unsigned char run_stop   			:1 ;
		unsigned char recive_value_err		:1 ;
	}detail;
}run_flag_T;

unsigned int crc(unsigned char ddata[],unsigned int l);
void USART1_Init_Config(unsigned int bound);
void USART2_Init_Config(unsigned int bound);
void uart1_send_byte(unsigned char byte);

#endif 

