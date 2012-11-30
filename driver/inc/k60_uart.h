/*
 * k60_uart.h
 *
 *  Created on: Nov 23, 2012
 *      Author: scout
 *      E-mail: andyhuzhill@gmail.com
 *    HomePage: http://andyhuzhill.github.com
 *
 * Description: 通用串行通信驱动
 *    Compiler: CodeWarrior 10.2
 * 	   version: 1.0
 */

#ifndef K60_UART_H_
#define K60_UART_H_

#include "common.h"

//串口号定义
typedef enum{
    UART_0 = 0,
    UART_1 = 1,
    UART_2 = 2,
    UART_3 = 3,
    UART_4 = 4,
    UART_5 = 5
}UART_TypeDef;

//波特率定义
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

////数据位定义
//typedef enum{
//    DataBit_8 = 8,
//    DataBit_9 = 9
//}DataBit_TypeDef;
//
////校验位定义
//typedef enum{
//    Parity_None = 0,
//    Parity_Odd = 1,  // 奇校验
//    Parity_Even = 2  // 偶校验
//}Parity_TypeDef;

//串行通信寄存器及标志位定义
#define ReSendStatusR(base) ((base)->S1) //状态寄存器
#define ReSendDataR(base)   ((base)->D)  //数据寄存器

#define RecFullBit 5  //接收缓存区满标志
#define SendEmptyBit 7 //发送缓存区空标志

///////////////////////////////////////////////////////////////////////////////
//          接口函数声明
///////////////////////////////////////////////////////////////////////////////

/*
 * @说明: 初始化UART
 * @参数:  uartNo : 串口号
 *        baudRate: 波特率
 * @返回值: 0 为初始化正常，否则为异常   
 */
uint8
UART_init(UART_TypeDef uartNo, BaudRate_TypeDef baudRate);

/*
 * @说明: 从Uart接收一个字节
 * @参数: uartNo: 串口号
 *          flag: 接收成功标志指针 *flag=0成功接收 *flag=1, 接收失败
 * @返回值: 返回接收字节
 */
uint8
UART_get_byte(UART_TypeDef uartNo, uint8 *flag);

/*
 * @说明: 向Uart发送一个字节
 * @参数: uartNo: 串口号
 *          chr: 要发送的字节
 * @返回值: 0 发送成功， 否则为异常
 */
uint8
UART_send_byte(UART_TypeDef uartNo, const uint8 chr);

/*
 * @说明: 从Uart接收n个字节
 * @参数: uartNo: 串口号
 *          buff: 接收缓存区
 *           len: 接收长度
 * @返回值: 0 成功 否则为异常
 * @注意: 缓存区定义时应初始化
 *      例如：
 *      uint8 str[1024]= {0};
 *      UART_get_n_byte(UART_1, &str, 12);
 */
uint8
UART_get_n_byte(UART_TypeDef uartNo, uint8* buff, uint16 len);

/*
 * @说明: 向Uart发送n个字节
 * @参数: uartNo: 串口号
 *          buff: 发送缓存区
 *           len: 发送长度
 * @返回值:  0为成功， 否则为异常
 */
uint8
UART_send_n_byte(UART_TypeDef uartNo, uint8* buff, uint16 len);

/*
 * @说明: 向Uart发送一条字符串
 * @参数: uartNo: 串口号
 *           str: 字符串首地址
 * @返回值: 0为正常，否则为异常
 */
uint8
UART_send_string(UART_TypeDef uartNo,const void * str);



#endif /* K60_UART_H_ */
