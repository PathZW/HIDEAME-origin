#include "sys_function.h"
#include "sys_uart.h"
#include "adc.h"
#include "delay.h"
#include "stdio.h"

unsigned char recept_data_target[1100];    		//接收数据缓存
unsigned char timer_protect_flag = 0;    		//定时保护计时标识
//unsigned char timer_read_vol_cur_flag = 0;  	//读电压电流计时标识
extern unsigned char  led_protect_flag1;		//led保护显示标志位
extern unsigned char  led_protect_flag2;	    //led保护显示标志位

extern run_parameter_T run_parameter;
extern run_flag_T run_flag;

extern unsigned char  recept_data[1100];        //串口数据接收缓存
extern unsigned char  single_vol_channel_flag;  //通道选择标志
extern unsigned short single_vol_onoff;    		//单独ON/OFF

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

            /************命令解析处理*************************************************************************/
            //ON、OFF
            if((recept_data_target[3] == 0x20) && (recept_data_target[1] == 0x10))      //ON、OFF
            {
                Protocols_ON_OFF_Driver();
            }
            //读背光电压电流
            else if((recept_data_target[3] == 0x51) && (recept_data_target[1] == 0x03)) //读电压电流
            {
                Protocols_Read_VolCur();
            }
            //改变背光电流
            else if((recept_data_target[3] == 0x8E) && (recept_data_target[1] == 0x10)) //改变背光电流
            {
                Protocols_Change_BacklightCur();
            }
            //写1K参数
            else if((recept_data_target[3] == 0x01) && (recept_data_target[1] == 0x10)) //写功能参数
            {
                Protocols_Analysis_1K_Parameter();
            }
            //读软件版本号
            else if((recept_data_target[3] == 0x1E) && (recept_data_target[1] == 0x03)) //读软件版本号
            {
                Protocols_ReadSoftwear_Version();
            }
            //读硬件版本号
            else if((recept_data_target[3] == 0x1F) && (recept_data_target[1] == 0x03)) //读硬件版本号
            {
                Protocols_ReadHardware_Version();
            }
        }/**** 命令解析处理 END	*****/
        //启动输出
        Start_Output();
        //停止输出
        Stop_Output();
        //过压保护
        Protect_OVP();		

        if((timer_protect_flag == 1) && ((led_protect_flag1 == 1) || (led_protect_flag2 == 1)))  //200ms
        {
            timer_protect_flag = 0;

            LED_VoltageCurrentDetection();    	//电压电流侦测
            LED_ChannelDetection();    		  	//通道检测
            Thermistor_Detection();           	//热敏电阻检测
            Protect_Channel();       			//通道保护
            Protect_EmptyLoad();     			//空载保护
            Protect_VF();			 			//压差保护
            Protect_UVP();			 			//欠压保护
            Protect_OverPower();	 			//过功率保护
        }
    }
}



