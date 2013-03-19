/******************** (C) COPYRIGHT 2011 Ұ��Ƕ��ʽ���������� ********************
 * �ļ���       ��port.c
 * ����         ��PORT�ⲿGPIO�ж�����
 *
 * ʵ��ƽ̨     ��Ұ��kinetis������
 * ��汾       ��
 * Ƕ��ϵͳ     ��
 *
 * ����         ��Ұ��Ƕ��ʽ����������
 * �Ա���       ��http://firestm32.taobao.com
 * ����֧����̳ ��http://www.ourdev.cn/bbs/bbs_list.jsp?bbs_id=1008
**********************************************************************************/

#include "common.h"
#include "k60_gpio.h"
#include "port.h"



/*************************************************************************
*                             Ұ��Ƕ��ʽ����������
*
*  �������ƣ�port_init
*  ����˵����PORT�ⲿGPIO�жϳ�ʼ��
*  ����˵����PTxn        �˿�
*            cfg         �˿��������ã��紥��ѡ�����������ѡ��
*  �������أ���
*  �޸�ʱ�䣺2013-1-19
*  ��    ע��
*************************************************************************/
void  port_init(PTxn ptxn, uint32 cfg )
{
    
    uint8 portx = ptxn>>5;
    uint8 n = ptxn & 0x1f;
    SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK << portx);    //����PORTx�˿�
    
    PORT_PCR_REG(PORTX[portx], n) = PORT_PCR_MUX(1) | cfg; // ����GPIO , ȷ������ģʽ ,������������������
    GPIO_PDDR_REG(GPIOx[portx]) &= ~(1 << n);       //����ģʽ
    //enable_irq(portx + 87);                         //ʹ��PORT�жϣ�PORTA��ISR�жϺ�Ϊ87
}