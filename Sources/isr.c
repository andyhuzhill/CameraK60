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

/*
 * ���жϴ�����
 */

extern 
volatile IMG_STATE img_flag;

void 
PORTA_ISR(void)
{
    if(PORTA_ISFR & (1 << 29))                       //PTA29�����ж�
    {
        //���ж���Ҫ�ж��ǳ��������ǳ���ʼ
        if(img_flag == IMG_START)                   //��Ҫ��ʼ�ɼ�ͼ��
        {
            img_flag = IMG_GATHER;                  //���ͼ��ɼ���
            disable_irq(87);

            DMA_EN(CAMERA_DMA_CH);                  //ʹ��ͨ��CHn Ӳ������
            DMA_DADDR(CAMERA_DMA_CH) = (uint32)IMG_BUFF;    //�ָ���ַ
        }
#ifdef DEBUG
        else if(img_flag == IMG_GATHER)             //ͼ��ɼ��н��볡�жϣ���ͼ��ɼ����
        {
            while(1);                               //DMA�ɼ��쳣
        }
#endif
        else                                        //ͼ��ɼ�����
        {
            disable_irq(87);                        //�ر�PTA���ж�
            img_flag = IMG_FAIL;                    //���ͼ��ɼ�ʧ��
        }
        PORTA_ISFR  = ~0;                           //���ж��ȫ����Ҫ���жϱ�־λ
        return;                                     //���жϴ����ˣ��Ͳ���Ҫ�������ж�
    }
    PORTA_ISFR  = ~0;                   //д1���жϱ�־λ
}

/*
 * DMA�жϴ�����
 */

void 
DMA0_ISR(void)
{
    DMA_DIS(CAMERA_DMA_CH);             //�ر�ͨ��CHn Ӳ������
    DMA_IRQ_CLEAN(CAMERA_DMA_CH);           //���ͨ�������жϱ�־λ

    img_flag = IMG_FINISH ; 
}


void 
PIT0_ISR(void)
{
    DisableInterrupts;
    
    DEBUG_OUT("\n Get %d pluses\n", encoder_cnt);
    encoder_cnt = 0;
    
    PIT_Flag_Clear(PIT0);
    EnableInterrupts;
}