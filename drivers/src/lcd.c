
#include "common.h"
#include "lcd.h"
#include "ASCII.h"	
#include "k60_gpio.h"


/*****************************************
 * 函数名：LCD_Init
 * 描述  ：LCD 控制 I/O 初始化
 *         LCD FSMC 初始化
 *         LCD 控制器 HX8347 初始化 
 * 输入  : 无
 * 输出  ：无
 * 举例  ：无
 * 注意  ：无
 ******************************************/    
void LCD_Init(uint16 colour)
{
    Site_t site={0,0};
    Size_t size={LCD_W,LCD_H};
    lcd_initialize();   					//初始化LCD

    LCD_Rectangle(site,size,colour);		//初始化背景
}


/******************************************************
 * 函数名：LCD_Rectangle
 * 描述  ：在指定坐标处显示指定长宽的矩形
 * 输入  : -x1      横向矩形起点 0~319
           -y1      纵向矩形起点 0~239
		   -len     横向长度 0~319
		   -wid     纵向宽度 0~239
		   -rgb565  填充颜色
 * 输出  ：无
 * 举例  ：	LCD_Rectangle(10,10,120,160,0);
		    LCD_Rectangle(200,200,120,40,0);
 * 注意  ：	已测试
 *********************************************************/    
void LCD_Rectangle(Site_t site,Size_t size,uint16 rgb565)
{                    
    uint32 n, temp;

    LCD_PTLON(site,size); 				//开窗

    temp = (uint32)size.W * size.H;  
    LCD_RAMWR();						//写内存
    for(n=0; n<temp; n++)
    {
        LCD_WR_16Data( rgb565 );		//写低位
    } 
}



/******************************************************
 * 函数名：LCD_Point
 * 描述  ：在指定坐标处显示一个点
 * 输入  : -x 横向显示位置 0~319
           -y 纵向显示位置 0~239
 * 输出  ：无
 * 举例  ：	LCD_Point(100,200);
		   	LCD_Point(10,200);
		   	LCD_Point(300,220);
 * 注意  ：	(0,0)位置为液晶屏左上角 已测试
 *********************************************************/    
void LCD_Point(Site_t site,uint16 rgb565)
{
    Size_t size={1,1};
    LCD_PTLON(site,size);   
    LCD_RAMWR();					//写内存
    LCD_WR_16Data(rgb565);			//写数据
} 

/************************************************************
 * 函数名：LCD_Char
 * 描述  ：在液晶坐标(x,y)中,显示一个8*16大小的字符
 * 输入  : 	-x 0~(239-6)
 *         	-y 0~(319-12)
 *      	-acsii    要写入的字符
 *			-Color    字符颜色
 *			-bkColor  背景颜色
 * 输出  ：无
 * 举例  ：无
 * 注意  ：字符串显示方向为横 已测试
 ******************************************************************/    
uint8 LCD_Char(Site_t site,const uint8 ascii, uint16 Color, uint16 bkColor)
{       
#define MAX_CHAR_POSX (LCD_W-8)
#define MAX_CHAR_POSY (LCD_H-16)

    uint8 temp, t, pos;   
    Size_t size={8,16};

    if(site.x > MAX_CHAR_POSX || site.y > MAX_CHAR_POSY)
    {
        return 0;
    }

    LCD_PTLON(site,size); 

    LCD_RAMWR();					//写内存

    for (pos=0;pos<16;pos++)
    {
        temp=ascii_8x16[((ascii-0x20)*16)+pos];

        for(t=0; t<8; t++)
        {

            if(temp & 0x80)              
            {
                LCD_WR_16Data(Color);
            }
            else 	
            {
                LCD_WR_16Data(bkColor);  
            }            
            temp <<= 1;	
        }
    }
    return 1;
#undef MAX_CHAR_POSX
#undef MAX_CHAR_POSY   
}

void LCD_Str(Site_t site,uint8 *Str, uint16 Color, uint16 bkColor)
{ 
#define MAX_CHAR_POSX (LCD_W-8)
#define MAX_CHAR_POSY (LCD_H-16)    
    while(*Str != '\0')
    {       
        if(site.x > MAX_CHAR_POSX ) 
        {	
            //换行
            site.x = 0;
            site.y += 16;
        }
        if(site.y > MAX_CHAR_POSY ) 
        {	
            //一屏
            site.y = 0;
            site.x = 0;   
        }

        LCD_Char(site, *Str,Color, bkColor);
        site.x += 8;
        Str ++ ;
    }
#undef MAX_CHAR_POSX
#undef MAX_CHAR_POSY   
}

//void LCD_Str_CH(Site_t site,const uint8 *str  , uint16 Color ,uint16 bkColor) 		//显示16*16汉字字符串
//{
//    while(*str != '\0')
//    {
//        if(site.x>(LCD_W-16))
//        {	 
//            /*换行*/
//            site.x =0;
//            site.y +=16;
//        }
//        if(site.y >(LCD_H-16))
//        {
//            /*重新归零*/
//            site.y =0;
//            site.x =0;
//        }
//        LCD_Char_CH(site,str,Color,bkColor);      
//        str += 2 ;
//        site.x += 16 ;	
//    }
//}


/************************************************************
 * 函数名：LCD_Str_R
 * 描述  ：在液晶坐标(x,y)中,显示一个16*8大小的字符串,右对齐
 * 输入  : -x 0~(239-6)
 *         -y 0~(319-12)
 *         -str 字符串首址
 		   -len 字符串长度
		   -strColor 字符颜色
		   -bkColor  背景颜色
 * 输出  ：无
 * 举例  ：LCD_Str_R(304,59,“12345”,14,0x0000,0xffff)
 * 注意  ：字符串显示方向为横 已测试
 ******************************************************************/    
void LCD_Str_R(Site_t site,const uint8 *str,uint16 Color,uint16 bkColor)    //右对齐，超过35位，仅显示最后35位
{
    uint16 i=1,strlen=0;	
    Site_t sitetemp;
    sitetemp.y=site.y;

    while(str[strlen] != 0 )strlen++; 
    while(str[strlen-i])
    {
        sitetemp.x=site.x-8*i;
        if ( LCD_Char(sitetemp,str[strlen-i],Color,bkColor) ==0 ) return;  //显示超出边界，直接返回
        i++;
        if( i>strlen)return;
    }	  
}




void LCD_Num(Site_t site,uint32 num, uint16 Color, uint16 bkColor)	  
{      
    uint32 res=num;   	   
    uint8 t=0;
    Site_t sitetemp;
    sitetemp.y=site.y;

    if( num == 0 )
    {
        LCD_Char(site,'0', Color, bkColor);
        return;
    }	
    while( res )  /*得到数字长度t*/
    {
        res/=10;
        t++;
    }

    while(num)
    {
        sitetemp.x=site.x+(8*(t--)-8);
        LCD_Char(sitetemp,(num%10)+'0', Color, bkColor);
        num /= 10 ;
    }			     
}


void LCD_Num_C(Site_t site,uint32 num, uint16 Color, uint16 bkColor)	  
{      
    uint32 res=num;   	   
    uint8 t=0;
    Site_t sitetemp;
    Size_t size;

    sitetemp.y=site.y;

    if( num == 0 )
    {
        LCD_Char(site,'0', Color, bkColor);

        site.x += 8;
        size.H  = 16;
        size.W  = 8*(MAX_NUM_BIT-1);
        LCD_Rectangle(site,size,bkColor);

        return;
    }	
    while( res )            /*得到数字长度t*/
    {
        res/=10;
        t++;
    }
    if(t >= MAX_NUM_BIT )    //限制最大长度
    {
        t = MAX_NUM_BIT;
    }

    res = t;  

    while( t !=0 )
    {
        sitetemp.x=site.x+(8*(t--)-8);
        LCD_Char(sitetemp,(num%10)+'0', Color, bkColor);
        num /= 10 ;
    }

    if(res != MAX_NUM_BIT )
    {
        size.W = 8*(MAX_NUM_BIT - res);
        site.x += (8*res);
        size.H  = 16;
        LCD_Rectangle(site,size,bkColor);
    }
}

void LCD_Num_BC(Site_t site,uint32 num,uint8 max_num_bit, uint16 Color, uint16 bkColor)	  
{      
    uint32 res=num;   	   
    uint8 t=0;
    Site_t sitetemp;
    Size_t size;

    sitetemp.y=site.y;

    if( num == 0 )
    {
        LCD_Char(site,'0', Color, bkColor);

        site.x += 8;
        size.H  = 16;
        size.W  = 8*(max_num_bit-1);
        LCD_Rectangle(site,size,bkColor);

        return;
    }	
    while( res )            /*得到数字长度t*/
    {
        res/=10;
        t++;
    }
    if(t >= max_num_bit )    //限制最大长度
    {
        t = max_num_bit;
    }

    res = t;  

    while( t !=0 )
    {
        sitetemp.x=site.x+(8*(t--)-8);
        LCD_Char(sitetemp,(num%10)+'0', Color, bkColor);
        num /= 10 ;
    }

    if(res != max_num_bit )
    {
        size.W = 8*(max_num_bit - res);
        site.x += (8*res);
        size.H  = 16;
        LCD_Rectangle(site,size,bkColor);
    }
}

void LCD_Img_RGB565	(Site_t site,Size_t size,uint16 *img)
{
    uint32 n, temp;

    LCD_PTLON(site,size); 				//开窗
    temp = (uint32)size.W * size.H;  
    LCD_RAMWR();						//写内存
    for(n=0; n<temp; n++)
    {
        LCD_WR_16Data_BIG( *img );		
        img++;
    }
}

void LCD_Img_RGB565_Z(Site_t site,Size_t size,uint16 *img,Size_t imgsize)
{

    uint16 x,y;
    uint16 X,Y;
    uint32 temp, tempY;
    LCD_PTLON(site,size); 						//开窗

    //temp = (uint32)size.W * size.H;  
    LCD_RAMWR();								//写内存
    for( y=0; y < (size.H) ; y++  )				//在开窗区域的纵轴扫描
    {
        Y=( (y * imgsize.H + (size.H>>1)-1)/size.H);	//求出在img里的高度，+ size.H>>1是四舍五入
        tempY = Y * imgsize.W ;
        for( x=0; x< size.W ; x++  )
        {
            X =( (x * imgsize.W + (size.W>>1)-1)/size.W);
            temp = tempY + X;

            LCD_WR_Data( (uint8 ) ( img [temp]) )     ;	//摄像头是先高位后低位
            LCD_WR_Data(  (uint8 ) (img  [temp+1]))  ;
        }
    }
}

void LCD_Img_Binary_Z(Site_t site,Size_t size,uint16 *img,Size_t imgsize)
{

    uint32 temp, tempY;
    uint16 x,y;    
    uint16 X,Y; 
    uint8 * pimg = (uint8 *)img;

    LCD_PTLON(site,size); 						//开窗

    LCD_RAMWR();								//写内存

    for(y=0; y < size.H; y++)
    {
        Y = ( (  y * imgsize.H  + (size.H >> 1)) / size.H) ;
        tempY = Y * imgsize.W ;

        for(x=0;x<size.W;x++)
        {
            X = (( x * imgsize.W  + (size.W >> 1)) / size.W) ;
            temp = tempY + X;
            if( (pimg[temp>>3] & (1<<(7- (temp & 0x07))) ) == 0  ) 
            {
                LCD_WR_16Data_BIG( BINARY_BGCOLOR );		//写图像数据
            }
            else
            {
                LCD_WR_16Data_BIG( BINARY_COLOR );		//写图像数据
            }
        }
    }
}



void LCD_Img_Binary(Site_t site,Size_t size,uint16 *img)
{
    uint32     total = (size.H * size.W)/8;
    uint8     imgtemp;
    uint8 	    bitindex;
    uint8  *   pimg=(uint8 *)img;

    LCD_PTLON(site,size); 						//开窗

    LCD_RAMWR();								//写内存

    while(total--)
    {
        imgtemp     = *(pimg++);
        bitindex    = 8;
        while(bitindex--)
        {
            if( imgtemp & (0x01<<bitindex) )
            {
                LCD_WR_16Data_BIG( BINARY_COLOR );		//写图像数据
            }
            else
            {
                LCD_WR_16Data_BIG(  BINARY_BGCOLOR  );		//写图像数据
            }
            BL14002_DELAY();
        }
    }
}


/********************************************************************
 * 函数名：LCD_Char_CH
 * 描述  ：显示单个汉字字符
 * 输入  : 	x: 0~(319-16)
 *         	y: 0~(239-16)
 *			str: 中文字符串首址
 *			Color: 字符颜色   
 *			bkColor: 背景颜色
 * 输出  ：无
 * 举例  ：	LCD_Char_CH(200,100,"好",0,0);
 * 注意	 ：如果输入大于1的汉字字符串，显示将会截断，只显示最前面一个汉字
 ************************************************************************/    
//void LCD_Char_CH(Site_t site,const uint8 *str,uint16 Color,uint16 bkColor)
//{
//
//#ifndef NO_CHNISEST_DISPLAY         /*如果汉字显示功能没有关闭*/
//    uint8 i,j;
//    uint8 buffer[32];
//    uint16 tmp_char=0;
//    Site_t tmpsite;
//    GetGBKCode_from_sd(buffer,str);  /* 取字模数据 */
//    tmpsite.y = site.y;
//
//    for (i=0;i<16;i++)
//    {
//        tmp_char=buffer[i*2];
//        tmp_char=(tmp_char<<8);
//        tmp_char|=buffer[2*i+1];
//
//        tmpsite.x = site.x;
//        for (j=0;j<16;j++)
//        {
//            if ( (tmp_char >> 15-j) & 0x01 == 0x01)
//            {
//
//                LCD_Point(tmpsite,Color);
//            }
//            else
//            {
//                LCD_Point(tmpsite,bkColor);
//            }
//            tmpsite.x++; 
//        }
//        tmpsite.y++;
//    }
//
//#endif
//}