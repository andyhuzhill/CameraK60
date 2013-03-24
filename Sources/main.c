/*
 * =====================================================
 * main.c
 *
 *  Created on: Mar 19, 2013
 *      Author: scout
 *		E-mail: andyhuzhill@gmail.com
 *    Homepage: http://andyhuzhill.github.com
 *	   
 * Description: 
 *    Revision:
 * =====================================================
*/

/*  
 *   引脚分配:
 *   PTC14          电机控制使能
 *   PTA8           电机控制PWM1    FTM1 CH0
 *   PTA9           电机控制PWM2    FTM1 CH1
 *   PTC1           舵机控制PWM     FTM0 CH0
 *   PTA11          编码器捕获输入  FTM2 CH1
 *   
 *   PTB0~PTB7      摄像头数据端口
 *   PTB8           PCLK
 *   PTA29          场中断
 *   PTB10~PTB11    摄像头控制端口
 *   
 *   PTA14~PTA17    SPI-2.4G 控制端口
 *   PTE27~PTE28    ？？
 *   
 *   PTD0~PTD7      LCD数据端口
 *   PTC19          LCDRST
 *   PTC9~PTC12     LCD控制端口
 *   
 *   PTC18          SD-Check
 *   PTE0~PTE5      SDcard 接口
 * 
 */

#include "derivative.h" /* include peripheral declarations */


uint8 img_bin_buff[CAMERA_SIZE];

#include "img_process.h"

int 
main(void)
{   
    LCD_Init(RED);
    ov7725_init(img_bin_buff);

    while(1)
    {
        ov7725_get_img();
        imgResize(img_bin_buff);
    }
}
