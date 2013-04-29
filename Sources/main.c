/**
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

/*  
 *   ���ŷ���:
 *   PTC14          �������ʹ��
 *   PTA8           �������PWM1    FTM1 CH0
 *   PTA9           �������PWM2    FTM1 CH1
 *   PTC1           �������PWM     FTM0 CH0
 *   PTA11          ��������������  FTM2 CH1
 *   
 *   PTB0~PTB7      ����ͷ���ݶ˿�
 *   PTB8           PCLK
 *   PTA29          ���ж�
 *   PTB10~PTB11    ����ͷ���ƶ˿�
 *   
 *   PTA14~PTA17    SPI-2.4G ���ƶ˿�
 *   PTE27~PTE28    ����
 *   
 *   PTD0~PTD7      LCD���ݶ˿�
 *   PTC19          LCDRST
 *   PTC9~PTC12     LCD���ƶ˿�
 *   
 *   PTC18          SD-Check
 *   PTE0~PTE5      SDcard �ӿ�
 * 
 */

#include "derivative.h" /* include peripheral declarations */

//ȫ�ֱ�������
volatile uint16 speed_cnt=0;      // �������ɼ��������ڵ��ٶ�ֵ
vuint16 encoder_cnt=0;
volatile bool getEncoder= false;

extern uint8 *srcImg;

int 
main(void)
{   

    uint8 duty=0;
    uint8 txbuf[3]={0};  //���ͻ�����
    uint8 status;       //�����жϽ���/����״̬

    Site_t site={0,0};                          //��ʾͼ�����Ͻ�λ��
    Size_t imgsize={CAMERA_W,CAMERA_H};         //ͼ���С 
    Size_t size={LCD_W,LCD_H};                  //��ʾ����ͼ���С

    DisableInterrupts;  //��ȫ���ж�
    
    LCD_Init(RED);

    imgInit();      //����ͷ��ʼ��
    //    motorInit();    //������Ƴ�ʼ��

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

        //        decoderSet();
        //        motorSetSpeed(speed_cnt, DUTY2PWM(20));
        imgProcess();
        LCD_Img_Binary_Z(site,size,(uint16 *)srcImg,imgsize);    //��ʾͼ��
    }
}
