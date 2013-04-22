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

#undef  VECTOR_085 
#define VECTOR_085    PIT1_ISR

#undef  VECTOR_080
#define VECTOR_080    FTM2_ISR         

//#undef  VECTOR_107     
//#define VECTOR_107    PORTE_ISR

void PORTA_ISR(void);           
    //���жϷ�����
void DMA0_ISR(void);            
    //DMA�жϷ�����
void PIT1_ISR(void);            
    //PIT1 �жϷ�����
void FTM2_ISR(void);            
    //�����������ж�
//void PORTE_ISR(void);           
    //2401���߷����жϷ�����

#endif /* ISR_H_ */
