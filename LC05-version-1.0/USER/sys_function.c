#include "stm32f10x.h"
#include "sys_function.h"
#include "sys_timer.h"
#include "sys_uart.h"
#include "adc.h"
#include "dac.h"
#include "74hc595.h"
#include "delay.h"
#include "stdio.h"

unsigned int   send_value_char_count;
unsigned char  senddata_frontpoint[12];          //���ݰ�ͷ
unsigned char  recept_data_sure_crc[2];	         //��������CRCУ��ֵ
unsigned char  light_timer = 0;                  //������ʱ
unsigned int   led_positive_voltage1 = 0;     	 //ͨ��1������ѹ
unsigned int   led_positive_voltage2 = 0;     	 //ͨ��2������ѹ
unsigned int   led_positive_current1 = 0;     	 //ͨ��1��������
unsigned int   led_positive_current2 = 0;     	 //ͨ��2��������
unsigned int   led_negative_voltage1[8] = {0};   //ͨ��1������ѹ
unsigned int   led_negative_voltage2[8] = {0};   //ͨ��2������ѹ
unsigned int   led_negative_current1[8] = {0};   //ͨ��1��������
unsigned int   led_negative_current2[8] = {0};   //ͨ��2��������
unsigned int   thermistor_value1 = 0;            //ͨ��1����������ֵ
unsigned int   thermistor_value2 = 0;			 //ͨ��2����������ֵ
unsigned char  port_channel_temp1 = 0;			 //ͨ��1���ͨ����
unsigned char  port_channel_temp2 = 0; 			 //ͨ��2���ͨ����

unsigned char  single_vol_channel_flag = 0;      //ͨ��ѡ���־ 1:ͨ��1 / 2:ͨ��2 /3:ͨ��1��2
unsigned char  start_times_flag = 0;			 //������ʱ��־
unsigned char  led_protect_flag1 = 0;			 //led1������ʾ��־λ  1:δ����
unsigned char  led_protect_flag2 = 0;	         //led2������ʾ��־λ
unsigned short single_vol_onoff = 0;             //����ON/OFF


set_parameter_T 	set_parameter;   		//1K ��������
run_parameter_T 	run_parameter;   		//���б���
protect_enable_T 	protect_enable; 		//��������
channel_chose_T1 	channel_chose_flag1;   //Aͨ������ͨ����
channel_chose_T2 	channel_chose_flag2;   //Bͨ������ͨ����
fault_code_flag1_T	fault_code_flag1;      //Aͨ�������־
fault_code_flag2_T	fault_code_flag2;      //Bͨ�������־
extern run_flag_T run_flag;
extern unsigned char recept_data_target[1100];    //�������ݻ���
extern unsigned char recept_data[1100];

/************************************************************************************************
* ������: Avr_Get
* ����	: ȥ�����������ֵ����Сֵ��ƽ��
* ����	: pADDR: �׵�ַ  num: �����С
* ����ֵ: ƽ��ֵ
************************************************************************************************/
unsigned int Avr_Get(unsigned int *pADDR, unsigned char num)
{
    unsigned int max, min, avr;
    u32 mux = 0;
    unsigned char i = 0, num_temp = 0;
    max = 0;
    min = 65535;
    for(i = 0; i < num; i++)
    {
        if(pADDR[i] == 0)
        {
            num_temp++;
        }
    }
    for(i = 0; i < num ; i++)
    {
        if(pADDR[i] != 0)
        {
            if(pADDR[i] > max) max = pADDR[i];
            if(pADDR[i] < min) min = pADDR[i];
            mux += pADDR[i];
        }
    }

    avr = (mux - max - min) / (num - num_temp - 2);
    return avr;
}

/************************************************************************************************
* ������: GPIO_Init_Config
* ����	: GPIO��ʼ������
* ����	: None
* ����ֵ: None
************************************************************************************************/
void GPIO_Init_Config(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    //PC.6~9
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);    //ʹ��PC�˿�ʱ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    //PD.11~15
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);    //ʹ��PD�˿�ʱ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    //PE.2~3 �Ȳ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE); //ʹ��PE�˿�ʱ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 		  //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;     //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}

/************************************************************************************************
* ������: StartValue_Init
* ����	: ��ʼ����Ԥ��ʼ��
* ����	: None
* ����ֵ: None
************************************************************************************************/
void StartValue_Init(void)
{
    HC595_SendCMD1(0xFF);  //ȫ��
    HC595_SendCMD2(0xFF);  //ȫ��
    LED_TEST_ON1;
    LED_TEST_ON2;
    LED12_ON;
    LED13_OFF;
    LED14_OFF;
    LED15_OFF;
}

/************************************************************************************************
* ������: Protocols_ON_OFF_Driver
* ����	: ����ON/OFF����(0x20)
* ����	: None
* ����ֵ: None
************************************************************************************************/
void Protocols_ON_OFF_Driver(void)
{
    unsigned char i = 0;
    unsigned char on_off_temp;
    unsigned int send_data_crc;
    on_off_temp = recept_data_target[9] & 0x01;

    if(on_off_temp == 1) //on
    {
        run_flag.detail.run_start = 1;

        if (((recept_data_target[8] & 0xC0) == 0xC0) || ((recept_data_target[8] & 0x30) == 0x30)
                || ((recept_data_target[8] & 0x90) == 0x90) || ((recept_data_target[8] & 0x60) == 0x60))
        {
            single_vol_channel_flag = 3;
            led_protect_flag1 = 1;
            led_protect_flag2 = 1;
        }
        else if (((recept_data_target[8] & 0xc0) == 0x80) || ((recept_data_target[8] & 0x30) == 0x20))
        {
            single_vol_channel_flag = 1;
            led_protect_flag1 = 1;
        }
        else if (((recept_data_target[8] & 0xc0) == 0x40) || ((recept_data_target[8] & 0x30) == 0x10))
        {
            single_vol_channel_flag = 2;
            led_protect_flag2 = 1;
        }
        else
        {
            single_vol_channel_flag = 0;
        }
    }
    else
    {
        run_flag.detail.run_stop = 1;

        if (((recept_data_target[8] & 0xC0) == 0xC0) || ((recept_data_target[8] & 0x30) == 0x30)
                || ((recept_data_target[8] & 0x90) == 0x90) || ((recept_data_target[8] & 0x60) == 0x60))
        {
            single_vol_channel_flag = 3;
            led_protect_flag1 = 0;
            led_protect_flag2 = 0;
        }
        else if (((recept_data_target[8] & 0xc0) == 0x80) || ((recept_data_target[8] & 0x30) == 0x20))
        {
            single_vol_channel_flag = 1;
            led_protect_flag1 = 0;
        }
        else if (((recept_data_target[8] & 0xc0) == 0x40) || ((recept_data_target[8] & 0x30) == 0x10))
        {
            single_vol_channel_flag = 2;
            led_protect_flag2 = 1;
        }
        else
        {
            single_vol_channel_flag = 0;
        }
    }
    ///////////////*down is point*///////////////////
    senddata_frontpoint[0] = 0x42;
    senddata_frontpoint[1] = 0x01;
    senddata_frontpoint[2] = 0x00;
    senddata_frontpoint[3] = 0x86;
    senddata_frontpoint[4] = 0x00;
    senddata_frontpoint[5] = 0x0c;
    senddata_frontpoint[6] = 0x00;
    senddata_frontpoint[7] = 0x01;
    senddata_frontpoint[10] = 0x00;
    senddata_frontpoint[11] = 0x00;
    senddata_frontpoint[8] =  0x00;
    if((recept_data_target[2] & 0xf0) == 0x10)
    {
        senddata_frontpoint[9] = 0x02;
    }
    else if	((recept_data_target[2] & 0xf0) == 0x20)
    {
        senddata_frontpoint[9] = 0x04;
    }
    else if((recept_data_target[2] & 0xf0) == 0x00)
    {
        senddata_frontpoint[9] = 0x01;
    }
    ///////////////*up is point*///////////////////
    send_data_crc = crc(&recept_data_target[0], 5);
    recept_data_sure_crc[0] = (unsigned char)send_data_crc;
    recept_data_sure_crc[1] = (unsigned char)(send_data_crc >> 8);

    for(i = 0; i < 12; i++)
    {
        uart1_send_byte(senddata_frontpoint[i]);
    }
    for(i = 0; i < 6; i++)
    {
        uart1_send_byte(recept_data_target[i]);
    }
    for(i = 0; i < 2; i++)
    {
        uart1_send_byte(recept_data_sure_crc[i]);
    }
}


/************************************************************************************************
* ������: Protocols_Read_VolCur
* ����	: ����ѹ��������(0x51)
* ����	: None
* ����ֵ: None
************************************************************************************************/
void Protocols_Read_VolCur(void)
{
    unsigned char i = 0;
    unsigned char mode_flag = 0;
    unsigned int send_data_crc;
//    unsigned int voltage_temp1 = 0;
//    unsigned int voltage_temp2 = 0;
    mode_flag = recept_data[4] << 1;
    send_value_char_count = (unsigned int)(recept_data[5]) << 1;
    ///////////////*down is point*///////////////////
    senddata_frontpoint[0] = 0x42;
    senddata_frontpoint[1] = 0x01;
    senddata_frontpoint[2] = 0x00;
    senddata_frontpoint[3] = 0x86;
    senddata_frontpoint[4] = (unsigned char)(send_value_char_count + 10) >> 8;
    senddata_frontpoint[5] = (send_value_char_count + 10);
    senddata_frontpoint[6] = 0x00;
    senddata_frontpoint[7] = 0x01;
    senddata_frontpoint[8] = 0x00;
    senddata_frontpoint[10] = 0x00;
    senddata_frontpoint[11] = 0x00;
    if((recept_data_target[2] & 0xf0) == 0x10)
    {
        senddata_frontpoint[9] = 0x02;
    }
    else if	((recept_data_target[2] & 0xf0) == 0x20)
    {
        senddata_frontpoint[9] = 0x04;
    }
    else if((recept_data_target[2] & 0xf0) == 0x00)
    {
        senddata_frontpoint[9] = 0x01;
    }
    ///////////////*up is point*///////////////////
    recept_data_target[0] = 0x32;
    recept_data_target[1] = 0x03;
    recept_data_target[2] = mode_flag;
    recept_data_target[3] = (unsigned char)send_value_char_count;
    recept_data_target[4] = fault_code_flag1.bit_all;      //Aͨ�������־;
    recept_data_target[5] = fault_code_flag2.bit_all;	   //Bͨ�������־;
    for(i = 0; i < 8; i++)
    {
//        voltage_temp1 = led_negative_voltage1[i];  //mv
        recept_data_target[6 + i * 4] = (unsigned char)(led_negative_voltage1[i] >> 8);
        recept_data_target[7 + i * 4] = (unsigned char)led_negative_voltage1[i];

        recept_data_target[8 + i * 4] = (unsigned char)(led_negative_current1[i] >> 8);
        recept_data_target[9 + i * 4] = (unsigned char)led_negative_current1[i];
    }

    for(i = 0; i < 8; i++)
    {
//        voltage_temp2 = led_negative_voltage2[i];  //mv
        recept_data_target[38 + i * 4] = (unsigned char)(led_negative_voltage2[i] >> 8);
        recept_data_target[39 + i * 4] = (unsigned char)led_negative_voltage2[i];

        recept_data_target[40 + i * 4] = (unsigned char)(led_negative_current2[i] >> 8);
        recept_data_target[41 + i * 4] = (unsigned char)led_negative_current2[i];
    }

    send_data_crc = crc(&recept_data_target[0], send_value_char_count + 3);
    recept_data_sure_crc[0] = (unsigned char)send_data_crc;
    recept_data_sure_crc[1] = (unsigned char)(send_data_crc >> 8);

    for(i = 0; i < 12; i++)
    {
        uart1_send_byte(senddata_frontpoint[i]);
    }
    for(i = 0; i < send_value_char_count + 4; i++)
    {
        uart1_send_byte(recept_data_target[i]);
    }
    for(i = 0; i < 2; i++)
    {
        uart1_send_byte(recept_data_sure_crc[i]);
    }
}


/************************************************************************************************
* ������: Protocols_Change_BacklightCur
* ����	: �ı䱳���������(0x8E)
* ����	: None
* ����ֵ: None
************************************************************************************************/
void Protocols_Change_BacklightCur(void)
{
    unsigned char i = 0;
    unsigned int send_data_crc;

    set_parameter.set_current = (unsigned short)(recept_data_target[10] << 8)  + recept_data_target[11];
    if(set_parameter.set_current > 1100)
    {
        recept_data_target[4] = 0x00; //�������ݴ��󣬷���0
        recept_data_target[5] = 0x00; //�������ݴ��󣬷���0

        LED12_ON;
        LED13_OFF;
        LED14_ON;
        LED15_ON;  //ȫ��
        run_flag.detail.recive_value_err = 1;
    }
    else
    {
        recept_data_target[4] = 0x00;
        recept_data_target[5] = 0x01;
        LED12_ON;
        LED13_OFF;
        LED14_OFF;
        LED15_OFF;
        run_flag.detail.recive_value_err = 0;
    }
    ///////////////*down is point*///////////////////
    senddata_frontpoint[0] = 0x42;
    senddata_frontpoint[1] = 0x01;
    senddata_frontpoint[2] = 0x00;
    senddata_frontpoint[3] = 0x86;
    senddata_frontpoint[4] = 0x00;
    senddata_frontpoint[5] = 0x0c;
    senddata_frontpoint[6] = 0x00;
    senddata_frontpoint[7] = 0x01;
    senddata_frontpoint[8] = 0x00;
    senddata_frontpoint[10] = 0x00;
    senddata_frontpoint[11] = 0x00;
    if((recept_data_target[2] & 0xf0) == 0x10)
    {
        senddata_frontpoint[9] = 0x02;
    }
    else if	((recept_data_target[2] & 0xf0) == 0x20)
    {
        senddata_frontpoint[9] = 0x04;
    }
    else if((recept_data_target[2] & 0xf0) == 0x00)
    {
        senddata_frontpoint[9] = 0x01;
    }
    ///////////////*up is point*///////////////////
    recept_data_target[0] = 0x32;
    recept_data_target[1] = 0x10;
    recept_data_target[2] = 0x20;
    recept_data_target[3] = 0x8E;
    recept_data_target[4] = 0x00;
    recept_data_target[5] = 0x01;

    send_data_crc = crc(&recept_data_target[0], send_value_char_count + 3);
    recept_data_sure_crc[0] = (unsigned char)send_data_crc;
    recept_data_sure_crc[1] = (unsigned char)(send_data_crc >> 8);

    for(i = 0; i < 12; i++)
    {
        uart1_send_byte(senddata_frontpoint[i]);
    }
    for(i = 0; i < 5; i++)
    {
        uart1_send_byte(recept_data_target[i]);
    }
    for(i = 0; i < 2; i++)
    {
        uart1_send_byte(recept_data_sure_crc[i]);
    }
}
/************************************************************************************************
* ������: Protocols_Analysis_1K_Parameter
* ����	: 1K������������(0x01)
* ����	: None
* ����ֵ: None
************************************************************************************************/
void Protocols_Analysis_1K_Parameter(void)
{
    unsigned char i;
    unsigned int send_data_crc;

    if(run_flag.detail.run_start == 0) //��run��ʱ��׼����1K����
    {
        /* ����ģʽ */
//		set_parameter.bl_cur_en = recept_data_target[0x8A+8] & 0x02;  //����Դʹ��
        protect_enable.bit_all = (unsigned short)(recept_data_target[0xC0 + 8] << 8) + recept_data_target[0xC1 + 8];
        set_parameter.set_current = (unsigned short)(recept_data_target[0xC2 + 8] << 8) + recept_data_target[0xC3 + 8];
//		set_parameter.set_current_low = (unsigned short)(recept_data_target[0xC4+8]<<8) + recept_data_target[0xC5+8];
        set_parameter.set_ovp = (unsigned short)(recept_data_target[0xC6 + 8] << 8) + recept_data_target[0xC7 + 8];
        set_parameter.set_uvp = (unsigned short)(recept_data_target[0xC8 + 8] << 8) + recept_data_target[0xC9 + 8];
//		set_parameter.set_ovp_low = (unsigned short)(recept_data_target[0xCA+8]<<8) + recept_data_target[0xCB+8];
//		set_parameter.set_uvp_low = (unsigned short)(recept_data_target[0xCC+8]<<8) + recept_data_target[0xCD+8];
        set_parameter.set_vf = recept_data_target[0xCE + 8];
//		set_parameter.set_vf_low = recept_data_target[0xCF+8];
//		set_parameter.light_pattern = recept_data_target[0xD0+8];
        set_parameter.light_timer = recept_data_target[0xD1 + 8];
        set_parameter.set_opp = (unsigned short)(recept_data_target[0xD2 + 8] << 8) + recept_data_target[0xD3 + 8];
        set_parameter.set_channel = (unsigned short)(recept_data_target[0xD4 + 8] << 8) + recept_data_target[0xD5 + 8];

//		set_parameter.set_thermistor_max = 300;  //30K
//		set_parameter.set_thermistor_min = 20;   //2K

        set_parameter.hot_sw_en = recept_data_target[0x33 + 8] & 0x01;

        //�ж���λ�����������Ƿ�Ƿ�
        if((set_parameter.set_current > 1100) || (set_parameter.set_ovp > 900)
                || (set_parameter.set_uvp > 900) || (set_parameter.set_opp > 100)
                || (set_parameter.set_vf > 100) || (set_parameter.set_channel > 8))
//			|| (set_parameter.set_thermistor_max > 300) || set_parameter.set_thermistor_min < 20)
        {
            recept_data_target[4] = 0x00; //�������ݴ��󣬷���0
            recept_data_target[5] = 0x00; //�������ݴ��󣬷���0

            LED12_ON;
            LED13_OFF;
            LED14_ON;
            LED15_ON;  //ȫ��
            run_flag.detail.recive_value_err = 1;
        }
        else
        {
            recept_data_target[4] = 0x00;
            recept_data_target[5] = 0x01;
            LED12_ON;
            LED13_OFF;
            LED14_OFF;
            LED15_OFF;
            run_flag.detail.recive_value_err = 0;
        }
    }
    else
    {
        recept_data_target[4] = 0x00;//�������ݴ��󣬷���0
        recept_data_target[5] = 0x00;//�������ݴ��󣬷���0
    }
    ///////////////*down is point*///////////////////
    senddata_frontpoint[0] = 0x42;
    senddata_frontpoint[1] = 0x01;
    senddata_frontpoint[2] = 0x00;
    senddata_frontpoint[3] = 0x86;
    senddata_frontpoint[4] = 0x00;
    senddata_frontpoint[5] = 0x0c;
    senddata_frontpoint[6] = 0x00;
    senddata_frontpoint[7] = 0x01;
    senddata_frontpoint[8] = 0x00;
    senddata_frontpoint[10] = 0x00;
    senddata_frontpoint[11] = 0x00;
    if((recept_data_target[2] & 0xf0) == 0x10)
    {
        senddata_frontpoint[9] = 0x02;
    }
    if((recept_data_target[2] & 0xf0) == 0x20)
    {
        senddata_frontpoint[9] = 0x04;
    }
    if((recept_data_target[2] & 0xf0) == 0x00)
    {
        senddata_frontpoint[9] = 0x01;
    }
    ///////////////*up is point*///////////////////
    recept_data_target[0] = 0x32;
    recept_data_target[1] = 0x10;
    recept_data_target[2] = 0x00;
    recept_data_target[3] = 0x01;
    send_data_crc = crc(&recept_data_target[0], 5);
    recept_data_sure_crc[0] = (unsigned char)send_data_crc;
    recept_data_sure_crc[1] = (unsigned char)(send_data_crc >> 8);

    for(i = 0; i < 12; i++)
    {
        uart1_send_byte(senddata_frontpoint[i]);
    }
    for(i = 0; i < 6; i++)
    {
        uart1_send_byte(recept_data_target[i]);
    }
    for(i = 0; i < 2; i++)
    {
        uart1_send_byte(recept_data_sure_crc[i]);
    }
}

/************************************************************************************************
* ������: Protocols_ReadSoftwear_Version
* ����	: �������汾������(0x1E)
* ����	: None
* ����ֵ: None
************************************************************************************************/
void Protocols_ReadSoftwear_Version(void)
{
    unsigned char i = 0;
    unsigned int  send_data_crc;

    send_value_char_count = ((unsigned int)(recept_data[4] << 8) + recept_data[5]) << 1;

    senddata_frontpoint[0] = 0x42;
    senddata_frontpoint[1] = 0x01;
    senddata_frontpoint[2] = 0x00;
    senddata_frontpoint[3] = 0x86;
    senddata_frontpoint[4] = (unsigned char)(send_value_char_count + 10) >> 8;
    senddata_frontpoint[5] = (send_value_char_count + 10);
    senddata_frontpoint[6] = 0x00;
    senddata_frontpoint[7] = 0x01;
    senddata_frontpoint[10] = 0x00;
    senddata_frontpoint[11] = 0x00;
    senddata_frontpoint[8] = 0x00;
    if((recept_data_target[2] & 0xf0) == 0x10)
    {
        senddata_frontpoint[9] = 0x02;
    }
    if((recept_data_target[2] & 0xf0) == 0x20)
    {
        senddata_frontpoint[9] = 0x04;
    }
    if((recept_data_target[2] & 0xf0) == 0x00)
    {
        senddata_frontpoint[9] = 0x01;
    }
    ///////////////*up is point*///////////////////
    recept_data_target[0] = recept_data[0];
    recept_data_target[1] = recept_data[1];
    recept_data_target[2] = recept_data[4] << 1;
    recept_data_target[3] = recept_data[5] << 1;
    recept_data_target[4] = (unsigned char)(SOFTWARE_VERSION >> 8);
    recept_data_target[5] = (unsigned char)SOFTWARE_VERSION;

    send_data_crc = crc(&recept_data_target[0], send_value_char_count + 3);
    recept_data_sure_crc[0] = (unsigned char)send_data_crc;
    recept_data_sure_crc[1] = (unsigned char)(send_data_crc >> 8);

    for(i = 0; i < 12; i++)
    {
        uart1_send_byte(senddata_frontpoint[i]);
    }
    for(i = 0; i < (send_value_char_count + 4); i++)
    {
        uart1_send_byte(recept_data_target[i]);
    }
    for(i = 0; i < 2; i++)
    {
        uart1_send_byte(recept_data_sure_crc[i]);
    }
}

/************************************************************************************************
* ������: Protocols_ReadHardware_Version
* ����	: ��Ӳ���汾��(0x1E)
* ����	: None
* ����ֵ: None
************************************************************************************************/
void Protocols_ReadHardware_Version(void)
{
    unsigned char i = 0;
    unsigned int  send_data_crc;

    send_value_char_count = ((unsigned int)(recept_data[4] << 8) + recept_data[5]) << 1;
    senddata_frontpoint[0]  = 0x42;
    senddata_frontpoint[1]  = 0x01;
    senddata_frontpoint[2]  = 0x00;
    senddata_frontpoint[3]  = 0x86;
    senddata_frontpoint[4]  = (unsigned char)(send_value_char_count + 10) >> 8;
    senddata_frontpoint[5]  = (send_value_char_count + 10);
    senddata_frontpoint[6]  = 0x00;
    senddata_frontpoint[7]  = 0x01;
    senddata_frontpoint[10] = 0x00;
    senddata_frontpoint[11] = 0x00;
    senddata_frontpoint[8]  = 0x00;

    if((recept_data_target[2] & 0xf0) == 0x10)
    {
        senddata_frontpoint[9] = 0x02;
    }
    else if((recept_data_target[2] & 0xf0) == 0x20)
    {
        senddata_frontpoint[9] = 0x04;
    }
    else if((recept_data_target[2] & 0xf0) == 0x00)
    {
        senddata_frontpoint[9] = 0x01;
    }
    ///////////////*up is point*///////////////////
    send_value_char_count = ((unsigned int)(recept_data[4] << 8) + recept_data[5]) << 1;
    recept_data_target[0] = recept_data[0];
    recept_data_target[1] = recept_data[1];
    recept_data_target[2] = recept_data[4] << 1;
    recept_data_target[3] = recept_data[5] << 1;
    recept_data_target[4] = (unsigned char)(HARDWAARE_VERSION >> 8);
    recept_data_target[5] = (unsigned char)HARDWAARE_VERSION;

    send_data_crc = crc(&recept_data_target[0], send_value_char_count + 3);
    recept_data_sure_crc[0] = (unsigned char)send_data_crc;
    recept_data_sure_crc[1] = (unsigned char)(send_data_crc >> 8);

    for(i = 0; i < 12; i++)
    {
        uart1_send_byte(senddata_frontpoint[i]);
    }
    for(i = 0; i < (send_value_char_count + 4); i++)
    {
        uart1_send_byte(recept_data_target[i]);
    }
    for(i = 0; i < 2; i++)
    {
        uart1_send_byte(recept_data_sure_crc[i]);
    }
}

/************************************************************************************************
* ������: Protect_Channel
* ����	: ͨ������
* ����	: None
* ����ֵ: None
************************************************************************************************/
void Protect_Channel(void)
{
    if(protect_enable.detail.channel_check == 1)
    {
        //ͨ��Aδ��������
        if(led_protect_flag1 == 1)
        {
            if(port_channel_temp1 != set_parameter.set_channel)
            {
                fault_code_flag1.detail.channel = 1;
                run_parameter.fault_value = port_channel_temp1;
                led_protect_flag1 = 0;
                Power_OFF1();
            }
        }
        //ͨ��Bδ��������
        if(led_protect_flag2 == 1)
        {
            if(port_channel_temp2 != set_parameter.set_channel)
            {
                fault_code_flag2.detail.channel = 1;
                run_parameter.fault_value = port_channel_temp2;
                led_protect_flag2 = 0;
                Power_OFF2();
            }
        }
    }
}

/************************************************************************************************
* ������: Protect_Thermistor
* ����	: ����������Ᵽ��
* ����	: None
* ����ֵ: None
************************************************************************************************/
//void Protect_Thermistor(void)
//{
//	//ͨ��Aδ��������
//	if(((run_parameter.fault_code & 0x00FF) == 0) && (led_protect_flag1 != 1)
//		&&((single_vol_channel_flag == 1) || (single_vol_channel_flag == 3)))
//	{
//		if((thermistor_value1 > set_parameter.set_thermistor_max) || (thermistor_value1 < set_parameter.set_thermistor_min))
//		{
//			run_parameter.fault_code |= 0x0033;
//			run_parameter.fault_value = thermistor_value1;
//			Power_OFF1();
//		}
//	}
//	//ͨ��Bδ��������
//	if(((run_parameter.fault_code & 0x00FF) == 0) && (led_protect_flag2 != 1)
//		&&((single_vol_channel_flag == 1) || (single_vol_channel_flag == 3)))
//	{
//		if((thermistor_value2 > set_parameter.set_thermistor_max) || (thermistor_value2 < set_parameter.set_thermistor_min))
//		{
//			run_parameter.fault_code |= 0x3300;
//			run_parameter.fault_value = thermistor_value2;
//			Power_OFF2();
//		}
//	}
//}

/************************************************************************************************
* ������: Protect_VF
* ����	: ѹ���
* ����	: None
* ����ֵ: None
************************************************************************************************/
void Protect_VF(void)
{
    unsigned char i = 0;
    unsigned short max_temp1 = 1000, max_temp2 = 1000;
    unsigned short min_temp1 = 65535, min_temp2 = 65535;
    if(protect_enable.detail.vf_check == 1)
    {
        //ͨ��Aδ��������
        if(led_protect_flag1 == 1)
        {
            for(i = 0; i < 8; i++)
            {
                if(channel_chose_flag1.bit_all & (1 << i))
                {
                    max_temp1 = (led_negative_voltage1[i] > max_temp1) ? led_negative_voltage1[i] : max_temp1;
                    min_temp1 = (led_negative_voltage1[i] < min_temp1) ? led_negative_voltage1[i] : min_temp1;
                }
            }
            if(((max_temp1 - min_temp1) / 100.00) > set_parameter.set_vf) //��λ������10Ϊ1V
            {
                fault_code_flag1.detail.vf = 1;
                run_parameter.fault_value = max_temp1;
                led_protect_flag1 = 0;
                Power_OFF1();
            }
        }
        //ͨ��Bδ��������
        if(led_protect_flag2 == 1)
        {
            for(i = 0; i < 8; i++)
            {
                if(channel_chose_flag2.bit_all & (1 << i))
                {
                    max_temp2 = (led_negative_voltage1[i] > max_temp2) ? led_negative_voltage1[i] : max_temp2;
                    min_temp2 = (led_negative_voltage1[i] < min_temp2) ? led_negative_voltage1[i] : min_temp2;
                }
            }
            if(((max_temp2 - min_temp2) / 100.00) > set_parameter.set_vf) //��λ������10Ϊ1V
            {
                fault_code_flag2.detail.vf = 1;
                run_parameter.fault_value = max_temp2;
                led_protect_flag2 = 0;
                Power_OFF2();
            }
        }
    }
}

/************************************************************************************************
* ������: Protect_PIN_Short
* ����	: Pin���·����
* ����	: None
* ����ֵ: None
************************************************************************************************/
void Protect_PIN_Short(void)
{
    unsigned char i = 0, j = 0;
    unsigned char temp1 = 0x01, temp2 = 0x01;
    unsigned int  voltage_temp1, voltage_temp2;
//	if(protect_enable.detail.pin_short_check == 1)
//	{
    //ͨ��Aδ��������
    if(led_protect_flag1 == 1)
    {
        POWER_OFF1;
        DAC_WriteData(DAC_OUT1, 0);

        for(i = 0; i < 8; i++)
        {
            HC595_SendCMD1(temp1);
            temp1 <<= 1;
            for(j = i + 1; j < 8; j++)
            {
                CD4051_ChannelSet1(i);
                delay_ms(1);
                voltage_temp1 = AcquireAverage_AD(2);
                if(voltage_temp1 > PIN_SHORT_VALUE)
                {
                    fault_code_flag1.detail.pin = 1;
                    run_parameter.fault_value = i;
                    Power_OFF1();
                    HC595_SendCMD1(0x00);
                }
            }
        }
        HC595_SendCMD1(0x00);
    }
    //ͨ��Bδ��������
    if(led_protect_flag2 == 1)
    {
        POWER_OFF2;
        DAC_WriteData(DAC_OUT2, 0);

        for(i = 0; i < 8; i++)
        {
            HC595_SendCMD2(temp2);
            temp2 <<= 1;
            for(j = i + 1; j < 8; j++)
            {
                CD4051_ChannelSet2(i);
                delay_ms(1);
                voltage_temp2 = AcquireAverage_AD(3);
                if(voltage_temp2 > PIN_SHORT_VALUE)
                {
                    fault_code_flag2.detail.pin = 1;
                    run_parameter.fault_value = i;
                    Power_OFF2();
                    HC595_SendCMD2(0x00);
                }
            }
        }
        HC595_SendCMD2(0x00);
    }
//	}
}

/************************************************************************************************
* ������: Protect_NegativeGND_Short
* ����	: �����Եر���
* ����	: None
* ����ֵ: None
************************************************************************************************/
void Protect_NegativeGND_Short(void)
{
    unsigned char i = 0;
    unsigned int  voltage_temp1, voltage_temp2;
    //ͨ��Aδ��������
    if(led_protect_flag1 == 1)
    {
        DAC_WriteData(DAC_OUT1, 0);
        POWER_OFF1;
        HC595_SendCMD1(0xFF);
        for(i = 0; i < 8; i++)
        {
            CD4051_ChannelSet1(i);
            delay_ms(1);
            voltage_temp1 = AcquireAverage_AD(2);
            if(voltage_temp1 < N_GND_SHORT_VALUE)
            {
                fault_code_flag1.detail.negat_gnd = 1;
                run_parameter.fault_value = i;
                led_protect_flag1 = 0;
                Power_OFF1();
                HC595_SendCMD1(0x00);
            }
        }
        HC595_SendCMD1(0x00);
    }
    //ͨ��Bδ��������
    if(led_protect_flag2 == 1)
    {
        DAC_WriteData(DAC_OUT2, 0);
        POWER_OFF2;
        HC595_SendCMD2(0xFF);
        for(i = 0; i < 8; i++)
        {
            CD4051_ChannelSet2(i);
            delay_ms(1);
            voltage_temp2 = AcquireAverage_AD(3);
            if(voltage_temp2 < N_GND_SHORT_VALUE)
            {
                fault_code_flag2.detail.negat_gnd = 1;
                run_parameter.fault_value = i;
                led_protect_flag2 = 0;
                Power_OFF2();
                HC595_SendCMD2(0x00);
            }
        }
        HC595_SendCMD2(0x00);
    }
}

/************************************************************************************************
* ������: Protect_PositiveGND_Short
* ����	: �����Եر���
* ����	: None
* ����ֵ: None
************************************************************************************************/
void Protect_PositiveGND_Short(void)
{
    unsigned int  voltage_temp1, voltage_temp2;

    //ͨ��Aδ��������
    if(led_protect_flag1 == 1)
    {
        POWER_OFF1;
        LED_TEST_OFF1;
        DAC_WriteData(DAC_OUT1, 0);
        delay_ms(50);
        voltage_temp1 = AcquireAverage_AD(0);
        if(voltage_temp1 < P_GND_SHORT_VALUE)
        {
            Power_OFF1();
            fault_code_flag1.bit_all = 0x11;
            run_parameter.fault_value = voltage_temp1;
            led_protect_flag1 = 0;
        }
        LED_TEST_ON1;
    }
    //ͨ��Bδ��������
    if(led_protect_flag2 == 1)
    {
        POWER_OFF2;
        LED_TEST_OFF2;
        DAC_WriteData(DAC_OUT2, 0);
        delay_ms(50);
        voltage_temp2 = AcquireAverage_AD(1);
        if(voltage_temp2 < P_GND_SHORT_VALUE)
        {
            Power_OFF2();
            fault_code_flag2.bit_all = 0x11;
            run_parameter.fault_value = voltage_temp2;
            led_protect_flag2 = 0;
        }
        LED_TEST_ON2;
    }
}

/************************************************************************************************
* ������: Protect_PositiveNegative_Short
* ����	: �����Ը�������
* ����	: None
* ����ֵ: None
************************************************************************************************/
void Protect_PositiveNegative_Short(void)
{
    unsigned char i = 0;
    unsigned int  voltage_temp1, voltage_temp2;

    //ͨ��Aδ��������
    if(led_protect_flag1 == 1)
    {
        POWER_OFF1;
        DAC_WriteData(DAC_OUT1, 0);
        HC595_SendCMD1(0x00);
        for(i = 0; i < 8; i++)
        {
            CD4051_ChannelSet1(i);
            delay_ms(1);
            voltage_temp1 = AcquireAverage_AD(2);
            if(voltage_temp1 < P_N_SHORT_VALUE)
            {
                fault_code_flag1.bit_all = 0x22;
                run_parameter.fault_value = i;
                Power_OFF1();
                led_protect_flag1 = 0;
                HC595_SendCMD1(0x00);
            }
        }
        HC595_SendCMD1(0x00);
        LED_TEST_ON1;
    }

    //ͨ��Bδ��������
    if(led_protect_flag2 == 1)
    {
        POWER_OFF2;
        DAC_WriteData(DAC_OUT2, 0);
        HC595_SendCMD2(0x00);
        for(i = 0; i < 8; i++)
        {
            CD4051_ChannelSet2(i);
            delay_ms(1);
            voltage_temp2 = AcquireAverage_AD(3);
            if(voltage_temp2 < P_N_SHORT_VALUE)
            {
                fault_code_flag2.bit_all = 0x22;
                run_parameter.fault_value = i;
                Power_OFF2();
                led_protect_flag2 = 0;
                HC595_SendCMD2(0x00);
            }
        }
        HC595_SendCMD2(0x00);
        LED_TEST_ON2;
    }
}

/************************************************************************************************
* ������: Protect_EmptyLoad
* ����	: ���ر���
* ����	: None
* ����ֵ: None
************************************************************************************************/
void Protect_EmptyLoad(void)
{
    if(protect_enable.detail.empty_load_check == 1)
    {
        //ͨ��Aδ��������
        if(led_protect_flag1 == 1)
        {
            if((port_channel_temp1 == 0) && (set_parameter.set_channel != 0))
            {
                fault_code_flag1.detail.empty_load = 1;
                run_parameter.fault_value = port_channel_temp1;
                led_protect_flag1 = 0;
                Power_OFF1();
            }
        }
        //ͨ��Bδ��������
        if(led_protect_flag2 == 1)
        {
            if((port_channel_temp2 == 0) && (set_parameter.set_channel != 0))
            {
                fault_code_flag2.detail.empty_load = 1;
                run_parameter.fault_value = port_channel_temp2;
                led_protect_flag2 = 0;
                Power_OFF2();
            }
        }
    }
}

/************************************************************************************************
* ������: Protect_OVP
* ����	: ��ѹ����
* ����	: None
* ����ֵ: None
************************************************************************************************/
void Protect_OVP(void)
{
    unsigned char i = 0;
    unsigned int  voltage_temp1, voltage_temp2;

    LED_VoltageCurrentDetection();    //��ѹ��� �������
    if(protect_enable.detail.ovp_check == 1)
    {
        //ͨ��Aδ��������
        if(led_protect_flag1 == 1)
        {
            for(i = 0; i < 8; i++)
            {
                if(channel_chose_flag1.bit_all & (1 << i))
                {
                    voltage_temp1 = led_negative_voltage1[i] / 100;
                    if(voltage_temp1 > set_parameter.set_ovp)
                    {
                        fault_code_flag1.detail.ovp = 1;
                        run_parameter.fault_value = led_negative_voltage1[i];
                        led_protect_flag1 = 0;
                        Power_OFF1();
                    }
                }
            }
        }
        //ͨ��Bδ��������
        if(led_protect_flag2 == 1)
        {
            for(i = 0; i < 8; i++)
            {
                if(channel_chose_flag2.bit_all & (1 << i))
                {
                    voltage_temp2 = led_negative_voltage2[i] / 100;
                    if(voltage_temp2 > set_parameter.set_ovp)
                    {
                        fault_code_flag2.detail.ovp = 1;
                        run_parameter.fault_value = led_negative_voltage2[i];
                        led_protect_flag2 = 0;
                        Power_OFF2();
                    }
                }
            }
        } /**END led_protect_flag2 == 1**/
    }
}

/************************************************************************************************
* ������: Protect_UVP
* ����	: Ƿѹ����
* ����	: None
* ����ֵ: None
************************************************************************************************/
void Protect_UVP(void)
{
    unsigned char i = 0;
    unsigned int  voltage_temp1, voltage_temp2;

    if(protect_enable.detail.uvp_check == 1)
    {
        //ͨ��Aδ��������
        if(led_protect_flag1 == 1)
        {
            for(i = 0; i < 8; i++)
            {
                if(channel_chose_flag1.bit_all & (1 << i))
                {
                    voltage_temp1 = led_negative_voltage1[i] / 100;
                    if(voltage_temp1 < set_parameter.set_uvp)
                    {
                        fault_code_flag1.detail.uvp = 1;
                        run_parameter.fault_value = led_negative_voltage1[i];
                        Power_OFF1();
                        led_protect_flag1 = 0;
                    }
                }
            }
        }
        //ͨ��Bδ��������
        if(led_protect_flag2 == 1)
        {
            for(i = 0; i < 8; i++)
            {
                if(channel_chose_flag2.bit_all & (1 << i))
                {
                    voltage_temp2 = led_negative_voltage2[i] / 100; //mv
                    if(voltage_temp2 > set_parameter.set_ovp)
                    {
                        fault_code_flag2.detail.uvp = 1;
                        run_parameter.fault_value = led_negative_voltage2[i];
                        Power_OFF2();
                        led_protect_flag2 = 0;
                    }
                }
            }
        }
    }
}

/************************************************************************************************
* ������: Protect_OverPower
* ����	: �����ʱ���
* ����	: None
* ����ֵ: None
************************************************************************************************/
void Protect_OverPower(void)
{
    unsigned char i = 0;
    unsigned int  power_temp1, power_temp2;

    if(protect_enable.detail.opp_check == 1)
    {
        //ͨ��Aδ��������
        if(led_protect_flag1 == 1)
        {
            for(i = 0; i < 8; i++)
            {
                if(channel_chose_flag1.bit_all & (1 << i))
                {
                    power_temp1 = (led_negative_voltage1[i] * led_negative_current1[i]) / 100000; //mW
                    LED_ChannelDetection();
                    power_temp1 = power_temp1 * port_channel_temp1;
                    if(power_temp1 < set_parameter.set_opp)
                    {
                        fault_code_flag1.detail.opp = 1;
                        run_parameter.fault_value = power_temp1 * 100;
                        Power_OFF1();
                        led_protect_flag1 = 0;
                    }
                }
            }
        }
        //ͨ��Bδ��������
        if(led_protect_flag2 == 1)
        {
            for(i = 0; i < 8; i++)
            {
                if(channel_chose_flag2.bit_all & (1 << i))
                {
                    power_temp2 = (led_negative_voltage2[i] * led_negative_current2[i]) / 100000; //mW
                    LED_ChannelDetection();
                    power_temp1 = power_temp2 * port_channel_temp2;
                    if(power_temp2 < set_parameter.set_opp)
                    {
                        fault_code_flag2.detail.opp = 1;
                        run_parameter.fault_value = power_temp2 * 100;
                        Power_OFF2();
                        led_protect_flag2 = 0;
                    }
                }
            }
        }
    }
}

/************************************************************************************************
* ������: LED_ChannelDetection
* ����	: ͨ�����
* ����	: None
* ����ֵ: None
************************************************************************************************/
void LED_ChannelDetection(void)
{
    unsigned char i;
    port_channel_temp1 = 0;
    port_channel_temp2 = 0;
    channel_chose_flag1.bit_all = 0;
    channel_chose_flag2.bit_all = 0;
    //ͨ��A
    if(led_protect_flag1 == 1)
    {
        for(i = 0; i < 8; i++)
        {
            //���ɼ��������趨ֵ����ʾ��ͨ��OK
            if(led_negative_voltage1[i] > CHANNEL_DECIDE_VALUE)
            {
                port_channel_temp1 ++;
                channel_chose_flag1.bit_all |= (1 << i);
            }
        }
    }
    //ͨ��B
    if(led_protect_flag2 == 1)
    {
        for(i = 0; i < 8; i++)
        {
            //���ɼ��������趨ֵ����ʾ��ͨ��OK
            if(led_negative_voltage2[i] > CHANNEL_DECIDE_VALUE)
            {
                port_channel_temp2 ++;
                channel_chose_flag2.bit_all |= (1 << i);
            }
        }
    }
}

/************************************************************************************************
* ������: LED_VoltageCurrentDetection
* ����	: ��ѹ�������
* ����	: None
* ����ֵ: None
************************************************************************************************/
void LED_VoltageCurrentDetection(void)
{
    unsigned char i = 0;
//	unsigned char temp1 = 0,temp2 = 0;
//	unsigned int  cur_temp1[5],cur_temp2[5];
    unsigned int  led_pvol_temp1 = 0, led_pvol_temp2 = 0;      //������ѹ
    unsigned int  led_pcur_temp1 = 0, led_pcur_temp2 = 0;      //��������
    unsigned int  led_nvol_temp1[8] = {0}, led_nvol_temp2[8] = {0}; //������ѹ
    unsigned int  led_ncur_temp1[8] = {0}, led_ncur_temp2[8] = {0}; //��������

    //printf("---LED_VoltageCurrentDetection---\n");

    //ͨ��A
    if(led_protect_flag1 == 1)
    {
        //������ѹ���
        led_pvol_temp1 = AcquireAverage_AD(0);
        delay_ms(1);
        //�����������
        led_pcur_temp1 = AcquireAverage_AD(4);

        for(i = 0; i < 8; i++)
        {
            CD4051_ChannelSet1(i);
            delay_ms(1);
            led_nvol_temp1[i] = AcquireAverage_AD(2);	//������ѹ���
            delay_ms(1);
            led_ncur_temp1[i] = AcquireAverage_AD(6);   //�����������

            if(led_nvol_temp1[i] < 300)
            {
                led_nvol_temp1[i] = 0;
            }
            if(led_ncur_temp1[i] < 10)
            {
                led_ncur_temp1[i] = 0;
            }
            //������ѹ���
            led_negative_voltage1[i] = led_pvol_temp1 - led_nvol_temp1[i];
            //�����������
            led_negative_current1[i] = led_ncur_temp1[i];

            if(channel_chose_flag1.bit_all & (1 << i))
            {
//                led_negative_voltage1[i] = 0;
                led_negative_current1[i] = 0;
            }
            //printf("������ѹ1[%d] = %d\n",i,led_negative_voltage1[i]);
            //printf("��������1[%d] = %d\n",i,led_negative_current1[i]);
        }
        //������ѹ���
        led_positive_voltage1 = led_pvol_temp1;
        //�����������
        led_positive_current1 = led_pcur_temp1;
        //printf("������ѹ1 = %d\n",led_positive_voltage1);
        //printf("��������1 = %d\n",led_positive_current1);
    }
    //ͨ��B
    if(led_protect_flag2 == 1)
    {
        led_pvol_temp2 = AcquireAverage_AD(1); //������ѹ���
        delay_ms(1);
        //�����������
        led_pcur_temp2 = AcquireAverage_AD(5);
        delay_ms(1);
        for(i = 0; i < 8; i++)
        {
            CD4051_ChannelSet2(i);
            delay_ms(1);
            led_nvol_temp2[i] = AcquireAverage_AD(3);  //������ѹ���
            delay_ms(1);
            //�����������
            led_ncur_temp2[i] = AcquireAverage_AD(7);  //�����������

            //���Ż�����
            if(led_nvol_temp2[i] < 300)
            {
                led_nvol_temp2[i] = 0;
            }
            if(led_ncur_temp2[i] < 10)
            {
                led_ncur_temp2[i] = 0;
            }

            //������ѹ���
            led_negative_voltage2[i] = led_pvol_temp2 - led_nvol_temp2[i];
            //�����������
            led_negative_current2[i] = led_ncur_temp2[i];
			if(channel_chose_flag2.bit_all & (1 << i))
            {
//                led_negative_voltage2[i] = 0;
                led_negative_current2[i] = 0;
            }
            //printf("������ѹ2[%d] = %d\n",i,led_negative_voltage1[i]);
            //printf("��������2[%d] = %d\n",i,led_negative_current1[i]);
        }
        //������ѹ���
        led_positive_voltage2 = led_pvol_temp2;
        //�����������
        led_positive_current2 = led_pcur_temp2;
        //printf("������ѹ2 = %d\n",led_positive_voltage2);
        //printf("��������2 = %d\n",led_positive_current2);
    }
}

/************************************************************************************************
* ������: Thermistor_Detection
* ����	: �����������
* ����	: None
* ����ֵ: None
************************************************************************************************/
void Thermistor_Detection(void)
{
    unsigned short rt_temp_value1 = 0, rt_temp_value2 = 0;

    //ͨ��A
    if(led_protect_flag1 == 1)
    {
        rt_temp_value1 = AcquireAverage_AD(8);	 //�����������
    }
    //ͨ��B
    if(led_protect_flag2 == 1)
    {
        rt_temp_value2 = AcquireAverage_AD(9);	 //�����������
    }
    thermistor_value1 = rt_temp_value1;
    thermistor_value2 = rt_temp_value2;
}

/************************************************************************************************
* ������: Power_ON1
* ����	: ͨ��A ON
* ����	: None
* ����ֵ: None
************************************************************************************************/
void Power_ON1(void)
{
    unsigned short set_current_temp;
//	led_para.set_current=led_para.set_current*20.9;
//	set_current_temp = led_para.set_current*8.2508;  //100ŷķ
//	set_current_temp = led_para.set_current*2.0672;  //25ŷķ
    set_current_temp = set_parameter.set_current * 4.1254; //50ŷķ
    DAC_WriteData(DAC_OUT1, set_current_temp);

    delay_ms(5);
    POWER_ON1;
}

/************************************************************************************************
* ������: Power_ON2
* ����	: ͨ��B ON
* ����	: None
* ����ֵ: None
************************************************************************************************/
void Power_ON2(void)
{
    unsigned short set_current_temp;
//	led_para.set_current=led_para.set_current*20.9;
//	set_current_temp = led_para.set_current*8.2508;  //100ŷķ
//	set_current_temp = led_para.set_current*2.0672;  //25ŷķ
    set_current_temp = set_parameter.set_current * 4.1254; //50ŷķ
    DAC_WriteData(DAC_OUT2, set_current_temp);
    POWER_ON2;
    delay_ms(5);
}

/************************************************************************************************
* ������: Power_OFF1
* ����	: ͨ��A OFF
* ����	: None
* ����ֵ: None
************************************************************************************************/
void Power_OFF1(void)
{
    POWER_OFF1;
    delay_ms(5);
    DAC_WriteData(DAC_OUT1, 0);
    LED14_ON;
    //�����־λ
    //printf("Power_OFF1: fault_code_flag1 = %x\n",fault_code_flag1.bit_all);
}

/************************************************************************************************
* ������: Power_OFF2
* ����	: ͨ��B OFF
* ����	: None
* ����ֵ: None
************************************************************************************************/
void Power_OFF2(void)
{
    POWER_OFF2;
    delay_ms(5);
    DAC_WriteData(DAC_OUT2, 0);
    LED15_ON;
    //�����־λ
    //printf("Power_OFF2: fault_code_flag2 = %x\n",fault_code_flag2.bit_all);
}

/************************************************************************************************
* ������: Start_Output
* ����	: �������
* ����	: None
* ����ֵ: None
************************************************************************************************/
void Start_Output(void)
{
    //����
    if(((led_protect_flag1 == 1) || (led_protect_flag2 == 1)) 							\
            && (run_flag.detail.run_start == 1) && (run_flag.detail.recive_value_err == 0))
    {
        run_flag.detail.run_start = 0;
        run_flag.detail.run_stop = 0;

        fault_code_flag1.bit_all = 0;
        fault_code_flag2.bit_all = 0;
        run_parameter.fault_value = 0;
        LED13_ON;
        LED14_OFF;
        LED15_OFF;
        TIM3_ENABLE();

        Protect_PositiveGND_Short(); 		  //���Եر���
        Protect_PositiveNegative_Short();     //���Ը�����
        Protect_NegativeGND_Short();		  //���Եر���
        Protect_PIN_Short();				  //Pin���·

        light_timer = set_parameter.light_timer;
        start_times_flag = 1;
        while(start_times_flag);  //��ʱ����ʱ100ms

        //ͨ��Aδ��������
        if(led_protect_flag1 == 1)
        {
            Power_ON1();   //A
//			LED_VoltageCurrentDetection();    //��ѹ��� �������
        }
        //ͨ��Bδ��������
        if(led_protect_flag2 == 1)
        {
            Power_ON2();   //B
//			LED_VoltageCurrentDetection();    //��ѹ��� �������
        }
//        LED_VoltageCurrentDetection();    //��ѹ��� �������
    }
}

/************************************************************************************************
* ������: Stop_Output
* ����	: ֹͣ���
* ����	: None
* ����ֵ: None
************************************************************************************************/
void Stop_Output(void)
{
    if(run_flag.detail.run_stop == 1)
    {
        switch(single_vol_channel_flag)
        {
        case 1:
            Power_OFF1();   //A
            LED14_OFF;
            break;
        case 2:
            Power_OFF2();   //B
            LED15_OFF;
            break;
        case 3:
            Power_OFF1();   //A,B
            Power_OFF2();
            LED13_OFF;
            LED14_OFF;
            LED15_OFF;
            TIM3_DISABLE();
            break;
        default:
            break;
        }
        if((led_protect_flag1 == 0) && (led_protect_flag2 == 0))
        {
            LED13_OFF;
            TIM3_DISABLE();

            fault_code_flag1.bit_all = 0;
            fault_code_flag2.bit_all = 0;
            run_parameter.fault_value = 0;
        }
        else if(led_protect_flag1 == 0)
        {
            fault_code_flag1.bit_all = 0;
            run_parameter.fault_value = 0;
        }
        else if(led_protect_flag2 == 0)
        {
            fault_code_flag2.bit_all = 0;
            run_parameter.fault_value = 0;
        }
        run_flag.detail.run_start = 0;
        run_flag.detail.run_stop = 0;
    }
}

/************************************************************************************************
* ������: Hot_Plug
* ����	: �Ȳ��
* ����	: None
* ����ֵ: None
************************************************************************************************/
void Hot_Plug(void)
{
    if(set_parameter.hot_sw_en == 1)
    {
        //ͨ��Aδ��������
        if(led_protect_flag1 == 1)
        {
            if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2) == 0)
            {
                Power_OFF1();
            }
        }
        //ͨ��Bδ��������
        if(led_protect_flag2 == 1)
        {
            if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3) == 0)
            {
                Power_OFF2();
            }
        }
    }
}

/************************************************************************************************
* ������: BSP_Init
* ����	: Ӳ����ʼ��
* ����	: None
* ����ֵ: None
************************************************************************************************/
void BSP_Init(void)
{
    SystemInit();	    // ����ϵͳʱ��Ϊ72M
    delay_init();
    USART1_Init_Config(115200);  //����ͨ��
    USART2_Init_Config(4800);    //���ڵ���
    GPIO_Init_Config();
    HC595_GPIO_Init();
    CD4051_GPIO_Init();
    ADC_DMA_InitConfig();
    DAC_Init_Config();

    DAC_Start_Init();
    StartValue_Init();

    TIM3_Int_Init(99, 7199); //10ms
    TIM3_DISABLE();

    /* �˴�����DAϵ��У��*/
//	DAC_WriteData(DAC_OUT1,0);
//	DAC_WriteData(DAC_OUT1,0);
//	DAC_WriteData(DAC_OUT1,0);
//
//	DAC_WriteData(DAC_OUT2,0);
//	DAC_WriteData(DAC_OUT2,0);
//	DAC_WriteData(DAC_OUT2,0);
}
