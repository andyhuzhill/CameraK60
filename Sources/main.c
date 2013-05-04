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

//全局变量定义
vuint32  speed_cnt=0;      // 编码器采集到的现在的速度值

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
    DisableInterrupts;  //关全局中断
    
    ledInit();
    steerInit();    //舵机控制初始化
    imgInit();      //摄像头初始化
    motorInit();    //电机控制初始化

    
    NRF_Init();

    EnableInterrupts;   //开全局中断
    
    motorSetSpeed(speed_cnt, 40);
    
    printf("Start Work\n");

    for (;;) 
    {
        imgProcess();
        
    }
}
