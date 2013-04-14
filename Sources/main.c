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
volatile uint16   speed_cnt =0;      // 编码器采集到的现在的速度值

int 
main(void)
{   
    uint8 h8,l8;
    uint8 duty=0;
    uint8 txbuf[3]={0};  //发送缓存区
    uint8 status;       //用于判断接受/发送状态
    DisableInterrupts;  //关全局中断
    
    imgInit();      //摄像头初始化
    motorInit();    //电机控制初始化
    steerInit();    //舵机控制初始化

    EnableInterrupts;   //开全局中断

    for (;;) 
    {
        motorSetSpeed(speed_cnt, DUTY2PWM(10));
    }
}
