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
        //���ж���Ҫ�ж��ǳ��������ǳ���ʼ
        if(img_flag == IMG_START)                   //��Ҫ��ʼ�ɼ�ͼ��
        {
            img_flag = IMG_GATHER;                  //���ͼ��ɼ���
            disable_irq(PORTA_IRQn);                //�ر�PTA���ж�
            
            DEBUG_OUT("++++++++++++++++++++++++++++++Start IMG+++++++++++++",0);

            DMA_EN(CAMERA_DMA_CH);                  //ʹ��ͨ��CHn Ӳ������
            DMA_DADDR(CAMERA_DMA_CH) = (uint32)IMG_BUFF;    //�ָ���ַ
        }
        else if(img_flag == IMG_GATHER)
        {
            DEBUG_OUT("IMG is under gathering",0);
        }else //ͼ��ɼ�����
        {
            disable_irq(PORTA_IRQn);                //�ر�PTA���ж�
            img_flag = IMG_FAIL;                    //���ͼ��ɼ�ʧ��
        }
        PORTA_ISFR  = ~0;                           //���ж��ȫ����Ҫ���жϱ�־λ
//        return;                                     //���жϴ����ˣ��Ͳ���Ҫ�������ж�
    }else if (PORTA_ISFR & (1 << 11))                //�����������ж�
    {               
        encoder_cnt ++;
        PORTA_ISFR = ~0;
        return ;
    }
    PORTA_ISFR  = ~0;                   //д1���жϱ�־λ
}

void 
DMA0_ISR(void)
{
    DMA_DIS(CAMERA_DMA_CH);                 //�ر�ͨ��CHn Ӳ������
    DMA_IRQ_CLEAN(CAMERA_DMA_CH);           //���ͨ�������жϱ�־λ
    img_flag = IMG_FINISH ; 
    DEBUG_OUT("************************IMG is FINISHED*********************",0);
}


extern vuint16 encoder_cnt;
void 
FTM2_ISR(void)
{
    uint8 status = FTM2_STATUS;             //��ȡ������Ƚ�״̬

    FTM2_STATUS = 0x00;                     //���жϱ�־

    if(status & (1 << ENCODER_CHN))
    {
        FTM_IRQ_DIS(ENCODER_FTM, ENCODER_CHN); //��ֹ���벶���ж�

        encoder_cnt ++;
        DEBUG_OUT("==================encoder_cnt ++========================",0);
        DEBUG_OUT("==================encoder_cnt ++========================",0);
        FTM_IRQ_EN(ENCODER_FTM, ENCODER_CHN);  //ʹ�����벶���ж�
    }
}


extern volatile bool getEncoder;
extern vint32  speed_cnt;
void 
PIT1_ISR(void)
{
    DisableInterrupts;
    speed_cnt = encoder_cnt;

    getEncoder = true;
    encoder_cnt = 0;
    
    DEBUG_OUT("#######################Get into PIT ISR#####################",0);
    DEBUG_OUT("#######################Get into PIT ISR#####################",0);

    PIT_Flag_Clear(PIT1);
    EnableInterrupts;
}


//void PORTE_ISR(void)
//{
//    DisableInterrupts;
//    if (PORTE_ISFR &(1 << 27))
//    {
//        PORTE_ISFR |= (1 << 27);        //����жϱ�־
//        
//        NRF_Handler();
//    }
//    EnableInterrupts;
//}
