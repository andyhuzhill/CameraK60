#ifndef	_LCD_H_
#define	_LCD_H_

#include "common.h"
#include "bl144002.h"
//#include "lcd_ch.h"


/******常用颜色*****/
#define RED	    0XF800    //红色
#define GREEN   0X07E0    //绿色
#define BLUE    0X001F    //蓝色
#define BRED    0XF81F
#define GRED    0XFFE0    //灰色
#define GBLUE   0X07FF    //
#define BLACK   0X0000    //黑色
#define WHITE   0XFFFF    //白色
#define YELLOW  0xFFE0    //黄色


#define FONT_COLOUR     BLUE    //定义字体颜色
#define BCOLOUR         RED     //定义背景颜色

//确保x的范围为 min~max
#define Range(x,max,min)				((uint8)((x)<(min) ? (min) : ( (x)>(max) ? (max):(x) )))			
		
//提取RGB的3元素，rgb565必须是16位
#define RGB565_R(rgb565)				((uint8)(((rgb565)>>11) &0x1F))								
#define RGB565_G(rgb565)				((uint8)(((rgb565)>> 5) &0x3F))
#define RGB565_B(rgb565)				((uint8)( (rgb565)      &0x1F))


#define GRAY_2_RGB565(gray)				((uint16)((((uint8)(gray)>>3)<<11)&(((uint8)(gray)>>2)<<5)&((uint8)(gray)>>3)))

#define RGB565_2_GRAY(rgb565)   		((uint8)(((RGB565_R(rgb565)*235+RGB565_G(rgb565)*613+RGB565_B(rgb565)*625)+1)>>8))	//	31*235+63*613+31*625+1  = 255*256


#define RGB565_H(rgb565)        		((uint8)(((uint16)(rgb565))>>8))
#define RGB565_L(rgb565)        		(uint8)(rgb565))



void LCD_Init(uint16 colour);

/***************  LCD绘画  ***************/
//默认数字和字母是 8*16大小，汉字是16*16大小
void LCD_Point		(Site_t site                            ,uint16 rgb565);   	//画点
void LCD_Rectangle  (Site_t site,Size_t size             ,uint16 rgb565);   	    //画矩形
uint8   LCD_Char       (Site_t site,const uint8 ascii , uint16 Color ,uint16 bkColor);  	//显示8*16字符
void LCD_Char_CH    (Site_t site,const uint8 *str  , uint16 Color ,uint16 bkColor);      //显示16*16汉字字符
void LCD_Str        (Site_t site,uint8 *Str        , uint16 Color ,uint16 bkColor);  	//显示8*16字符串
void LCD_Str_CH		(Site_t site,const uint8 *str  , uint16 Color ,uint16 bkColor); 		//显示16*16汉字字符串
void LCD_Str_R      (Site_t site,const uint8 *str  , uint16 Color ,uint16 bkColor);  	//显示字符串（右对齐）



void LCD_Num   (Site_t site,uint32 num        , uint16 Color ,uint16 bkColor);  	//显示数字
#define MAX_NUM_BIT 5                                                       	//数字的最大位数（用于清掉多余的屏幕残留数字）
void LCD_Num_C (Site_t site,uint32 num        , uint16 Color ,uint16 bkColor);  	//显示数字(按照 MAX_NUM_BIT 来清除多余的屏幕残留数字，如果传入数字位数大于设置的最大位，只显示高位，所以要设置好哦)
void LCD_Num_BC(Site_t site,uint32 num,uint8 max_num_bit, uint16 Color, uint16 bkColor);//同上，不同的是可传递最大显示位数

void LCD_Img_RGB565		(Site_t site,Size_t size,uint16 *img);				//图像显示
void LCD_Img_RGB565_Z	(Site_t site,Size_t size,uint16 *img,Size_t imgsize);//图像显示，带zoom缩放功能


#define	BINARY_BGCOLOR	WHITE		//定义二值化图像背景颜色
#define	BINARY_COLOR	BLACK		//定义二值化图像前景颜色

void LCD_Img_Binary     (Site_t site,Size_t size,uint16 *img);                   //显示二值化图像
void LCD_Img_Binary_Z   (Site_t site,Size_t size,uint16 *img,Size_t imgsize); //显示二值化图像(可缩放)

#endif