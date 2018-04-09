#include "sys_uart.h"
#include "stdio.h"
#include "sys.h"



unsigned int recept_step;  //接收步长
unsigned char recept_data[1100];
unsigned int recept_value_char_count;
unsigned int recept_data_crc;
unsigned int temp_recept_data_crc;

run_flag_T run_flag;


//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
    int handle;
};

FILE __stdout;
//定义_sys_exit()以避免使用半主机模式
_sys_exit(int x)
{
    x = x;
}
//重定义fputc函数
int fputc(int ch, FILE *f)
{
//	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕
//  USART1->DR = (u8) ch;
//	return ch;

    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    USART2->DR = (u8)ch;
    return ch;
}
#endif

/************************************************************************************************
* 函数名: crc
* 功能	: 校验位
* 参数	: data[]:需要校验的数  l：长度
* 返回值: 校验位值
************************************************************************************************/
unsigned int crc(unsigned char data[], unsigned int l)
{
    unsigned char temp;
    unsigned int i;
    unsigned char j;
    unsigned int crc8;

    crc8 = 0xFFFF;
    for(i = 0; i <= l; i++)
    {
        crc8 = crc8 ^ data[i];
        for(j = 0; j < 8; j++)
        {
            temp = crc8 % 2;
            crc8 >>= 1;
            if(temp)
                crc8 = crc8 ^ 40961;
        }
    }
    return(crc8);
}



/*******************************************************************************************************
* 函数名: uart1_send_byte
* 功能	: 串口1发送函数
* 参数	: byte:要发送的字节
* 返回值: None
*******************************************************************************************************/
void uart1_send_byte(unsigned char byte)
{
    /* 将Printf内容发往串口 */
    USART_SendData(USART1, byte);
    while (!(USART1->SR & USART_FLAG_TXE));
}


/*******************************************************************************************************
* 函数名: USART1_Init_Config
* 功能	: 串口1初始化配置
* 参数	: bound:波特率
* 返回值: None
*******************************************************************************************************/
void USART1_Init_Config(unsigned int bound)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* 使能 USART1 时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
    //  USART_DeInit(USART1);

    /* USART1 使用IO端口配置 */  //Tx PA 09
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //Rx PA 10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStructure);   //初始化GPIOA

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0 ; //先占优先级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	//从优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* USART1 工作模式配置 */
    USART_InitStructure.USART_BaudRate = bound;	//波特率设置：115200
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//数据位数设置：8位
    USART_InitStructure.USART_StopBits = USART_StopBits_1; 	//停止位设置：1位
    USART_InitStructure.USART_Parity = USART_Parity_No ;  //是否奇偶校验：无
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//硬件流控制模式设置：没有使能
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//接收与发送都使能
    USART_Init(USART1, &USART_InitStructure);  //初始化USART1
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);// USART1使能
}


/*******************************************************************************************************
* 函数名: USART2_Init_Config
* 功能	: 串口2初始化配置
* 参数	: bound:波特率
* 返回值: None
*******************************************************************************************************/
void USART2_Init_Config(unsigned int bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    /* Enable the USART2 Pins Software Remapping */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 | RCC_APB2Periph_AFIO, ENABLE);

    /* Configure USART2 Rx (PA.03) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART2 Tx (PA.02) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Enable the USART2 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

    USART_Init(USART2, &USART_InitStructure);
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    /* Enable USART2 */
    USART_Cmd(USART2, ENABLE);
}



/*******************************************************************************************************
* 函数名: USART1_IRQHandler
* 功能	: 串口1中断
* 参数	: None
* 返回值: None
*******************************************************************************************************/
void USART1_IRQHandler(void)
{
    unsigned char recept_data_buffer;
    if(USART1->SR & (1 << 5))
    {
        recept_data_buffer = USART_ReceiveData(USART1);
        if(run_flag.detail.receive_ok == 0)
        {
            switch (recept_step)
            {
            case 0:
                if(recept_data_buffer == 0x32) //Slave address
                {
                    recept_data[0] = recept_data_buffer;
                    recept_step = 1;
                }
                else
                {
                    recept_step = 0;
                }
                break;

            case 1:
                if(recept_data_buffer == 0x03) //read
                {
                    recept_data[1] = 0x03;
                    recept_step = 2;
                }
                else if(recept_data_buffer == 0x10) //write
                {
                    recept_data[1] = 0x10;
                    recept_step = 2;
                }
                else
                {
                    recept_step = 0;
                }
                break;

            default:
                if((recept_step >= 2) && (recept_step < 1100))
                {
                    recept_data[recept_step] = recept_data_buffer;
                    recept_value_char_count = ((u16)(recept_data[4] << 8) + recept_data[5]) << 1;
                    if(1031 == recept_step)
                    {
                        recept_step = 1031;
                    }
                    if((recept_step == (recept_value_char_count + 9)) && (recept_data[1] == 0x10))
                    {
                        recept_data_crc = crc(&recept_data[0], recept_step - 2);
                        temp_recept_data_crc = (unsigned short)(recept_data[recept_step] << 8) + recept_data[recept_step - 1];
                        if(recept_data_crc == temp_recept_data_crc)
                        {
                            run_flag.detail.receive_ok = 1;
                        }
                        recept_step = 0;
                        return;
                    }
                    else if((recept_step  == 7) && (recept_data[1] == 0x03))
                    {
                        recept_data_crc = crc(&recept_data[0], recept_step - 2);
                        temp_recept_data_crc = (unsigned short)(recept_data[recept_step] << 8) + recept_data[recept_step - 1];

                        if(recept_data_crc == temp_recept_data_crc)
                        {
                            run_flag.detail.receive_ok = 1;
                        }
                        recept_step = 0;
                        return;
                    }
                    recept_step++;
                }
                else
                {
                    recept_step = 0;
                }
            } /**END switch**/
        }
        else
        {
            recept_step = 0;
        }
    }
}

/*******************************************************************************************************
* 函数名: USART2_IRQHandler
* 功能	: 串口2中断
* 参数	: None
* 返回值: None
*******************************************************************************************************/
void USART2_IRQHandler(void)
{
    if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == SET)
    {

    }
}
