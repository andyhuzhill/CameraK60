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
uint16   motor_cnt =0;
//uint8   img_bin_buff[CAMERA_SIZE];

void delay(void)
{
    volatile int x,y,z;
    for (z = 0; z < 20; ++z) 
    {
        for (x = 0; x < 1500; ++x) 
        {
            for (y = 0; y < 1500; ++y) 
            {
                ;
            }
        } 
    }
}

int 
main(void)
{   
    uint8 h8,l8;
    uint8 duty=0;
    uint8 txbuf[3]={0};  //发送缓存区
    uint8 status;       //用于判断接受/发送状态
    DisableInterrupts;  //关全局中断
    uint8 h8,l8;
    uint8 duty=0;
    uint8 txbuf[3]={0};  //发送缓存区
    uint8 status;       //用于判断接受/发送状态
    DisableInterrupts;  //关全局中断

    NRF_Init();
    controllerInit();
    motorInit();
    steerInit();


    NRF_Init();
    controllerInit();
    motorInit();
    steerInit();


    EnableInterrupts;   //开全局中断

    for (;;) 
    {

        ov7725_get_img();                     //采集图像

        h8 = motor_cnt / 256;
        l8 = motor_cnt % 256;

        txbuf[2] = l8;
        txbuf[1] = h8;
        txbuf[0] = duty;

        NRF_ISR_Tx_Dat(txbuf, 3);
        do
        {
            status = NRF_ISR_Tx_State();
        }while(status == TX_ISR_SEND);
        FTM_PWM_Duty(MOTOR1_FTM, MOTOR1_CHN, duty);

    }
}
