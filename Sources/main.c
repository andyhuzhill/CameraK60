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
vuint32 speed_cnt=0;      // �������ɼ��������ڵ��ٶ�ֵ
vuint32  encoder_cnt=0;
volatile bool getEncoder= false;

extern uint8 *srcImg;

int 
main(void)
{   
    int retval =0;
    DisableInterrupts;  //��ȫ���ж�

    imgInit();      //����ͷ��ʼ��
    motorInit();    //������Ƴ�ʼ��
    steerInit();

    EnableInterrupts;   //��ȫ���ж�

    for (;;) 
    {
        decoderSet();
        motorSetSpeed(speed_cnt, 20);
        
        retval = imgProcess();
        
        steerSetDuty(retval);
    }
}
