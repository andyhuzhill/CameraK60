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
volatile uint16 speed_cnt=0;      // �������ɼ��������ڵ��ٶ�ֵ
vuint16 encoder_cnt=0;
volatile bool getEncoder= false;

int 
main(void)
{   
    DisableInterrupts;  //��ȫ���ж�

    imgInit();      //����ͷ��ʼ��
    motorInit();    //������Ƴ�ʼ��

    EnableInterrupts;   //��ȫ���ж�

    for (;;) 
    {
        decoderSet();
        motorSetSpeed(speed_cnt, DUTY2PWM(20));
        imgProcess();
    }
}
