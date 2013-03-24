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

void PORTA_ISR();           //���жϷ�����
void DMA0_ISR();            //DMA�жϷ�����

#endif /* ISR_H_ */
