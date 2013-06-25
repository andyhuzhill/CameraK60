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
        default :
            break;
        }
    }

    if (PORTA_ISFR & (1 << 9))                     //编码器引脚中断
    {               
        encoder_cnt ++;
        GPIOD_PTOR |= (1 << 10);
    }
    PORTA_ISFR  = ~0;                              //场中断里，全部都要清中断标志位
}

void 
DMA0_ISR(void)
{
    DMA_DIS(CAMERA_DMA_CH);                 //关闭通道CHn 硬件请求
    DMA_IRQ_CLEAN(CAMERA_DMA_CH);           //清除通道传输中断标志位
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