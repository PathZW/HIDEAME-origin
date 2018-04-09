#ifndef __SYS_FUNCTION_H__
#define __SYS_FUNCTION_H__

#include "sys.h"

typedef struct
{	
	unsigned short bl_cur_en;            //电流源bl使能   
	unsigned short set_current; 		 //设定高电流			
	unsigned short set_ovp;  			 //设定高电流过压值  单位100mV			
	unsigned short set_uvp;  			 //设定高电流欠压值  单位100mV  			
	unsigned short set_vf;  			 //设定高电流压差值  单位100mV 			
//	unsigned short set_vf_low;  		 //设定低电流压差值  单位100mV	
//	unsigned short set_current_low; 	 //设定低电流						
//	unsigned short set_ovp_low;  		 //设定高电流过压值  单位100mV			
//	unsigned short set_uvp_low;  		 //设定高电流欠压值  单位100mV  			
 
	unsigned short set_opp;  			 //设定过功率值  单位100mW	
//	unsigned char  light_pattern;        //点亮模式
	unsigned char  light_timer;          //点亮时间(延时时间)
	unsigned char  set_channel;  		 //设定通道数 	
	
//	unsigned short set_thermistor_max;   //设定热敏电阻阻值最大值
//	unsigned short set_thermistor_min;   //设定热敏电阻阻值最小值
	
	unsigned char  hot_sw_en;            //热插拔使能
}set_parameter_T;  //1K参数接收

typedef struct{
//	unsigned char  start_stop;			 //运行 OR 停止
//	unsigned short fault_code;  		 //故障代码						
	unsigned short fault_value;  		 //故障值   //可以不要
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

typedef union{     //此处结构体顺序不能改变
	unsigned short bit_all;
	struct{
		unsigned short high_cur			:1;   //高电流使能
		unsigned short low_cur			:1;   //低电流使能
		unsigned short high_cur_check	:1;   //高电流是否检测
		unsigned short low_cur_check	:1;   //低电流是否检测
		unsigned short leakage_check	:1;   //漏电是否检测
		unsigned short pin_short_check	:1;   //Pin间短路是否检测
		unsigned short channel_check	:1;   //通道错误是否检测
		unsigned short vf_check			:1;   //压差是否检测
		unsigned short negat_gnd_check	:1;   //负对地是否检测
		unsigned short empty_load_check	:1;   //空载是否检测
		unsigned short ovp_check		:1;   //过压是否检测
		unsigned short uvp_check		:1;   //欠压是否检测
		unsigned short opp_check		:1;	  //过功率是否检测
	}detail;
}protect_enable_T;  //1K检测参数解析


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
}channel_chose_T1;  //通道A选择标志位

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
}channel_chose_T2;  //通道B选择标志位


#define	 SOFTWARE_VERSION  	      0x0001  //软体版本号
#define  HARDWAARE_VERSION        0x0002  //硬件版本号

#define  CHANNEL_DECIDE_VALUE     1500    //通道数保护判定值
#define  P_N_SHORT_VALUE          225     //正对负保护判定值
#define  N_GND_SHORT_VALUE        225     //负对地保护判定值
#define  P_GND_SHORT_VALUE        225     //正对地保护判定值
#define  PIN_SHORT_VALUE          500     //Pin间短路保护判定值

//通道A ON/OFF
#define POWER_ON1 		  	GPIO_SetBits(GPIOD,GPIO_Pin_12);
#define POWER_OFF1 		  	GPIO_ResetBits(GPIOD,GPIO_Pin_12);
//通道B ON/OFF
#define POWER_ON2 		  	GPIO_SetBits(GPIOD,GPIO_Pin_11);
#define POWER_OFF2 		  	GPIO_ResetBits(GPIOD,GPIO_Pin_11);
//通道A正对地 ON/OFF
#define LED_TEST_ON1  	  	GPIO_SetBits(GPIOD,GPIO_Pin_13);
#define LED_TEST_OFF1 	  	GPIO_ResetBits(GPIOD,GPIO_Pin_13);
//通道B正对地 ON/OFF
#define LED_TEST_ON2  	  	GPIO_SetBits(GPIOB,GPIO_Pin_14);
#define LED_TEST_OFF2 	  	GPIO_ResetBits(GPIOB,GPIO_Pin_14);

//程序下载OK LED
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

void Protect_Channel(void); 			//通道保护
//void Protect_Thermistor(void);          //热敏电阻
void Protect_VF(void); 					//压差检测
void Protect_PIN_Short(void); 			//Pin间短路检测
void Protect_NegativeGND_Short(void);   //负极对地检测
void Protect_PositiveGND_Short(void);   //正极对地检测
void Protect_PositiveNegative_Short(void); //正极对负极检测
void Protect_EmptyLoad(void);           //空载检测
void Protect_OVP(void);                 //过压检测
void Protect_UVP(void);                 //欠压检测
void Protect_OverPower(void);           //过功率检测

void LED_ChannelDetection(void);        //通道检测
void Thermistor_Detection(void);        //热敏电阻检测
void LED_VoltageCurrentDetection(void); //电压电流侦测


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
