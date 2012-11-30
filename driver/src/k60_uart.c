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
 *   FIXME: ��ʼ��������λ����żУ��������
 */

#include "k60_uart.h"

//ϵͳʱ��
extern uint32 core_clk_khz;
//ϵͳ����ʱ��
extern uint32 periph_clk_khz;

///////////////////////////////////////////////////////////////////////////////
//              �ڲ�����ʵ��
///////////////////////////////////////////////////////////////////////////////

/*
 * @˵��: ��ô��ڵĻ���ַ
 * @����: uartNo: ���ں�
 * @����ֵ: ���ڵĻ�ַָ��
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
//              �ӿں���ʵ��
///////////////////////////////////////////////////////////////////////////////

/*
 * @˵��: ��ʼ��UART
 * @����:  uartNo : ���ں�
 *        baudRate: ������
 * @����ֵ: 0 Ϊ��ʼ������������Ϊ�쳣   
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
    
    //ʹ�����Ź��ܲ�����ʱ��
    switch(uartNo)
    {
    case UART_0:
        PORTD_PCR7 = PORT_PCR_MUX(0x3); //ʹ��UART0_TXD
        PORTD_PCR6 = PORT_PCR_MUX(0x3); //ʹ��UART0_RXD
        BSET(SIM_SCGC4, SIM_SCGC4_UART0_SHIFT); //����UART0ʱ��
        break;
    case UART_1:
        PORTC_PCR4 = PORT_PCR_MUX(0x3); //ʹ��UART1_TXD
        PORTC_PCR3 = PORT_PCR_MUX(0x3); //ʹ��UART1_RXD
        BSET(SIM_SCGC4, SIM_SCGC4_UART1_SHIFT); //����UART1ʱ��
        break;
    case UART_2:
        PORTD_PCR3 = PORT_PCR_MUX(0x3); //ʹ��UART2_TXD
        PORTD_PCR2 = PORT_PCR_MUX(0x3); //ʹ��UART2_RXD
        BSET(SIM_SCGC4, SIM_SCGC4_UART2_SHIFT); //����UART2ʱ��
        break;
    case UART_3:
        PORTB_PCR11 = PORT_PCR_MUX(0x3); //ʹ��UART3_TXD
        PORTD_PCR6 = PORT_PCR_MUX(0x3); //ʹ��UART3_RXD
        BSET(SIM_SCGC4, SIM_SCGC4_UART3_SHIFT); //����UART3ʱ��
        break;
    case UART_4:
        PORTE_PCR24 = PORT_PCR_MUX(0x3); //ʹ��UART4_TXD
        PORTE_PCR25 = PORT_PCR_MUX(0x3); //ʹ��UART4_RXD
        BSET(SIM_SCGC1, SIM_SCGC1_UART4_SHIFT); //����UART4ʱ��
        break;
    case UART_5:
        PORTE_PCR9 = PORT_PCR_MUX(0x3); //ʹ��UART5_TXD
        PORTE_PCR8 = PORT_PCR_MUX(0x3); //ʹ��UART5_RXD
        BSET(SIM_SCGC1, SIM_SCGC1_UART5_SHIFT); //����UART5ʱ��
        break;
    default :
        return -1;
    }
    
    //���úüĴ���֮ǰ���ȹرշ������ͽ�����
    UART_C2_REG(uartch) &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);
    
#if 0
    //��������λ
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
    
    //����У��λ
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
    
    //���ò�����
    sbr = (uint16)((sysClk * 1000)/(baudRate * 16));
    
    //����UARTx_BDH�Ĵ����г���SBR��ֵ
    tmp = UART_BDH_REG(uartch) & ~(UART_BDH_SBR(0x1f));
    
    UART_BDH_REG(uartch) = tmp | UART_BDH_SBR(((sbr & 0x1f00) >> 8));
    UART_BDL_REG(uartch) = (uint8)(sbr & UART_BDL_SBR_MASK);
    
    //���ò�����΢������
    brfa = (((sysClk * 32000)/(baudRate * 16))-(sbr * 32));
    
    //����UARTx_C4�Ĵ����г���BRFA��ֵ
    tmp = UART_C4_REG(uartch) & ~(UART_C4_BRFA(0x1f));
    UART_C4_REG(uartch) = tmp | UART_C4_BRFA(brfa);
    
    //ʹ�ܷ������ͽ�����
    UART_C2_REG(uartch) |= (UART_C2_TE_MASK | UART_C2_RE_MASK);
    
    return 0;
}

/*
 * @˵��: ��Uart����һ���ֽ�
 * @����: uartNo: ���ں�
 *          flag: ���ճɹ���־ָ�� *flag=0�ɹ����� *flag=1, ����ʧ��
 * @����ֵ: ���ؽ����ֽ�
 */
uint8
UART_get_byte(UART_TypeDef uartNo, uint8 *flag)
{
    //���ַ�������ͨ���ı���һ�е����������ֵ�����л�
#if 1
    uint8 data;
    UART_MemMapPtr uartch = UART_get_base_addr(uartNo);
    if(!uartch)
    {
        *flag = 1;
        return 0; //�˿ںŴ���
    }
    
    //�ȴ����ݽ���
    while (!(UART_S1_REG(uartch) & UART_S1_RDRF_MASK));
    
    data = UART_D_REG(uartch);
    return data;
#else  //���·����ж������� ����ʹ��
    uint8 data;
    uint32 t;
    UART_MemMapPtr uartch = UART_get_base_addr(uartNo);
    if(!uartch) 
    {
        *flag = 1;
        return 0; //�˿ںŴ���
    }
    
    for (t=0 ; t < 0xfbbb; t++) //��ѯָ������
    {
        //�жϽ��ջ������Ƿ���
        if (BGET(ReSendStatusR(uartch), RecFullBit))
        {
            data = ReSendDataR(uartch);
            *flag = 0; //���յ�����
            break;
        }
    }// end for
    if (t >= 0xfbbb)
    {
        data = 0;
        *flag = 1; //δ���յ�����
    }
    return data;
#endif
}

/*
 * @˵��: ��Uart����һ���ֽ�
 * @����: uartNo: ���ں�
 *          chr: Ҫ���͵��ֽ�
 * @����ֵ: 0 ���ͳɹ��� ����Ϊ�쳣
 */
uint8
UART_send_byte(UART_TypeDef uartNo, const uint8 chr)
{
    //���ַ�������ͨ���ı���һ�е����������ֵ�����л�
#if 1
    UART_MemMapPtr uartch = UART_get_base_addr(uartNo);
    if(!uartch) return -1; //�˿ںŴ���
    
    //�ȴ�Uart����
    while(!(UART_S1_REG(uartch) & UART_S1_TDRE_MASK));
    
    //��������
    UART_D_REG(uartch) = chr;
    return 0;
#else //���·����ж������� ����ʹ��
    uint32 t;
    UART_MemMapPtr uartch = UART_get_base_addr(uartNo);
    if(!uartch) return -1; //�˿ںŴ���
    
    for (t=0; t < 0xfbbb; t++) //��ѯָ������
    {
        //���Է��ͻ������Ƿ�Ϊ��
        if (BGET(ReSendStatusR(uartch), SendEmptyBit))
        {
            ReSendDataR(uartch) = chr;
            break;
        }
    } // end for
    if (t >= 0xfbbb)
        return -1; //���ͳ�ʱ
    else
        return 0; //���ͳɹ�
#endif
}

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
UART_get_n_byte(UART_TypeDef uartNo, uint8* buff, uint16 len)
{
    uint16 cnt;
    uint8 flag = 0;
    
    for (cnt=0; cnt < len && 0 ==flag ; cnt++)
    {
        buff[cnt] = UART_get_byte(uartNo, &flag);
    }
    if (cnt >= len)
        return -1; //����ʧ��
    else
        return 0; //���ճɹ�
}

/*
 * @˵��: ��Uart����n���ֽ�
 * @����: uartNo: ���ں�
 *          buff: ���ͻ�����
 *           len: ���ͳ���
 * @����ֵ:  0Ϊ�ɹ��� ����Ϊ�쳣
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
        return -1; //����ʧ��
    else
        return 0;  //���ͳɹ�
}

/*
 * @˵��: ��Uart����һ���ַ���
 * @����: uartNo: ���ں�
 *           str: �ַ����׵�ַ
 * @����ֵ: 0Ϊ����������Ϊ�쳣
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
