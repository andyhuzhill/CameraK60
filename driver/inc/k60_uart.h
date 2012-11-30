/*
 * k60_uart.h
 *
 *  Created on: Nov 23, 2012
 *      Author: scout
 *      E-mail: andyhuzhill@gmail.com
 *    HomePage: http://andyhuzhill.github.com
 *
 * Description: ͨ�ô���ͨ������
 *    Compiler: CodeWarrior 10.2
 * 	   version: 1.0
 */

#ifndef K60_UART_H_
#define K60_UART_H_

#include "common.h"

//���ںŶ���
typedef enum{
    UART_0 = 0,
    UART_1 = 1,
    UART_2 = 2,
    UART_3 = 3,
    UART_4 = 4,
    UART_5 = 5
}UART_TypeDef;

//�����ʶ���
typedef enum{
    Baud_1200 = 1200,
    Baud_2400 = 2400,
    Baud_4800 = 4800,
    Baud_9600 = 9600,
    Baud_19200 = 19200,
    Baud_38400 = 38400,
    Baud_57600 = 57600,
    Baud_115200 = 115200
}BaudRate_TypeDef;

////����λ����
//typedef enum{
//    DataBit_8 = 8,
//    DataBit_9 = 9
//}DataBit_TypeDef;
//
////У��λ����
//typedef enum{
//    Parity_None = 0,
//    Parity_Odd = 1,  // ��У��
//    Parity_Even = 2  // żУ��
//}Parity_TypeDef;

//����ͨ�żĴ�������־λ����
#define ReSendStatusR(base) ((base)->S1) //״̬�Ĵ���
#define ReSendDataR(base)   ((base)->D)  //���ݼĴ���

#define RecFullBit 5  //���ջ���������־
#define SendEmptyBit 7 //���ͻ������ձ�־

///////////////////////////////////////////////////////////////////////////////
//          �ӿں�������
///////////////////////////////////////////////////////////////////////////////

/*
 * @˵��: ��ʼ��UART
 * @����:  uartNo : ���ں�
 *        baudRate: ������
 * @����ֵ: 0 Ϊ��ʼ������������Ϊ�쳣   
 */
uint8
UART_init(UART_TypeDef uartNo, BaudRate_TypeDef baudRate);

/*
 * @˵��: ��Uart����һ���ֽ�
 * @����: uartNo: ���ں�
 *          flag: ���ճɹ���־ָ�� *flag=0�ɹ����� *flag=1, ����ʧ��
 * @����ֵ: ���ؽ����ֽ�
 */
uint8
UART_get_byte(UART_TypeDef uartNo, uint8 *flag);

/*
 * @˵��: ��Uart����һ���ֽ�
 * @����: uartNo: ���ں�
 *          chr: Ҫ���͵��ֽ�
 * @����ֵ: 0 ���ͳɹ��� ����Ϊ�쳣
 */
uint8
UART_send_byte(UART_TypeDef uartNo, const uint8 chr);

/*
 * @˵��: ��Uart����n���ֽ�
 * @����: uartNo: ���ں�
 *          buff: ���ջ�����
 *           len: ���ճ���
 * @����ֵ: 0 �ɹ� ����Ϊ�쳣
 * @ע��: ����������ʱӦ��ʼ��
 *      ���磺
 *      uint8 str[1024]= {0};
 *      UART_get_n_byte(UART_1, &str, 12);
 */
uint8
UART_get_n_byte(UART_TypeDef uartNo, uint8* buff, uint16 len);

/*
 * @˵��: ��Uart����n���ֽ�
 * @����: uartNo: ���ں�
 *          buff: ���ͻ�����
 *           len: ���ͳ���
 * @����ֵ:  0Ϊ�ɹ��� ����Ϊ�쳣
 */
uint8
UART_send_n_byte(UART_TypeDef uartNo, uint8* buff, uint16 len);

/*
 * @˵��: ��Uart����һ���ַ���
 * @����: uartNo: ���ں�
 *           str: �ַ����׵�ַ
 * @����ֵ: 0Ϊ����������Ϊ�쳣
 */
uint8
UART_send_string(UART_TypeDef uartNo,const void * str);



#endif /* K60_UART_H_ */
