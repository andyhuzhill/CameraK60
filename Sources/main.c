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


#include "derivative.h" /* include peripheral declarations */
#include "img_process.h"
#include "controller.h"

int 
main(void)
{   
    DisableInterrupts;  //��ȫ���ж�
    
    controllerInit();
    
#ifdef DEBUG
    Site_t site={0,0};                            //��ʾͼ�����Ͻ�λ��
    Size_t imgsize={CAMERA_W,CAMERA_H};         //ͼ���С 
    Size_t size={LCD_W,LCD_H};                  //��ʾ����ͼ���С 

    LCD_Init(RED);
#endif
    ov7725_init(img_bin_buff);

    EnableInterrupts;   //��ȫ���ж�
    
    for (;;) 
    {
        ov7725_get_img();                     //�ɼ�ͼ��
#ifdef DEBUG
        LCD_Img_Binary_Z(site,size,(uint16 *)img_bin_buff,imgsize);    //��ʾͼ��
        imgResize(img_bin_buff);
        imgGetMidLine();
#endif
    }
}
