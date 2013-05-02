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


int 
main(void)
{   

    int retval =0;
    DisableInterrupts;  //��ȫ���ж�

    LCD_Init(RED);
    imgInit();      //����ͷ��ʼ��
    motorInit();    //������Ƴ�ʼ��
    steerInit();

    EnableInterrupts;   //��ȫ���ж�
    
    motorSetSpeed(encoder_cnt, 40);

    for (;;) 
    {
        imgProcess();
        
        //        for (int i = 40; i < 61; ++i) 
        //        {
        //            steerSetDuty(i);
        //            DELAY_MS(500);
        //        }  
        
    }
}
