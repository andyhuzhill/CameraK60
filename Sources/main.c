/*
 * =====================================================
 * main.c
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

#include "derivative.h" /* include peripheral declarations */

uint8 img_bin_buff[CAMERA_SIZE];


void delay(void)
{
    int x,y;
    for(x= 100; x>0; --x)
        for(y= 500; y>0; --y);
}

int 
main(void)
{   
    GPIO_InitTypeDef  gpio;
    
    gpio.Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    gpio.MODE = Mode_OUT;
    gpio.IRQC = IRQ_Disable;
    gpio.STATE = State_High;
    
    GPIO_init(PORT_D, &gpio);
    
    
    printf("hello world!\n");
    
    
    pit_init(PIT0, 500);

    while(1)
    {
       
       printf("Hello\n");
    }
}
