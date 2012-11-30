/*
 * k60_wdog.c
 *
 *  Created on: Nov 24, 2012
 *      Author: scout
 *      E-mail: andyhuzhill@gmail.com
 *    HomePage: http://andyhuzhill.github.com
 *
 * Description: ���Ź�ģ��
 *    Compiler: CodeWarrior 10.2
 * 	   version: 1.0
 */

#include "k60_wdog.h"


/*
 * @˵��: ��ʼ�����Ź�
 * @����: ��
 * @����ֵ: ��
 */
void
WDOG_init(void)
{
    //���Ƚ������Ź�
    WDOG_unlock();
    //Ԥ��Ƶ
    WDOG_PRESC = 0x0400; //���Ź�ʹ��Ƶ�� 40MHz/5 = 9.6MHz
    //��ֵ ���Ź���ʱʱ����Ϊ1s
    WDOG_TOVALH = 0x0092;
    WDOG_TOVALL = 0x7c00;
    
    //ʹ������ʱ�䣬��ֹ����ģʽ����ֹ���Ź��жϣ��������Ź�
    WDOG_STCTRLH = 0x01d3;
}

/*
 * @˵��: �رտ��Ź�
 * @����: ��
 * @����ֵ: ��
 */
void
WDOG_disable(void)
{
    //���Ƚ������Ź������ܸ����Ź��Ĵ���дֵ
    WDOG_unlock();
    
    // ������Ź�ʹ��λ�Թرտ��Ź�
    WDOG_STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;
}

/*
 * @˵��: �������Ź�
 * @����: ��
 * @����ֵ: ��
 */
void
WDOG_enable(void)
{
    //�������Ź�
    WDOG_unlock();
    //�������Ź�
    WDOG_STCTRLH |= WDOG_STCTRLH_WDOGEN_MASK;
}

/*
 * @˵��: ��λ���Ź��� ι��
 * @����: ��
 * @����ֵ: ��
 */
void
WDOG_feed(void)
{
    WDOG_unlock();
    
    WDOG_PRESC = 0x0400;
    
    //ˢ������
    WDOG_REFRESH = 0xa602;
    WDOG_REFRESH = 0xb480;
}

/*
 * @˵��: �������Ź��ж�
 * @����: ��
 * @����ֵ: ��
 */
void
WDOG_enable_int(void)
{
    WDOG_unlock();
    //�����Ź��ж�
    WDOG_STCTRLH |= WDOG_STCTRLH_IRQRSTEN_MASK;
    //����IRQ�ж�
    enable_irq(22);
}

/*
 * @˵��: �رտ��Ź��ж�
 * @����: ��
 * @����ֵ: ��
 */
void
WDOG_disable_int(void)
{
    WDOG_unlock();
    //�رտ��Ź��ж�
    WDOG_STCTRLH &= ~WDOG_STCTRLH_IRQRSTEN_MASK;
}

/*
 * @˵��:���� ���Ź�
 * @����: ��
 * @����ֵ: ��
 */
void
WDOG_unlock(void)
{
  /* ע��: ��Ҫ�������Դ˺�������
   * ���Ź�������Ҫ��20��ʱ��������д��ָ�����У��������Ի�����CPU��λ
   */  
    
    //�����ж�
    DINT;
    
    WDOG_UNLOCK = 0xc520;
    WDOG_UNLOCK = 0xd928;
    
    //�����ж�
    EINT;
}
