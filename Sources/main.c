/**
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

//ȫ�ֱ�������
volatile uint16 speed_cnt=0;      // �������ɼ��������ڵ��ٶ�ֵ
vuint16 encoder_cnt=0;
volatile bool getEncoder= false;

extern uint8 *srcImg;

int 
main(void)
{   
//
//    Site_t site={0,0};                          //��ʾͼ�����Ͻ�λ��
//    Size_t imgsize={CAMERA_W,CAMERA_H};         //ͼ���С 
//    Size_t size={LCD_W,LCD_H};                  //��ʾ����ͼ���С
//    
    DisableInterrupts;  //��ȫ���ж�

    imgInit();      //����ͷ��ʼ��
    //    motorInit();    //������Ƴ�ʼ��

    EnableInterrupts;   //��ȫ���ж�

    for (;;) 
    {
        //        decoderSet();
        //        motorSetSpeed(speed_cnt, DUTY2PWM(20));
        imgProcess();
//        LCD_Img_Binary_Z(site,size,(u16 *)srcImg,imgsize);    //��ʾͼ��
    }
}
