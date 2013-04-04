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

//全局变量定义
uint16   motor_cnt =0;
uint8   img_bin_buff[CAMERA_SIZE];

int 
main(void)
{   
    uint8 h8,l8;
    uint8 status;       //用于判断接受/发送状态
    DisableInterrupts;  //关全局中断
    
    NRF_Init();
    controllerInit();
    motorInit();
//    steerInit();
    
    EnableInterrupts;   //开全局中断
    
    for (;;) 
    {
        h8 = motor_cnt / 256;
        l8 = motor_cnt % 256;
        NRF_ISR_Tx_Dat(&h8, 1);
        NRF_ISR_Tx_Dat(&l8, 1);
        do
        {
            status = NRF_ISR_Tx_State();
        }while(status == TX_ISR_SEND);
//        motorSetSpeed(50, motor_cnt);
    }
}
