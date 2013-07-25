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

//ȫ�ֱ���
speedChoice choice;				//�ٶ�ѡ��
int32 imgcount = 0;				//����ͼ��ɼ��ĳ���, ��������һ��ʼ�������߼��
extern vint8 startLine ;		//�Ƿ���������  0 Ϊ�����  1 Ϊ���
int8 isStraight = 1;			//�Ƿ���ֱֱ��	 1 Ϊֱ��    0 Ϊ��ֱ��

/*
 * ��������: ��ʼ�����İ��ϵ�LED����Ϊ�����ָʾ
 */
void
ledInit(void)
{
	GPIO_InitTypeDef initGPIO;

	initGPIO.Pin  = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |
			GPIO_Pin_14 | GPIO_Pin_15;
	initGPIO.MODE = Mode_OUT;
	initGPIO.STATE = High;
	initGPIO.IRQC = None_IRQ;

	GPIO_init(PORT_D, &initGPIO);
}

/*
 * ��������: ��ȡ���뿪�ص�ֵ �����ٶȲ���
 */
void
getSpeedChoice(void)
{
	gpio_init(PORT_D, 12, Mode_IN, High);
	gpio_init(PORT_D, 13, Mode_IN, High);

	switch(((GPIOD_PDIR & (0x3 << 12)) >> 12))
	{
	case 0:
		choice = LOWEST;
		maxspeed = 10;
		minspeed = 10;
		break;
	case 1:
		choice = MID;
		maxspeed = 15;
		minspeed = 5;
		break;
	case 2:
		choice = FASTER;
		maxspeed = 20;
		minspeed = 1;
		break;
	case 3:
		choice = FASTEST;
		maxspeed = 30;
		minspeed = -5;
		break;
	default:
		choice = LOWEST;
		maxspeed = 10;
		minspeed = 10;
		break;
	}
}

int 
main(void)
 {   
	int speed = 0;
	int i = 0;
	DisableInterrupts;  //��ȫ���ж�

	ledInit();
	steerInit();	//�����ʼ��
	imgInit();      //����ͷ��ʼ��
	motorInit();    //������Ƴ�ʼ��
	getSpeedChoice();

#ifdef AT2401
	NRF_Init();
#endif

	EnableInterrupts;   //��ȫ���ж�
	
	startLine = 1;		// ���������
	isStraight = 1;		// ��ֱ��

	DELAY_MS(2000);		// ����涨��������ʱ������
	
	for(;;)
	{
		speed = imgProcess();	// ͼ���� ������� �������ٶȿ���ֵ
		motorSetSpeed(speed);	// �ٶȿ���
	}
}
