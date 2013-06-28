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

void
ledInit(void)
{
	GPIO_InitTypeDef initGPIO;

	initGPIO.Pin  = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |
			GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	initGPIO.MODE = Mode_OUT;
	initGPIO.STATE = High;
	initGPIO.IRQC = None_IRQ;

	GPIO_init(PORT_D, &initGPIO);
}

int 
main(void)
{   
	int speed = 0;
	DisableInterrupts;  //��ȫ���ж�

	ledInit();
	steerInit();	//�����ʼ��
	imgInit();      //����ͷ��ʼ��
	motorInit();    //������Ƴ�ʼ��

#ifdef AT2401
	NRF_Init();
#endif

	EnableInterrupts;   //��ȫ���ж�

	for (;;) 
	{
		speed = imgProcess();
		motorSetSpeed(speed);
	}
}
