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

static vuint32 encoder_cnt;

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
        default :
            break;
        }
    }

    if (PORTA_ISFR & (1 << 9))                     //�����������ж�
    {               
        encoder_cnt ++;
        GPIOD_PTOR |= (1 << 10);
    }
    PORTA_ISFR  = ~0;                              //���ж��ȫ����Ҫ���жϱ�־λ
}

void 
DMA0_ISR(void)
{
    DMA_DIS(CAMERA_DMA_CH);                 //�ر�ͨ��CHn Ӳ������
    DMA_IRQ_CLEAN(CAMERA_DMA_CH);           //���ͨ�������жϱ�־λ
    GPIOD_PTOR |= (1 << 12);
    img_flag = IMG_FINISH ; 
}

vint8 getEncoder = 0;
extern vint32  speed_cnt;
vint32 imgspeed = 0;

void 
PIT0_ISR(void)
{
    DisableInterrupts;
    
    speed_cnt = encoder_cnt;
    getEncoder = 1;
    encoder_cnt = 0;
    imgspeed ++;
    
    printf("\t");
    
    
    GPIOD_PTOR |= (1 << 8);
    PIT_Flag_Clear(PIT0);
    
    EnableInterrupts;
}

#ifdef AT2401

void 
PORTE_ISR(void)
{
	if(PORTE_ISFR & (1 << 27)){
		NRF_Handler();
	}
	
	PORTE_ISFR |= ~0;
}

#endif