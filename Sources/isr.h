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

#undef  VECTOR_103                       
#define VECTOR_103    PORTA_ISR    

#undef  VECTOR_016
#define VECTOR_016    DMA0_ISR  

#undef  VECTOR_084 
#define VECTOR_084    PIT0_ISR


void PORTA_ISR(void);           
    //场中断服务函数
void DMA0_ISR(void);            
    //DMA中断服务函数
void PIT0_ISR(void);            
    //PIT0 中断服务函数

#endif /* ISR_H_ */
