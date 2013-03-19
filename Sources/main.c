/*
 * =====================================================
 * main.c
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

/*
 *   ���ŷ���:
 *   PTC14          �������ʹ��
 *   PTA8           �������PWM1    FTM1 CH0
 *   PTA9           �������PWM2    FTM1 CH1
 *   PTC1           �������PWM     FTM0 CH0
 *   PTA11          ��������������  FTM2 CH1
 *   
 *   PTB0~PTB7      ����ͷ���ݶ˿�
 *   PTB8           PCLK
 *   PTA29          ���ж�
 *   PTB10~PTB11    ����ͷ���ƶ˿�
 *   
 *   PTA14~PTA17    SPI-2.4G ���ƶ˿�
 *   PTE27~PTE28    ����
 *   
 *   PTD0~PTD7      LCD���ݶ˿�
 *   PTC19          LCDRST
 *   PTC9~PTC12     LCD���ƶ˿�
 *   
 *   PTC18          SD-Check
 *   PTE0~PTE5      SDcard �ӿ�
 * 
 */

#include "derivative.h" /* include peripheral declarations */


uint8 img_bin_buff[CAMERA_SIZE];




int 
main(void)
{   
    GPIO_InitTypeDef  gpio;
    
    gpio.Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    gpio.MODE = Mode_OUT;
    gpio.IRQC = None_IRQ;
    gpio.STATE = High;
    
    GPIO_init(PORT_D, &gpio);
    
    
    
    printf("hello world!\n");
    
    pit_init(PIT0, 500);

    while(1)
    {
       
    }
}
