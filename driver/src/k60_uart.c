/*
 * k60_uart.c
 *
 *  Created on: Nov 23, 2012
 *      Author: scout
 *      E-mail: andyhuzhill@gmail.com
 *    HomePage: http://andyhuzhill.github.com
 *
 * Description:
 *    Compiler: 
 * 	   version:
 *   FIXME: 初始化的数据位和奇偶校验有问题
 */

#include "k60_uart.h"

//系统时钟
extern uint32 core_clk_khz;
//系统总线时钟
extern uint32 periph_clk_khz;

///////////////////////////////////////////////////////////////////////////////
//              内部函数实现
///////////////////////////////////////////////////////////////////////////////

/*
 * @说明: 获得串口的基地址
 * @参数: uartNo: 串口号
 * @返回值: 串口的基址指针
 */
static UART_MemMapPtr UART_get_base_addr(UART_TypeDef uartNo)
{
    switch(uartNo)
    {
    case UART_0:
        return UART0_BASE_PTR;
        break;
    case UART_1:
        return UART1_BASE_PTR;
        break;      
    case UART_2:
        return UART2_BASE_PTR;
        break;      
    case UART_3:
        return UART3_BASE_PTR;
        break;    
    case UART_4:
        return UART4_BASE_PTR;
        break;
    case UART_5:
        return UART5_BASE_PTR;
        break;
    default :
        return 0;   
    }
}

///////////////////////////////////////////////////////////////////////////////
//              接口函数实现
///////////////////////////////////////////////////////////////////////////////

/*
 * @说明: 初始化UART
 * @参数:  uartNo : 串口号
 *        baudRate: 波特率
 * @返回值: 0 为初始化正常，否则为异常   
 */
uint8
UART_init(UART_TypeDef uartNo, BaudRate_TypeDef baudRate)
{
    register uint16 sbr, brfa;
    uint8 tmp;
    uint32 sysClk;
    UART_MemMapPtr uartch = UART_get_base_addr(uartNo);
    
    if (!uartch) return -1;
    
    if ((UART_0 == uartNo) || (UART_1 == uartNo))      
        sysClk = core_clk_khz;
    else
        sysClk = periph_clk_khz;
    
    //使能引脚功能并启动时钟
    switch(uartNo)
    {
    case UART_0:
        PORTD_PCR7 = PORT_PCR_MUX(0x3); //使能UART0_TXD
        PORTD_PCR6 = PORT_PCR_MUX(0x3); //使能UART0_RXD
        BSET(SIM_SCGC4, SIM_SCGC4_UART0_SHIFT); //启动UART0时钟
        break;
    case UART_1:
        PORTC_PCR4 = PORT_PCR_MUX(0x3); //使能UART1_TXD
        PORTC_PCR3 = PORT_PCR_MUX(0x3); //使能UART1_RXD
        BSET(SIM_SCGC4, SIM_SCGC4_UART1_SHIFT); //启动UART1时钟
        break;
    case UART_2:
        PORTD_PCR3 = PORT_PCR_MUX(0x3); //使能UART2_TXD
        PORTD_PCR2 = PORT_PCR_MUX(0x3); //使能UART2_RXD
        BSET(SIM_SCGC4, SIM_SCGC4_UART2_SHIFT); //启动UART2时钟
        break;
    case UART_3:
        PORTB_PCR11 = PORT_PCR_MUX(0x3); //使能UART3_TXD
        PORTD_PCR6 = PORT_PCR_MUX(0x3); //使能UART3_RXD
        BSET(SIM_SCGC4, SIM_SCGC4_UART3_SHIFT); //启动UART3时钟
        break;
    case UART_4:
        PORTE_PCR24 = PORT_PCR_MUX(0x3); //使能UART4_TXD
        PORTE_PCR25 = PORT_PCR_MUX(0x3); //使能UART4_RXD
        BSET(SIM_SCGC1, SIM_SCGC1_UART4_SHIFT); //启动UART4时钟
        break;
    case UART_5:
        PORTE_PCR9 = PORT_PCR_MUX(0x3); //使能UART5_TXD
        PORTE_PCR8 = PORT_PCR_MUX(0x3); //使能UART5_RXD
        BSET(SIM_SCGC1, SIM_SCGC1_UART5_SHIFT); //启动UART5时钟
        break;
    default :
        return -1;
    }
    
    //配置好寄存器之前，先关闭发送器和接收器
    UART_C2_REG(uartch) &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);
    
#if 0
    //设置数据位
    switch(databit)
    {
    case DataBit_8:
        UART_C1_REG(uartch) &= ~UART_C1_M_MASK;
        break;
    case DataBit_9:
        UART_C1_REG(uartch) |= UART_C1_M_MASK;
        break;
    default:
        break;
    }
    
    //设置校验位
    switch(parity)
    {
    case Parity_None:
        UART_C1_REG(uartch) &= ~UART_C1_PE_MASK;
        break;
    case Parity_Odd:
        UART_C1_REG(uartch) |= (UART_C1_PE_MASK | UART_C1_PT_MASK);
        break;
    case Parity_Even:
        UART_C1_REG(uartch) |= (UART_C1_PE_MASK);
        UART_C1_REG(uartch) &= ~(UART_C1_PT_MASK);
        break;
    default:
        break; 
    }
#endif 
    
    //设置波特率
    sbr = (uint16)((sysClk * 1000)/(baudRate * 16));
    
    //保存UARTx_BDH寄存器中除了SBR的值
    tmp = UART_BDH_REG(uartch) & ~(UART_BDH_SBR(0x1f));
    
    UART_BDH_REG(uartch) = tmp | UART_BDH_SBR(((sbr & 0x1f00) >> 8));
    UART_BDL_REG(uartch) = (uint8)(sbr & UART_BDL_SBR_MASK);
    
    //配置波特率微调分数
    brfa = (((sysClk * 32000)/(baudRate * 16))-(sbr * 32));
    
    //配置UARTx_C4寄存器中除了BRFA的值
    tmp = UART_C4_REG(uartch) & ~(UART_C4_BRFA(0x1f));
    UART_C4_REG(uartch) = tmp | UART_C4_BRFA(brfa);
    
    //使能发送器和接收器
    UART_C2_REG(uartch) |= (UART_C2_TE_MASK | UART_C2_RE_MASK);
    
    return 0;
}

/*
 * @说明: 从Uart接收一个字节
 * @参数: uartNo: 串口号
 *          flag: 接收成功标志指针 *flag=0成功接收 *flag=1, 接收失败
 * @返回值: 返回接收字节
 */
uint8
UART_get_byte(UART_TypeDef uartNo, uint8 *flag)
{
    //两种方法可以通过改变下一行的条件编译的值进行切换
#if 1
    uint8 data;
    UART_MemMapPtr uartch = UART_get_base_addr(uartNo);
    if(!uartch)
    {
        *flag = 1;
        return 0; //端口号错误
    }
    
    //等待数据接收
    while (!(UART_S1_REG(uartch) & UART_S1_RDRF_MASK));
    
    data = UART_D_REG(uartch);
    return data;
#else  //以下方法有丢包现象 不再使用
    uint8 data;
    uint32 t;
    UART_MemMapPtr uartch = UART_get_base_addr(uartNo);
    if(!uartch) 
    {
        *flag = 1;
        return 0; //端口号错误
    }
    
    for (t=0 ; t < 0xfbbb; t++) //查询指定次数
    {
        //判断接收缓存区是否满
        if (BGET(ReSendStatusR(uartch), RecFullBit))
        {
            data = ReSendDataR(uartch);
            *flag = 0; //接收到数据
            break;
        }
    }// end for
    if (t >= 0xfbbb)
    {
        data = 0;
        *flag = 1; //未接收到数据
    }
    return data;
#endif
}

/*
 * @说明: 向Uart发送一个字节
 * @参数: uartNo: 串口号
 *          chr: 要发送的字节
 * @返回值: 0 发送成功， 否则为异常
 */
uint8
UART_send_byte(UART_TypeDef uartNo, const uint8 chr)
{
    //两种方法可以通过改变下一行的条件编译的值进行切换
#if 1
    UART_MemMapPtr uartch = UART_get_base_addr(uartNo);
    if(!uartch) return -1; //端口号错误
    
    //等待Uart就绪
    while(!(UART_S1_REG(uartch) & UART_S1_TDRE_MASK));
    
    //发送数据
    UART_D_REG(uartch) = chr;
    return 0;
#else //以下方法有丢包现象 不再使用
    uint32 t;
    UART_MemMapPtr uartch = UART_get_base_addr(uartNo);
    if(!uartch) return -1; //端口号错误
    
    for (t=0; t < 0xfbbb; t++) //查询指定次数
    {
        //测试发送缓存区是否为空
        if (BGET(ReSendStatusR(uartch), SendEmptyBit))
        {
            ReSendDataR(uartch) = chr;
            break;
        }
    } // end for
    if (t >= 0xfbbb)
        return -1; //发送超时
    else
        return 0; //发送成功
#endif
}

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
UART_get_n_byte(UART_TypeDef uartNo, uint8* buff, uint16 len)
{
    uint16 cnt;
    uint8 flag = 0;
    
    for (cnt=0; cnt < len && 0 ==flag ; cnt++)
    {
        buff[cnt] = UART_get_byte(uartNo, &flag);
    }
    if (cnt >= len)
        return -1; //接收失败
    else
        return 0; //接收成功
}

/*
 * @说明: 向Uart发送n个字节
 * @参数: uartNo: 串口号
 *          buff: 发送缓存区
 *           len: 发送长度
 * @返回值:  0为成功， 否则为异常
 */
uint8
UART_send_n_byte(UART_TypeDef uartNo, uint8* buff, uint16 len)
{
    uint16 cnt;
    
    for (cnt = 0; cnt < len; cnt++)
    {
        if (UART_send_byte(uartNo, buff[cnt]) != 0) break;
    }
    if (cnt >= len)
        return -1; //发送失败
    else
        return 0;  //发送成功
}

/*
 * @说明: 向Uart发送一条字符串
 * @参数: uartNo: 串口号
 *           str: 字符串首地址
 * @返回值: 0为正常，否则为异常
 */
uint8
UART_send_string(UART_TypeDef uartNo,const void * str)
{
    uint16 cnt = 0;
    uint8 *buff_ptr = (uint8 *) str;
    for (cnt = 0; buff_ptr[cnt] !='\0'; cnt++)
        if (UART_send_byte(uartNo, buff_ptr[cnt]) != 0)
            return -1;
    return 0;
}
