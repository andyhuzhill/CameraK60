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
 *                             野火嵌入式开发工作室
 *
 *  函数名称：PORTE_IRQHandler
 *  功能说明：PORTE端口中断服务函数
 *  参数说明：无
 *  函数返回：无
 *  修改时间：2012-9-17    已测试
 *  备    注：引脚号需要自己初始化来清除
 *************************************************************************/
void PORTE_IRQHandler()
{
    uint8  n;    //引脚号

    n = 27;
    if(PORTE_ISFR & (1 << n))           //PTA26触发中断
    {
        PORTE_ISFR  |= (1 << n);        //写1清中断标志位
        disable_irq(PIT1+ 68);          //关闭PIT1中断，避免影响无线模块收发

        NRF_Handler();                  //无线模块中断处理函数
        enable_irq(PIT1+ 68);          //关闭PIT1中断，避免影响无线模块收发
    }
}

/*************************************************************************
 *                             野火嵌入式开发工作室
 *
 *  函数名称：PORTA_IRQHandler
 *  功能说明：PORTA端口中断服务函数
 *  参数说明：无
 *  函数返回：无
 *  修改时间：2012-1-25    已测试
 *  备    注：引脚号需要自己初始化来清除
 *************************************************************************/


extern 
volatile IMG_STATE img_flag;

void PORTA_IRQHandler()
{
    uint8  n = 0;    //引脚号
    //Site_t site={10,90};
    //   volatile uint16 i;
    /*==============================================================================
    注意：
        场中断放在行中断前面，即优先级高于行中断。
        场中断用户任务里，全部都return，确保不进入行中断的用户任务。

    ==============================================================================*/
    n = 29;                                         //场中断
    if(PORTA_ISFR & (1 << n))                       //PTA29触发中断
    {

        /*  以下为用户任务  */

        //场中断需要判断是场结束还是场开始
        if(img_flag == IMG_START)                   //需要开始采集图像
        {
            img_flag = IMG_GATHER;                  //标记图像采集中
            disable_irq(87);

            DMA_EN(CAMERA_DMA_CH);                  //使能通道CHn 硬件请求

            DMA_DADDR(CAMERA_DMA_CH) = (uint32)IMG_BUFF;    //恢复地址

        }
#ifdef DEBUG
        else if(img_flag == IMG_GATHER)             //图像采集中进入场中断，即图像采集完毕
        {
            while(1);                               //DMA采集异常
        }
#endif
        else                                        //图像采集错误
        {
            disable_irq(87);                        //关闭PTA的中断
            //DMA_IRQ_DIS(CAMERA_DMA_CH);                   //关闭通道CHn 中断请求 
            //LCD_Str(site,"ERROR",BLUE,RED);
            img_flag = IMG_FAIL;                    //标记图像采集失败
        }
        PORTA_ISFR  = ~0;                           //场中断里，全部都要清中断标志位
        return;                                     //场中断触发了，就不需要处理行中断

        /*  以上为用户任务  */
    }


    PORTA_ISFR  = ~0;                   //写1清中断标志位
}


void DMA0_IRQHandler()
{
    //disable_irq(87);                      //场中断开始采集图片时，就关闭了中断，这里就不需要再关闭
    DMA_DIS(CAMERA_DMA_CH);             //关闭通道CHn 硬件请求
    DMA_IRQ_CLEAN(CAMERA_DMA_CH);           //清除通道传输中断标志位

    img_flag = IMG_FINISH ; 
}