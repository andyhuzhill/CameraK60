/**
 * =====================================================
 * main.c
 *
 *  Created on: Mar 19, 2013
 *      Author: scout
 *		E-mail: andyhuzhill@gmail.com
 *    Homepage: http://andyhuzhill.github.com
 *	   
 * Description: 主程序
 *    Revision:
 * =====================================================
 */

#include "derivative.h" /* include peripheral declarations */

speedChoice choice;
int32 imgcount = 0;

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

void
getSpeedChoice(void)
{
	gpio_init(PORT_D, 12, Mode_IN, High);
	gpio_init(PORT_D, 13, Mode_IN, High);

	switch(((GPIOD_PDIR & (0x3 << 12)) >> 12))
	{
	case 0:
		choice = LOWEST;
		maxspeed = 7;
		minspeed = 7;
		break;
	case 1:
		choice = MID;
		maxspeed = 10;
		minspeed = 7;
		break;
	case 2:
		choice = FASTER;
		maxspeed = 12;
		minspeed = 5;
		break;
	case 3:
		choice = FASTEST;
		maxspeed = 7;
		minspeed = 7;
		break;
	default:
		choice = LOWEST;
		maxspeed = 7;
		minspeed = 7;
		break;
	}
}

int 
main(void)
{   
	int speed = 0;
	int i = 0;
	DisableInterrupts;  //关全局中断

	ledInit();
	steerInit();	//舵机初始化
	imgInit();      //摄像头初始化
	motorInit();    //电机控制初始化
	getSpeedChoice();

#ifdef AT2401
	NRF_Init();
#endif

	EnableInterrupts;   //开全局中断

	//	DELAY_MS(2000);

	while(1) 
	{
		speed = imgProcess();
		motorSetSpeed(speed);
	}
}
