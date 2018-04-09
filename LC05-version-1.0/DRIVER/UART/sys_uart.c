#include "sys_uart.h"
#include "stdio.h"
#include "sys.h"



unsigned int recept_step;  //���ղ���
unsigned char recept_data[1100];
unsigned int recept_value_char_count;
unsigned int recept_data_crc;
unsigned int temp_recept_data_crc;

run_flag_T run_flag;


//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB
#if 1
#pragma import(__use_no_semihosting)
//��׼����Ҫ��֧�ֺ���
struct __FILE
{
    int handle;
};

FILE __stdout;
//����_sys_exit()�Ա���ʹ�ð�����ģʽ
_sys_exit(int x)
{
    x = x;
}
//�ض���fputc����
int fputc(int ch, FILE *f)
{
//	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������
//  USART1->DR = (u8) ch;
//	return ch;

    while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
    USART2->DR = (u8)ch;
    return ch;
}
#endif

/************************************************************************************************
* ������: crc
* ����	: У��λ
* ����	: data[]:��ҪУ�����  l������
* ����ֵ: У��λֵ
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
* ������: uart1_send_byte
* ����	: ����1���ͺ���
* ����	: byte:Ҫ���͵��ֽ�
* ����ֵ: None
*******************************************************************************************************/
void uart1_send_byte(unsigned char byte)
{
    /* ��Printf���ݷ������� */
    USART_SendData(USART1, byte);
    while (!(USART1->SR & USART_FLAG_TXE));
}


/*******************************************************************************************************
* ������: USART1_Init_Config
* ����	: ����1��ʼ������
* ����	: bound:������
* ����ֵ: None
*******************************************************************************************************/
void USART1_Init_Config(unsigned int bound)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    /* ʹ�� USART1 ʱ��*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
    //  USART_DeInit(USART1);

    /* USART1 ʹ��IO�˿����� */  //Tx PA 09
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�����������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //Rx PA 10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);   //��ʼ��GPIOA

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0 ; //��ռ���ȼ�
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	//�����ȼ�
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* USART1 ����ģʽ���� */
    USART_InitStructure.USART_BaudRate = bound;	//���������ã�115200
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//����λ�����ã�8λ
    USART_InitStructure.USART_StopBits = USART_StopBits_1; 	//ֹͣλ���ã�1λ
    USART_InitStructure.USART_Parity = USART_Parity_No ;  //�Ƿ���żУ�飺��
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//Ӳ��������ģʽ���ã�û��ʹ��
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//�����뷢�Ͷ�ʹ��
    USART_Init(USART1, &USART_InitStructure);  //��ʼ��USART1
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);// USART1ʹ��
}


/*******************************************************************************************************
* ������: USART2_Init_Config
* ����	: ����2��ʼ������
* ����	: bound:������
* ����ֵ: None
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
* ������: USART1_IRQHandler
* ����	: ����1�ж�
* ����	: None
* ����ֵ: None
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
* ������: USART2_IRQHandler
* ����	: ����2�ж�
* ����	: None
* ����ֵ: None
*******************************************************************************************************/
void USART2_IRQHandler(void)
{
    if(USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == SET)
    {

    }
}
