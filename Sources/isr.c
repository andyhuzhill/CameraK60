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
PORTA_ISR(void)         //场中断处理函数
{
    if(PORTA_ISFR & (1 << 29))                       //PTA29触发中断
    {
        //场中断需要判断是场结束还是场开始
        if(img_flag == IMG_START)                   //需要开始采集图像
        {
            img_flag = IMG_GATHER;                  //标记图像采集中
            disable_irq(PORTA_IRQn);                //关闭PTA的中断

            DMA_EN(CAMERA_DMA_CH);                  //使能通道CHn 硬件请求
            DMA_DADDR(CAMERA_DMA_CH) = (uint32)IMG_BUFF;    //恢复地址
        }
        else                                        //图像采集错误
        {
            disable_irq(PORTA_IRQn);                //关闭PTA的中断
            img_flag = IMG_FAIL;                    //标记图像采集失败
        }
        PORTA_ISFR  = ~0;                           //场中断里，全部都要清中断标志位
        return;                                     //场中断触发了，就不需要处理行中断
    }
    PORTA_ISFR  = ~0;                   //写1清中断标志位
}

void 
DMA0_ISR(void)
{
    DMA_DIS(CAMERA_DMA_CH);             //关闭通道CHn 硬件请求
    DMA_IRQ_CLEAN(CAMERA_DMA_CH);           //清除通道传输中断标志位

    img_flag = IMG_FINISH ; 
}


static uint16 encoder_cnt;

void FTM2_ISR(void)
{
    uint8 status = FTM2_STATUS;             //读取捕获与比较状态
    
    FTM2_STATUS = 0x00;                     //清中断标志
    
    if(status & (1 << ENCODER_CHN))
    {
        FTM_IRQ_DIS(ENCODER_FTM, ENCODER_CHN); //禁止输入捕获中断
        
        encoder_cnt ++;
        
        FTM_IRQ_EN(ENCODER_FTM, ENCODER_CHN);  //使能输入捕获中断
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
        PORTE_ISFR |= (1 << 27);        //清除中断标志
        
        NRF_Handler();
    }
}
