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
vuint32  speed_cnt=0;      // �������ɼ��������ڵ��ٶ�ֵ

volatile bool getEncoder= false;

void
ledInit(void)
{
    GPIO_InitTypeDef initGPIO;

    initGPIO.Pin  = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |
            GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    initGPIO.MODE = Mode_OUT;
    initGPIO.STATE = High;
    initGPIO.IRQC = None_IRQ;
    
    GPIO_init(GPIO_D, &initGPIO);
}


int 
main(void)
{   
    DisableInterrupts;  //��ȫ���ж�
    
    ledInit();
    imgInit();      //����ͷ��ʼ��
    motorInit();    //������Ƴ�ʼ��
    steerInit();    //������Ƴ�ʼ��
    
    NRF_Init();

    EnableInterrupts;   //��ȫ���ж�
    
    motorSetSpeed(speed_cnt, 30);
    
    printf("Start Work\n");

    for (;;) 
    {
        imgProcess();
        
    }
}
