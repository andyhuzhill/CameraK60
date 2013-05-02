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
    GPIO_InitTypeDef initGPIO;

    initGPIO.Pin  = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |
            GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    initGPIO.MODE = Mode_OUT;
    initGPIO.STATE = High;
    initGPIO.IRQC = None_IRQ;
    
    GPIO_init(GPIO_D, &initGPIO);

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
        
        motorSetSpeed(encoder_cnt, 10);
    }
}
