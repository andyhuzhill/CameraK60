/*
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
#include "img_process.h"
#include "controller.h"

//ȫ�ֱ�������
uint16  motor_cnt =0;
uint8   img_bin_buff[CAMERA_SIZE];

int 
main(void)
{   
    DisableInterrupts;  //��ȫ���ж�
    
    controllerInit();
    motorInit();
//    steerInit();
    
    EnableInterrupts;   //��ȫ���ж�
    
    for (;;) 
    {
//        motorSetSpeed(50, motor_cnt);
    }
}
