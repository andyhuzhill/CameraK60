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

//全局变量定义
volatile uint16 speed_cnt=0;      // 编码器采集到的现在的速度值
vuint16 encoder_cnt=0;
volatile bool getEncoder= false;

extern uint8 *srcImg;

int 
main(void)
{   
    Site_t site={0,0};                          //显示图像左上角位置
    Size_t imgsize={CAMERA_W,CAMERA_H};         //图像大小 
    Size_t size={LCD_W,LCD_H};                  //显示区域图像大小

    DisableInterrupts;  //关全局中断
    
    LCD_Init(RED);

    imgInit();      //摄像头初始化
    //    motorInit();    //电机控制初始化

    EnableInterrupts;   //开全局中断

    for (;;) 
    {

        imgProcess();
        LCD_Img_Binary_Z(site,size,(uint16 *)srcImg,imgsize);    //显示图像
    }
}
