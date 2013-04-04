/*
 * =====================================================
 * isr.c
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

#include "derivative.h"

extern 
volatile IMG_STATE img_flag;

void 
PORTA_ISR(void)         //���жϴ�����
{
    if(PORTA_ISFR & (1 << 29))                       //PTA29�����ж�
    {
        //���ж���Ҫ�ж��ǳ��������ǳ���ʼ
        if(img_flag == IMG_START)                   //��Ҫ��ʼ�ɼ�ͼ��
        {
            img_flag = IMG_GATHER;                  //���ͼ��ɼ���
            disable_irq(PORTA_IRQn);                //�ر�PTA���ж�

            DMA_EN(CAMERA_DMA_CH);                  //ʹ��ͨ��CHn Ӳ������
            DMA_DADDR(CAMERA_DMA_CH) = (uint32)IMG_BUFF;    //�ָ���ַ
        }
        else                                        //ͼ��ɼ�����
        {
            disable_irq(PORTA_IRQn);                //�ر�PTA���ж�
            img_flag = IMG_FAIL;                    //���ͼ��ɼ�ʧ��
        }
        PORTA_ISFR  = ~0;                           //���ж��ȫ����Ҫ���жϱ�־λ
        return;                                     //���жϴ����ˣ��Ͳ���Ҫ�������ж�
    }
    PORTA_ISFR  = ~0;                   //д1���жϱ�־λ
}

void 
DMA0_ISR(void)
{
    DMA_DIS(CAMERA_DMA_CH);             //�ر�ͨ��CHn Ӳ������
    DMA_IRQ_CLEAN(CAMERA_DMA_CH);           //���ͨ�������жϱ�־λ

    img_flag = IMG_FINISH ; 
}


static uint16 encoder_cnt;

void FTM2_ISR(void)
{
    uint8 status = FTM2_STATUS;             //��ȡ������Ƚ�״̬
    
    FTM2_STATUS = 0x00;                     //���жϱ�־
    
    if(status & (1 << ENCODER_CHN))
    {
        FTM_IRQ_DIS(ENCODER_FTM, ENCODER_CHN); //��ֹ���벶���ж�
        
        encoder_cnt ++;
        
        FTM_IRQ_EN(ENCODER_FTM, ENCODER_CHN);  //ʹ�����벶���ж�
    }
}

extern uint16 motor_cnt;
void 
PIT0_ISR(void)
{
    DisableInterrupts;
    
    motor_cnt = encoder_cnt;
    DEBUG_OUT("\n Get %d pluses\n", encoder_cnt);
    encoder_cnt = 0;
    
    PIT_Flag_Clear(PIT0);
    EnableInterrupts;
}


void PORTE_ISR(void)
{
    if (PORTE_ISFR &(1 << 27))
    {
        PORTE_ISFR |= (1 << 27);        //����жϱ�־
        
        NRF_Handler();
    }
}
