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

//ȫ�ֱ�������
uint16   motor_cnt =0;
//uint8   img_bin_buff[CAMERA_SIZE];

int 
main(void)
{   
    uint8 h8,l8;
    uint8 duty=0;
    uint8 status;       //�����жϽ���/����״̬
    DisableInterrupts;  //��ȫ���ж�

    NRF_Init();
    controllerInit();
    motorInit();
    //    steerInit();

    EnableInterrupts;   //��ȫ���ж�

    for (;;) 
    {
        FTM_PWM_Duty(MOTOR1_FTM, MOTOR1_CHN, duty);
        h8 = motor_cnt / 256;
        l8 = motor_cnt % 256;
        NRF_ISR_Tx_Dat(&h8, 1);
        NRF_ISR_Tx_Dat(&l8, 1);
        NRF_ISR_Tx_Dat(&duty, 1);
        do
        {
            status = NRF_ISR_Tx_State();
        }while(status == TX_ISR_SEND);
        DELAY_MS(500);
        duty ++;
        if (duty >100)
        { 
            duty = 0;
        }
    }
}
