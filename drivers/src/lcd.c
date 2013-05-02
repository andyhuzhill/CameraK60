
#include "common.h"
#include "lcd.h"
#include "ASCII.h"	
#include "k60_gpio.h"


/*****************************************
 * ��������LCD_Init
 * ����  ��LCD ���� I/O ��ʼ��
 *         LCD FSMC ��ʼ��
 *         LCD ������ HX8347 ��ʼ�� 
 * ����  : ��
 * ���  ����
 * ����  ����
 * ע��  ����
 ******************************************/    
void LCD_Init(uint16 colour)
{
    Site_t site={0,0};
    Size_t size={LCD_W,LCD_H};
    lcd_initialize();   					//��ʼ��LCD

    LCD_Rectangle(site,size,colour);		//��ʼ������
}


/******************************************************
 * ��������LCD_Rectangle
 * ����  ����ָ�����괦��ʾָ������ľ���
 * ����  : -x1      ���������� 0~319
           -y1      ���������� 0~239
		   -len     ���򳤶� 0~319
		   -wid     ������ 0~239
		   -rgb565  �����ɫ
 * ���  ����
 * ����  ��	LCD_Rectangle(10,10,120,160,0);
		    LCD_Rectangle(200,200,120,40,0);
 * ע��  ��	�Ѳ���
 *********************************************************/    
void LCD_Rectangle(Site_t site,Size_t size,uint16 rgb565)
{                    
    uint32 n, temp;

    LCD_PTLON(site,size); 				//����

    temp = (uint32)size.W * size.H;  
    LCD_RAMWR();						//д�ڴ�
    for(n=0; n<temp; n++)
    {
        LCD_WR_16Data( rgb565 );		//д��λ
    } 
}



/******************************************************
 * ��������LCD_Point
 * ����  ����ָ�����괦��ʾһ����
 * ����  : -x ������ʾλ�� 0~319
           -y ������ʾλ�� 0~239
 * ���  ����
 * ����  ��	LCD_Point(100,200);
		   	LCD_Point(10,200);
		   	LCD_Point(300,220);
 * ע��  ��	(0,0)λ��ΪҺ�������Ͻ� �Ѳ���
 *********************************************************/    
void LCD_Point(Site_t site,uint16 rgb565)
{
    Size_t size={1,1};
    LCD_PTLON(site,size);   
    LCD_RAMWR();					//д�ڴ�
    LCD_WR_16Data(rgb565);			//д����
} 

/************************************************************
 * ��������LCD_Char
 * ����  ����Һ������(x,y)��,��ʾһ��8*16��С���ַ�
 * ����  : 	-x 0~(239-6)
 *         	-y 0~(319-12)
 *      	-acsii    Ҫд����ַ�
 *			-Color    �ַ���ɫ
 *			-bkColor  ������ɫ
 * ���  ����
 * ����  ����
 * ע��  ���ַ�����ʾ����Ϊ�� �Ѳ���
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

    LCD_RAMWR();					//д�ڴ�

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
            //����
            site.x = 0;
            site.y += 16;
        }
        if(site.y > MAX_CHAR_POSY ) 
        {	
            //һ��
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

//void LCD_Str_CH(Site_t site,const uint8 *str  , uint16 Color ,uint16 bkColor) 		//��ʾ16*16�����ַ���
//{
//    while(*str != '\0')
//    {
//        if(site.x>(LCD_W-16))
//        {	 
//            /*����*/
//            site.x =0;
//            site.y +=16;
//        }
//        if(site.y >(LCD_H-16))
//        {
//            /*���¹���*/
//            site.y =0;
//            site.x =0;
//        }
//        LCD_Char_CH(site,str,Color,bkColor);      
//        str += 2 ;
//        site.x += 16 ;	
//    }
//}


/************************************************************
 * ��������LCD_Str_R
 * ����  ����Һ������(x,y)��,��ʾһ��16*8��С���ַ���,�Ҷ���
 * ����  : -x 0~(239-6)
 *         -y 0~(319-12)
 *         -str �ַ�����ַ
 		   -len �ַ�������
		   -strColor �ַ���ɫ
		   -bkColor  ������ɫ
 * ���  ����
 * ����  ��LCD_Str_R(304,59,��12345��,14,0x0000,0xffff)
 * ע��  ���ַ�����ʾ����Ϊ�� �Ѳ���
 ******************************************************************/    
void LCD_Str_R(Site_t site,const uint8 *str,uint16 Color,uint16 bkColor)    //�Ҷ��룬����35λ������ʾ���35λ
{
    uint16 i=1,strlen=0;	
    Site_t sitetemp;
    sitetemp.y=site.y;

    while(str[strlen] != 0 )strlen++; 
    while(str[strlen-i])
    {
        sitetemp.x=site.x-8*i;
        if ( LCD_Char(sitetemp,str[strlen-i],Color,bkColor) ==0 ) return;  //��ʾ�����߽磬ֱ�ӷ���
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
    while( res )  /*�õ����ֳ���t*/
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
    while( res )            /*�õ����ֳ���t*/
    {
        res/=10;
        t++;
    }
    if(t >= MAX_NUM_BIT )    //������󳤶�
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
    while( res )            /*�õ����ֳ���t*/
    {
        res/=10;
        t++;
    }
    if(t >= max_num_bit )    //������󳤶�
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

    LCD_PTLON(site,size); 				//����
    temp = (uint32)size.W * size.H;  
    LCD_RAMWR();						//д�ڴ�
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
    LCD_PTLON(site,size); 						//����

    //temp = (uint32)size.W * size.H;  
    LCD_RAMWR();								//д�ڴ�
    for( y=0; y < (size.H) ; y++  )				//�ڿ������������ɨ��
    {
        Y=( (y * imgsize.H + (size.H>>1)-1)/size.H);	//�����img��ĸ߶ȣ�+ size.H>>1����������
        tempY = Y * imgsize.W ;
        for( x=0; x< size.W ; x++  )
        {
            X =( (x * imgsize.W + (size.W>>1)-1)/size.W);
            temp = tempY + X;

            LCD_WR_Data( (uint8 ) ( img [temp]) )     ;	//����ͷ���ȸ�λ���λ
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

    LCD_PTLON(site,size); 						//����

    LCD_RAMWR();								//д�ڴ�

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
                LCD_WR_16Data_BIG( BINARY_BGCOLOR );		//дͼ������
            }
            else
            {
                LCD_WR_16Data_BIG( BINARY_COLOR );		//дͼ������
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

    LCD_PTLON(site,size); 						//����

    LCD_RAMWR();								//д�ڴ�

    while(total--)
    {
        imgtemp     = *(pimg++);
        bitindex    = 8;
        while(bitindex--)
        {
            if( imgtemp & (0x01<<bitindex) )
            {
                LCD_WR_16Data_BIG( BINARY_COLOR );		//дͼ������
            }
            else
            {
                LCD_WR_16Data_BIG(  BINARY_BGCOLOR  );		//дͼ������
            }
            BL14002_DELAY();
        }
    }
}


/********************************************************************
 * ��������LCD_Char_CH
 * ����  ����ʾ���������ַ�
 * ����  : 	x: 0~(319-16)
 *         	y: 0~(239-16)
 *			str: �����ַ�����ַ
 *			Color: �ַ���ɫ   
 *			bkColor: ������ɫ
 * ���  ����
 * ����  ��	LCD_Char_CH(200,100,"��",0,0);
 * ע��	 ������������1�ĺ����ַ�������ʾ����ضϣ�ֻ��ʾ��ǰ��һ������
 ************************************************************************/    
//void LCD_Char_CH(Site_t site,const uint8 *str,uint16 Color,uint16 bkColor)
//{
//
//#ifndef NO_CHNISEST_DISPLAY         /*���������ʾ����û�йر�*/
//    uint8 i,j;
//    uint8 buffer[32];
//    uint16 tmp_char=0;
//    Site_t tmpsite;
//    GetGBKCode_from_sd(buffer,str);  /* ȡ��ģ���� */
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