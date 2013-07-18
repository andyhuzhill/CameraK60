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
extern vint8 startLine ;

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
startLineInit(void)
{
//	gpio_init(PORT_B, 20, Mode_IN, High);
//	gpio_init(PORT_B, 21, Mode_IN, High);
//	gpio_init(PORT_B, 22, Mode_IN, High);
//	gpio_init(PORT_B, 23, Mode_IN, High);
	//	
	port_init(PTB20, IRQ_FALLING | PULLUP);
	port_init(PTB21, IRQ_FALLING | PULLUP);
	port_init(PTB22, IRQ_FALLING | PULLUP);
	port_init(PTB23, IRQ_FALLING | PULLUP);
//
//	//	disable_irq(PORTB_IRQn);
	enable_irq(PORTB_IRQn);
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
		maxspeed = 10;
		minspeed = 10;
		break;
	case 1:
		choice = MID;
		maxspeed = 15;
		minspeed = 8;
		break;
	case 2:
		choice = FASTER;
		maxspeed = 18;
		minspeed = 7;
		break;
	case 3:
		choice = FASTEST;
		maxspeed = 30;
		minspeed = 0;
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
//	startLineInit();

#ifdef AT2401
	NRF_Init();
#endif

	EnableInterrupts;   //开全局中断

	//	DELAY_MS(2000);

	while(1) 
	{
//		GPIOD_PDOR |= ((GPIOB_PDIR & (0xf<<20)) >> 20) << 8;
		speed = imgProcess();
		motorSetSpeed(speed);
	}
}
