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


void PIT0_ISR(void)
{
    GPIOD_PTOR = 0xffffffff;
    PIT_Flag_Clear(PIT0);
}



/*************************************************************************
 *                             Ұ��Ƕ��ʽ����������
 *
 *  �������ƣ�PORTE_IRQHandler
 *  ����˵����PORTE�˿��жϷ�����
 *  ����˵������
 *  �������أ���
 *  �޸�ʱ�䣺2012-9-17    �Ѳ���
 *  ��    ע�����ź���Ҫ�Լ���ʼ�������
 *************************************************************************/
void PORTE_IRQHandler()
{
    uint8  n;    //���ź�

    n = 27;
    if(PORTE_ISFR & (1 << n))           //PTA26�����ж�
    {
        PORTE_ISFR  |= (1 << n);        //д1���жϱ�־λ
        disable_irq(PIT1+ 68);          //�ر�PIT1�жϣ�����Ӱ������ģ���շ�

        NRF_Handler();                  //����ģ���жϴ�����
        enable_irq(PIT1+ 68);          //�ر�PIT1�жϣ�����Ӱ������ģ���շ�
    }
}

/*************************************************************************
 *                             Ұ��Ƕ��ʽ����������
 *
 *  �������ƣ�PORTA_IRQHandler
 *  ����˵����PORTA�˿��жϷ�����
 *  ����˵������
 *  �������أ���
 *  �޸�ʱ�䣺2012-1-25    �Ѳ���
 *  ��    ע�����ź���Ҫ�Լ���ʼ�������
 *************************************************************************/


extern 
volatile IMG_STATE img_flag;

void PORTA_IRQHandler()
{
    uint8  n = 0;    //���ź�
    //Site_t site={10,90};
    //   volatile uint16 i;
    /*==============================================================================
    ע�⣺
        ���жϷ������ж�ǰ�棬�����ȼ��������жϡ�
        ���ж��û������ȫ����return��ȷ�����������жϵ��û�����

    ==============================================================================*/
    n = 29;                                         //���ж�
    if(PORTA_ISFR & (1 << n))                       //PTA29�����ж�
    {

        /*  ����Ϊ�û�����  */

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
            //DMA_IRQ_DIS(CAMERA_DMA_CH);                   //�ر�ͨ��CHn �ж����� 
            //LCD_Str(site,"ERROR",BLUE,RED);
            img_flag = IMG_FAIL;                    //���ͼ��ɼ�ʧ��
        }
        PORTA_ISFR  = ~0;                           //���ж��ȫ����Ҫ���жϱ�־λ
        return;                                     //���жϴ����ˣ��Ͳ���Ҫ�������ж�

        /*  ����Ϊ�û�����  */
    }


    PORTA_ISFR  = ~0;                   //д1���жϱ�־λ
}


void DMA0_IRQHandler()
{
    //disable_irq(87);                      //���жϿ�ʼ�ɼ�ͼƬʱ���͹ر����жϣ�����Ͳ���Ҫ�ٹر�
    DMA_DIS(CAMERA_DMA_CH);             //�ر�ͨ��CHn Ӳ������
    DMA_IRQ_CLEAN(CAMERA_DMA_CH);           //���ͨ�������жϱ�־λ

    img_flag = IMG_FINISH ; 
}