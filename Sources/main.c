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
vuint32 speed_cnt=0;      // �������ɼ��������ڵ��ٶ�ֵ
vuint32  encoder_cnt=0;
volatile bool getEncoder= false;


void 
ledInit(void)
{
    GPIO_InitTypeDef initGPIO;

    initGPIO.Pin  = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |
            GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    initGPIO.MODE = Mode_OUT;
    initGPIO.STATE = High;
    initGPIO.IRQC = None_IRQ;

    GPIO_init(GPIO_D, &initGPIO);
}

int 
main(void)
{   
    DisableInterrupts;  //��ȫ���ж�

    LCD_Init(RED);
    ledInit();

    imgInit();      //����ͷ��ʼ��
    motorInit();    //������Ƴ�ʼ��
    steerInit();

    EnableInterrupts;   //��ȫ���ж�

    motorSetSpeed(encoder_cnt, 40);

    for (;;) 
    {
        imgProcess();

        motorSetSpeed(encoder_cnt, 10);
    }
}
