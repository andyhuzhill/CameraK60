/**
 * =====================================================
 * main.c
 *
 *  Created on: Mar 19, 2013
 *      Author: scout
 *		E-mail: andyhuzhill@gmail.com
 *    Homepage: http://andyhuzhill.github.com
 *	   
 * Description: ������
 *    Revision:
 * =====================================================
 */


#include "derivative.h" /* include peripheral declarations */

//ȫ�ֱ�������
volatile uint16   speed_cnt =0;      // �������ɼ��������ڵ��ٶ�ֵ

int 
main(void)
{   
    uint8 h8,l8;
    uint8 duty=0;
    uint8 txbuf[3]={0};  //���ͻ�����
    uint8 status;       //�����жϽ���/����״̬
    DisableInterrupts;  //��ȫ���ж�
    
    imgInit();      //����ͷ��ʼ��
    motorInit();    //������Ƴ�ʼ��
    steerInit();    //������Ƴ�ʼ��

    EnableInterrupts;   //��ȫ���ж�

    for (;;) 
    {
        motorSetSpeed(speed_cnt, DUTY2PWM(10));
    }
}
