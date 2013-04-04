/*
 * =====================================================
 * main.c
 *
 *  Created on: Mar 19, 2013
 *      Author: scout
 *		E-mail: andyhuzhill@gmail.com
 *    Homepage: http://andyhuzhill.github.com
 *	   
 * Description: 主程序
 *    Revision:
 * =====================================================
 */


#include "derivative.h" /* include peripheral declarations */
#include "img_process.h"
#include "controller.h"

int 
main(void)
{   
    DisableInterrupts;  //关全局中断
    
    controllerInit();
    
#ifdef DEBUG
    Site_t site={0,0};                            //显示图像左上角位置
    Size_t imgsize={CAMERA_W,CAMERA_H};         //图像大小 
    Size_t size={LCD_W,LCD_H};                  //显示区域图像大小 

    LCD_Init(RED);
#endif
    ov7725_init(img_bin_buff);

    EnableInterrupts;   //开全局中断
    
    for (;;) 
    {
        ov7725_get_img();                     //采集图像
#ifdef DEBUG
        LCD_Img_Binary_Z(site,size,(uint16 *)img_bin_buff,imgsize);    //显示图像
        imgResize(img_bin_buff);
        imgGetMidLine();
#endif
    }
}
