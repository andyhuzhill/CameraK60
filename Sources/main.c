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
    uint8 txbuf[3]={0};  //���ͻ�����
    uint8 status;       //�����жϽ���/����״̬
    DisableInterrupts;  //��ȫ���ж�

    NRF_Init();
    controllerInit();
    motorInit();
    steerInit();

    EnableInterrupts;   //��ȫ���ж�

    for (;;) 
    {
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
