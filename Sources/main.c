/*
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
#include "img_process.h"
#include "controller.h"

//全局变量定义
uint16  motor_cnt =0;
uint8   img_bin_buff[CAMERA_SIZE];

int 
main(void)
{   
    DisableInterrupts;  //关全局中断
    
    controllerInit();
    motorInit();
//    steerInit();
    
    EnableInterrupts;   //开全局中断
    
    for (;;) 
    {
//        motorSetSpeed(50, motor_cnt);
    }
}
