/*
 * =====================================================
 * isr.h
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

#ifndef ISR_H_
#define ISR_H_

#include "common.h"

#undef VECTOR_084
#define VECTOR_084  PIT0_ISR



#undef  VECTOR_103                        //取消中断号的定义
#define VECTOR_103    PORTA_IRQHandler    //PORTA中断

#undef  VECTOR_107                        //取消中断号的定义
#define VECTOR_107    PORTE_IRQHandler    //PORTE中断

#undef  VECTOR_016
#define VECTOR_016   DMA0_IRQHandler    

extern void PIT0_ISR(void);


extern void PORTA_IRQHandler();           //PORTA中断服务函数
extern void PORTE_IRQHandler();           //PORTE中断服务函数
extern void DMA0_IRQHandler();

#endif /* ISR_H_ */
