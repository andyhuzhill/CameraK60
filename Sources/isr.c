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
PORTA_ISR(void)         //场中断处理函数
{
    if(PORTA_ISFR & (1 << 29))                       //PTA29触发中断
    {
        switch(img_flag)
        {
        case IMG_START:
            img_flag = IMG_GATHER;                  //标记图像采集中

            DMA_EN(CAMERA_DMA_CH);                  //使能通道CHn 硬件请求
            DMA_DADDR(CAMERA_DMA_CH) = (uint32)IMG_BUFF;    //恢复地址
            break;
        case IMG_GATHER:                            //如果进入这个状态说明DMA未结束就进入了场中断
            img_flag = IMG_FAIL;
            DMA_DIS(CAMERA_DMA_CH);                 //关闭通道CHn DMA请求
            break;
        default :
            break;
        }
    }

    if (PORTA_ISFR & (1 << 11))                     //编码器引脚中断
    {               
        encoder_cnt ++;
    }
    PORTA_ISFR  = ~0;                              //场中断里，全部都要清中断标志位
}

void 
DMA0_ISR(void)
{
    DMA_DIS(CAMERA_DMA_CH);                 //关闭通道CHn 硬件请求
    DMA_IRQ_CLEAN(CAMERA_DMA_CH);           //清除通道传输中断标志位
    img_flag = IMG_FINISH ; 
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
    
    GPIOD_PTOR = (1 << 10);

    PIT_Flag_Clear(PIT0);
    EnableInterrupts;
}

void
PORTE_ISR(void)
{
    if (PORTE_ISFR & (1 << 27))
    {
        PORTE_ISFR |= (1 << 27);
        
        NRF_Handler();
    }
}