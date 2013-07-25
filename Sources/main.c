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

//全局变量
speedChoice choice;				//速度选择
int32 imgcount = 0;				//计算图像采集的场数, 用来跳过一开始的起跑线检测
extern vint8 startLine ;		//是否检测起跑线  0 为不检测  1 为检测
int8 isStraight = 1;			//是否是直直道	 1 为直道    0 为非直道

/*
 * 函数功能: 初始化核心板上的LED灯作为程序的指示
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
 * 函数功能: 获取拨码开关的值 觉得速度策略
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
	
	startLine = 1;		// 检测起跑线
	isStraight = 1;		// 是直道

	DELAY_MS(2000);		// 规则规定发车需延时两秒钟
	
	for(;;)
	{
		speed = imgProcess();	// 图像处理 舵机控制 并计算速度控制值
		motorSetSpeed(speed);	// 速度控制
	}
}
