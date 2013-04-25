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
volatile uint16 speed_cnt;      // 编码器采集到的现在的速度值
vuint16 encoder_cnt;
volatile bool getEncoder= false;

int 
main(void)
{   
    DisableInterrupts;  //关全局中断

    imgInit();      //摄像头初始化
    motorInit();    //电机控制初始化

    EnableInterrupts;   //开全局中断

    for (;;) 
    {
        encoder_cnt = 0;
        DELAY_MS(10);
        speed_cnt = encoder_cnt;
        getEncoder = true;
        motorSetSpeed(speed_cnt, DUTY2PWM(20));
        imgProcess();
    }
}
