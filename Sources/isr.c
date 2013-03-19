/*
 * =====================================================
 * isr.c
 *
 *  Created on: Mar 19, 2013
 *      Author: scout
 *		  E-mail: andyhuzhill@gmail.com
 *    Homepage: http://andyhuzhill.github.com
 *	   
 * Description: 
 *    Revision:
 * =====================================================
 */

#include "derivative.h"


void PIT0_ISR(void)
{
    GPIOD_PDOR = 0xffffffff;
    DELAY_MS(500);
    GPIOD_PDOR = 0;
    DELAY_MS(500);
    PIT_Flag_Clear(PIT0);
}