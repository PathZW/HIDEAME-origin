#ifndef __SYS_FUNCTION_H__
#define __SYS_FUNCTION_H__

#include "sys.h"

typedef struct
{	
	unsigned short bl_cur_en;            //����Դblʹ��   
	unsigned short set_current; 		 //�趨�ߵ���			
	unsigned short set_ovp;  			 //�趨�ߵ�����ѹֵ  ��λ100mV			
	unsigned short set_uvp;  			 //�趨�ߵ���Ƿѹֵ  ��λ100mV  			
	unsigned short set_vf;  			 //�趨�ߵ���ѹ��ֵ  ��λ100mV 			
//	unsigned short set_vf_low;  		 //�趨�͵���ѹ��ֵ  ��λ100mV	
//	unsigned short set_current_low; 	 //�趨�͵���						
//	unsigned short set_ovp_low;  		 //�趨�ߵ�����ѹֵ  ��λ100mV			
//	unsigned short set_uvp_low;  		 //�趨�ߵ���Ƿѹֵ  ��λ100mV  			
 
	unsigned short set_opp;  			 //�趨������ֵ  ��λ100mW	
//	unsigned char  light_pattern;        //����ģʽ
	unsigned char  light_timer;          //����ʱ��(��ʱʱ��)
	unsigned char  set_channel;  		 //�趨ͨ���� 	
	
//	unsigned short set_thermistor_max;   //�趨����������ֵ���ֵ
//	unsigned short set_thermistor_min;   //�趨����������ֵ��Сֵ
	
	unsigned char  hot_sw_en;            //�Ȳ��ʹ��
}set_parameter_T;  //1K��������

typedef struct{
//	unsigned char  start_stop;			 //���� OR ֹͣ
//	unsigned short fault_code;  		 //���ϴ���						
	unsigned short fault_value;  		 //����ֵ   //���Բ�Ҫ
}run_parameter_T;

typedef union{
	unsigned char bit_all;
	struct{
		unsigned char	channel		:1;
		unsigned char	vf			:1;
		unsigned char	pin			:1;
		unsigned char	negat_gnd	:1;
		unsigned char	empty_load	:1;
		unsigned char	ovp			:1;
		unsigned char	uvp			:1;
		unsigned char	opp			:1;	
	}detail;
}fault_code_flag1_T;

typedef union{
	unsigned char bit_all;
	struct{
		unsigned char	channel		:1;
		unsigned char	vf			:1;
		unsigned char	pin			:1;
		unsigned char	negat_gnd	:1;
		unsigned char	empty_load	:1;
		unsigned char	ovp			:1;
		unsigned char	uvp			:1;
		unsigned char	opp			:1;	
	}detail;
}fault_code_flag2_T;

typedef union{     //�˴��ṹ��˳���ܸı�
	unsigned short bit_all;
	struct{
		unsigned short high_cur			:1;   //�ߵ���ʹ��
		unsigned short low_cur			:1;   //�͵���ʹ��
		unsigned short high_cur_check	:1;   //�ߵ����Ƿ���
		unsigned short low_cur_check	:1;   //�͵����Ƿ���
		unsigned short leakage_check	:1;   //©���Ƿ���
		unsigned short pin_short_check	:1;   //Pin���·�Ƿ���
		unsigned short channel_check	:1;   //ͨ�������Ƿ���
		unsigned short vf_check			:1;   //ѹ���Ƿ���
		unsigned short negat_gnd_check	:1;   //���Ե��Ƿ���
		unsigned short empty_load_check	:1;   //�����Ƿ���
		unsigned short ovp_check		:1;   //��ѹ�Ƿ���
		unsigned short uvp_check		:1;   //Ƿѹ�Ƿ���
		unsigned short opp_check		:1;	  //�������Ƿ���
	}detail;
}protect_enable_T;  //1K����������


typedef union{
	unsigned char bit_all;
	struct{
		unsigned char ch0	:1;
		unsigned char ch1	:1;
		unsigned char ch2	:1;
		unsigned char ch3	:1;
		unsigned char ch4	:1;
		unsigned char ch5	:1;
		unsigned char ch6	:1;
		unsigned char ch7	:1;	
	}detail;
}channel_chose_T1;  //ͨ��Aѡ���־λ

typedef union{
	unsigned char bit_all;
	struct{
		unsigned char ch0	:1;
		unsigned char ch1	:1;
		unsigned char ch2	:1;
		unsigned char ch3	:1;
		unsigned char ch4	:1;
		unsigned char ch5	:1;
		unsigned char ch6	:1;
		unsigned char ch7	:1;	
	}detail;
}channel_chose_T2;  //ͨ��Bѡ���־λ


#define	 SOFTWARE_VERSION  	      0x0001  //����汾��
#define  HARDWAARE_VERSION        0x0002  //Ӳ���汾��

#define  CHANNEL_DECIDE_VALUE     1500    //ͨ���������ж�ֵ
#define  P_N_SHORT_VALUE          225     //���Ը������ж�ֵ
#define  N_GND_SHORT_VALUE        225     //���Եر����ж�ֵ
#define  P_GND_SHORT_VALUE        225     //���Եر����ж�ֵ
#define  PIN_SHORT_VALUE          500     //Pin���·�����ж�ֵ

//ͨ��A ON/OFF
#define POWER_ON1 		  	GPIO_SetBits(GPIOD,GPIO_Pin_12);
#define POWER_OFF1 		  	GPIO_ResetBits(GPIOD,GPIO_Pin_12);
//ͨ��B ON/OFF
#define POWER_ON2 		  	GPIO_SetBits(GPIOD,GPIO_Pin_11);
#define POWER_OFF2 		  	GPIO_ResetBits(GPIOD,GPIO_Pin_11);
//ͨ��A���Ե� ON/OFF
#define LED_TEST_ON1  	  	GPIO_SetBits(GPIOD,GPIO_Pin_13);
#define LED_TEST_OFF1 	  	GPIO_ResetBits(GPIOD,GPIO_Pin_13);
//ͨ��B���Ե� ON/OFF
#define LED_TEST_ON2  	  	GPIO_SetBits(GPIOB,GPIO_Pin_14);
#define LED_TEST_OFF2 	  	GPIO_ResetBits(GPIOB,GPIO_Pin_14);

//��������OK LED
#define LED7_ON 		  	GPIO_ResetBits(GPIOC,GPIO_Pin_7);
#define LED7_OFF 		  	GPIO_SetBits(GPIOC,GPIO_Pin_7);
//POWER  LED
#define LED12_ON 		  	GPIO_ResetBits(GPIOD,GPIO_Pin_15);
#define LED12_OFF 		  	GPIO_SetBits(GPIOD,GPIO_Pin_15);
//Signal LED
#define LED13_ON 		  	GPIO_ResetBits(GPIOC,GPIO_Pin_6);
#define LED13_OFF 		  	GPIO_SetBits(GPIOC,GPIO_Pin_6); 
//ERR1   LED
#define LED14_ON 		  	GPIO_ResetBits(GPIOC,GPIO_Pin_8);
#define LED14_OFF 		 	GPIO_SetBits(GPIOC,GPIO_Pin_8);
//ERR2   LED 
#define LED15_ON 		  	GPIO_ResetBits(GPIOC,GPIO_Pin_9);
#define LED15_OFF 		  	GPIO_SetBits(GPIOC,GPIO_Pin_9);


unsigned int Avr_Get(unsigned int *pADDR, unsigned char num);

void Protocols_ON_OFF_Driver(void);
void Protocols_Read_VolCur(void);
void Protocols_Change_BacklightCur(void);
void Protocols_Analysis_1K_Parameter(void);
void Protocols_ReadSoftwear_Version(void);
void Protocols_ReadHardware_Version(void);

void Protect_Channel(void); 			//ͨ������
//void Protect_Thermistor(void);          //��������
void Protect_VF(void); 					//ѹ����
void Protect_PIN_Short(void); 			//Pin���·���
void Protect_NegativeGND_Short(void);   //�����Եؼ��
void Protect_PositiveGND_Short(void);   //�����Եؼ��
void Protect_PositiveNegative_Short(void); //�����Ը������
void Protect_EmptyLoad(void);           //���ؼ��
void Protect_OVP(void);                 //��ѹ���
void Protect_UVP(void);                 //Ƿѹ���
void Protect_OverPower(void);           //�����ʼ��

void LED_ChannelDetection(void);        //ͨ�����
void Thermistor_Detection(void);        //����������
void LED_VoltageCurrentDetection(void); //��ѹ�������


void Power_ON1(void);
void Power_ON2(void);
void Power_OFF1(void);
void Power_OFF2(void);
void Start_Output(void);
void Stop_Output(void);
void Hot_Plug(void);

void StartValue_Init(void);	
void GPIO_Init_Config(void);	
void BSP_Init(void);

#endif
