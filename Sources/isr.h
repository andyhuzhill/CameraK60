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

#undef  VECTOR_103                       
#define VECTOR_103    PORTA_ISR    

#undef  VECTOR_016
#define VECTOR_016   DMA0_ISR  

extern void PORTA_ISR();           //场中断服务函数
extern void DMA0_ISR();            //DMA中断服务函数

#endif /* ISR_H_ */
