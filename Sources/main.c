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
vuint32 speed_cnt=0;      // 编码器采集到的现在的速度值
vuint32  encoder_cnt=0;
volatile bool getEncoder= false;


int 
main(void)
{   

    int retval =0;
    DisableInterrupts;  //关全局中断

    LCD_Init(RED);
    imgInit();      //摄像头初始化
    motorInit();    //电机控制初始化
    steerInit();

    EnableInterrupts;   //开全局中断
    
    motorSetSpeed(encoder_cnt, 40);

    for (;;) 
    {
        imgProcess();
        
        //        for (int i = 40; i < 61; ++i) 
        //        {
        //            steerSetDuty(i);
        //            DELAY_MS(500);
        //        }  
        
    }
}
