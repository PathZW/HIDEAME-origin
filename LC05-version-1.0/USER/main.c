#include "sys_function.h"
#include "sys_uart.h"
#include "adc.h"
#include "delay.h"
#include "stdio.h"

unsigned char recept_data_target[1100];    		//�������ݻ���
unsigned char timer_protect_flag = 0;    		//��ʱ������ʱ��ʶ
//unsigned char timer_read_vol_cur_flag = 0;  	//����ѹ������ʱ��ʶ
extern unsigned char  led_protect_flag1;		//led������ʾ��־λ
extern unsigned char  led_protect_flag2;	    //led������ʾ��־λ

extern run_parameter_T run_parameter;
extern run_flag_T run_flag;

extern unsigned char  recept_data[1100];        //�������ݽ��ջ���
extern unsigned char  single_vol_channel_flag;  //ͨ��ѡ���־
extern unsigned short single_vol_onoff;    		//����ON/OFF

int main(void)
{
    unsigned short i = 0;

    BSP_Init();
//	LED_VoltageCurrentDetection();
    while(1)
    {
//		printf("************************************\n");
        if(run_flag.detail.receive_ok == 1)
        {
            run_flag.detail.receive_ok = 0;

            for(i = 0; i < 1100; i++)
            {
                recept_data_target[i] = recept_data[i];
            }

            /************�����������*************************************************************************/
            //ON��OFF
            if((recept_data_target[3] == 0x20) && (recept_data_target[1] == 0x10))      //ON��OFF
            {
                Protocols_ON_OFF_Driver();
            }
            //�������ѹ����
            else if((recept_data_target[3] == 0x51) && (recept_data_target[1] == 0x03)) //����ѹ����
            {
                Protocols_Read_VolCur();
            }
            //�ı䱳�����
            else if((recept_data_target[3] == 0x8E) && (recept_data_target[1] == 0x10)) //�ı䱳�����
            {
                Protocols_Change_BacklightCur();
            }
            //д1K����
            else if((recept_data_target[3] == 0x01) && (recept_data_target[1] == 0x10)) //д���ܲ���
            {
                Protocols_Analysis_1K_Parameter();
            }
            //������汾��
            else if((recept_data_target[3] == 0x1E) && (recept_data_target[1] == 0x03)) //������汾��
            {
                Protocols_ReadSoftwear_Version();
            }
            //��Ӳ���汾��
            else if((recept_data_target[3] == 0x1F) && (recept_data_target[1] == 0x03)) //��Ӳ���汾��
            {
                Protocols_ReadHardware_Version();
            }
        }/**** ����������� END	*****/
        //�������
        Start_Output();
        //ֹͣ���
        Stop_Output();
        //��ѹ����
        Protect_OVP();		

        if((timer_protect_flag == 1) && ((led_protect_flag1 == 1) || (led_protect_flag2 == 1)))  //200ms
        {
            timer_protect_flag = 0;

            LED_VoltageCurrentDetection();    	//��ѹ�������
            LED_ChannelDetection();    		  	//ͨ�����
            Thermistor_Detection();           	//����������
            Protect_Channel();       			//ͨ������
            Protect_EmptyLoad();     			//���ر���
            Protect_VF();			 			//ѹ���
            Protect_UVP();			 			//Ƿѹ����
            Protect_OverPower();	 			//�����ʱ���
        }
    }
}



