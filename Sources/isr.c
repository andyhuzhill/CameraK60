/**
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

extern vuint16 encoder_cnt;

void 
PORTA_ISR(void)         //���жϴ�����
{
    if(PORTA_ISFR & (1 << 29))                       //PTA29�����ж�
    {
        switch(img_flag)
        {
        case IMG_START:
            img_flag = IMG_GATHER;                  //���ͼ��ɼ���

            DMA_EN(CAMERA_DMA_CH);                  //ʹ��ͨ��CHn Ӳ������
            DMA_DADDR(CAMERA_DMA_CH) = (uint32)IMG_BUFF;    //�ָ���ַ
            break;
        case IMG_GATHER:
            break;
        case IMG_READY:
            break;
        case IMG_FAIL:
            break;
        case IMG_STOP:
            break;
        default :
            break;
        }
    }

    if (PORTA_ISFR & (1 << 11))                     //�����������ж�
    {               
        encoder_cnt ++;
    }
    PORTA_ISFR  = ~0;                              //���ж��ȫ����Ҫ���жϱ�־λ
}

void 
DMA0_ISR(void)
{
    disable_irq(PORTA_IRQn);
    DMA_DIS(CAMERA_DMA_CH);                 //�ر�ͨ��CHn Ӳ������
    DMA_IRQ_CLEAN(CAMERA_DMA_CH);           //���ͨ�������жϱ�־λ
    img_flag = IMG_FINISH ; 
//    DEBUG_OUT("************************IMG is FINISHED*********************",0);
}

extern volatile bool getEncoder;
extern vint32  speed_cnt;
void 
PIT0_ISR(void)
{
    DisableInterrupts;
    speed_cnt = encoder_cnt;

    getEncoder = true;
    encoder_cnt = 0;

    PIT_Flag_Clear(PIT0);
    EnableInterrupts;
}

