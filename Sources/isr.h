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

extern void PIT0_ISR(void);

#endif /* ISR_H_ */
