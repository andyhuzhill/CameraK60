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
volatile uint16 speed_cnt =0;      // �������ɼ��������ڵ��ٶ�ֵ

int 
main(void)
{   
    DisableInterrupts;  //��ȫ���ж�

        imgInit();      //����ͷ��ʼ��
    motorInit();    //������Ƴ�ʼ��
    //    steerInit();    //������Ƴ�ʼ�� 

    EnableInterrupts;   //��ȫ���ж�

    for (;;) 
    {
        motorSetSpeed(speed_cnt, DUTY2PWM(20));
        imgProcess();
    }
}
