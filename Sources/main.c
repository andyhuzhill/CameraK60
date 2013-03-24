/*
 * =====================================================
 * main.c
 *
 *  Created on: Mar 19, 2013
 *      Author: scout
 *		E-mail: andyhuzhill@gmail.com
 *    Homepage: http://andyhuzhill.github.com
 *	   
 * Description: ������
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
#include "img_process.h"

uint8 img_bin_buff[CAMERA_SIZE];

int 
main(void)
{   
#ifdef DEBUG
    Site_t site={0,0};                            //��ʾͼ�����Ͻ�λ��
    Size_t imgsize={CAMERA_W,CAMERA_H};         //ͼ���С 
    Size_t size={LCD_W,LCD_H};                  //��ʾ����ͼ���С 

    LCD_Init(RED);
#endif
    ov7725_init(img_bin_buff);

    while(1)
    {
        ov7725_get_img();                     //�ɼ�ͼ��
#ifdef DEBUG
        LCD_Img_Binary_Z(site,size,(uint16 *)img_bin_buff,imgsize);    //��ʾͼ��
        imgResize(img_bin_buff);
#endif
    }
}
