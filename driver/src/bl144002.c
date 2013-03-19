//�ϵ�ʱ��CS��WR��RD Ӧ���Ǹߵ�ƽ
//P0 Ϊ������ݣ����ﻹû���÷����Ŷ��
//P1 Ϊ�������ݣ����ﻹû���÷����Ŷ��


#include "common.h"
#include "k60_gpio.h"
#include "bl144002.h"


void lcd_io_init()
{
    GPIO_InitTypeDef GpioInit;

    GpioInit.Pin =( 0 |
             GPIO_Pin_0 |
             GPIO_Pin_1 |
             GPIO_Pin_2 |
             GPIO_Pin_3 |
             GPIO_Pin_4 |
             GPIO_Pin_5 |
             GPIO_Pin_6 | 
             GPIO_Pin_7);
 
    GpioInit.MODE = Mode_OUT;
    GpioInit.STATE = State_Low;
    GpioInit.IRQC = IRQ_Disable;
    GPIO_init(PORT_D, &GpioInit);
    
    GpioInit.Pin =( 0 |
            GPIO_Pin_9 |
            GPIO_Pin_10 |
            GPIO_Pin_11 |
            GPIO_Pin_12 |
            GPIO_Pin_19 );
    GpioInit.MODE = Mode_OUT;
    GpioInit.STATE = State_Low;
    GpioInit.IRQC = IRQ_Disable;

    GPIO_init(PORT_C, &GpioInit);
}


uint8  rd_data(void)
{
    uint8 data;
    WR=1;
    RS=1;
    CS=0;

    RD=0;     		//����һ�������أ�
    BL14002_DELAY();
    PDDR_IN();		//����Ϊ����
    data=PIN;  		//��ȡ��8λ����  
    RD=1;
    CS=1;
    PDDR_OUT();		//�л�Ϊ���

    return  data;
}



/*-----------------------------------------------LSI INITIAL----------------------------------------------------*/
//LCD��ʼ��
void lcd_initialize()
{
    lcd_io_init();

    RST=1;
    DELAY_MS(100);
    RST=0;
    DELAY_MS(100);
    RST=1;
    DELAY_MS(500);		//�ϵ���㹻ʱ��

    LCD_WR_CMD(0x11);   	//Sleep out   �˳�˯��ģʽ
    DELAY_MS(120);    //Delay 120ms
    //------------------------------------ST7735S Frame Rate-----------------------------------------//
    /* Set the frame frequency of the full colors normal mode. */
    LCD_WR_CMD(0xB1);     //In Normal Mode (Full Colors)    ȫ��ģʽ
    LCD_WR_Data(0x05);    //���� RTNA Set 1-line  Period  һ������
    LCD_WR_Data(0x3A);    //���� FPA: Front Porch   
    LCD_WR_Data(0x3A);    //���� BPA: Back Porch 
    //Frame rate=fosc/((RTNA x 2 + 40) x (LINE + FPA + BPA +2))  
    //���� fosc = 850kHz 

    /* Set the frame frequency of the Idle mode.  */
    LCD_WR_CMD(0xB2);     //In Idle Mode (8-colors)
    LCD_WR_Data(0x05);
    LCD_WR_Data(0x3A);
    LCD_WR_Data(0x3A);

    /* Set the frame frequency of the Partial mode/ full colors. */
    LCD_WR_CMD(0xB3);
    LCD_WR_Data(0x05);
    LCD_WR_Data(0x3A);
    LCD_WR_Data(0x3A);
    LCD_WR_Data(0x05);
    LCD_WR_Data(0x3A);
    LCD_WR_Data(0x3A);

    //------------------------------------End ST7735S Frame Rate-----------------------------------------//
    LCD_WR_CMD(0xB4);   //Display Inversion Control  ��ʾ��ת����

    LCD_WR_Data(0x07);  //LCD_WR_Data(0x03); ԭ��Ϊ3����Ϊ7
    // ����λ�Ӹߵ��ͣ��ֱ�Ϊ  full colors normal mode ��Idle mode ��
    // full Colors partial mode  �� �㷴ת �� �з�ת ��1Ϊ�з�ת

    LCD_WR_CMD(0xC0);   //Power Control Setting  ��Դ��������
    LCD_WR_Data(0x28);
    LCD_WR_Data(0x08);
    LCD_WR_Data(0x84);
    LCD_WR_CMD(0xC1);
    LCD_WR_Data(0XC0);
    LCD_WR_CMD(0xC2);
    LCD_WR_Data(0x0C);
    LCD_WR_Data(0x00);
    LCD_WR_CMD(0xC3);
    LCD_WR_Data(0x8C);
    LCD_WR_Data(0x2A);
    LCD_WR_CMD(0xC4);
    LCD_WR_Data(0x8A);
    LCD_WR_Data(0xEE);
    //---------------------------------End ST7735S Power Sequence-------------------------------------//
    LCD_WR_CMD(0xC5); //  VCOM ��ѹ����
    LCD_WR_Data(0x0C);//  -0.725 
    LCD_WR_CMD(0x36); // �ڴ����ݷ��ʿ���  MX �е�ַ˳��, MY �е�ַ˳�� ��MV ���н��� ��ML ��ֱˢ��˳�� ,RGB  RGB-BGA˳��
    LCD_WR_Data(0xC8);//08
    //------------------------------------ST7735S Gamma Sequence-----------------------------------------//
    LCD_WR_CMD(0xE0);
    LCD_WR_Data(0x05);
    LCD_WR_Data(0x1A);
    LCD_WR_Data(0x0C);
    LCD_WR_Data(0x0E);
    LCD_WR_Data(0x3A);
    LCD_WR_Data(0x34);
    LCD_WR_Data(0x2D);
    LCD_WR_Data(0x2F);
    LCD_WR_Data(0x2D);
    LCD_WR_Data(0x2A);
    LCD_WR_Data(0x2F);
    LCD_WR_Data(0x3C);
    LCD_WR_Data(0x00);
    LCD_WR_Data(0x01);
    LCD_WR_Data(0x02);
    LCD_WR_Data(0x10);
    LCD_WR_CMD(0xE1);
    LCD_WR_Data(0x04);
    LCD_WR_Data(0x1B);
    LCD_WR_Data(0x0D);
    LCD_WR_Data(0x0E);
    LCD_WR_Data(0x2D);
    LCD_WR_Data(0x29);
    LCD_WR_Data(0x24);
    LCD_WR_Data(0x29);
    LCD_WR_Data(0x28);
    LCD_WR_Data(0x26);
    LCD_WR_Data(0x31);
    LCD_WR_Data(0x3B);
    LCD_WR_Data(0x00);
    LCD_WR_Data(0x00);
    LCD_WR_Data(0x03);
    LCD_WR_Data(0x12);
    //------------------------------------End ST7735S Gamma Sequence-----------------------------------------//

    LCD_WR_CMD(0x3A); //65k mode
    LCD_WR_Data(0x05);
    LCD_WR_CMD(0x29); //����ʾ Display on
    LCD_WR_CMD(0x2c); //

    LCD_WR_CMD(0x2a);
    LCD_WR_Data(0x00);
    LCD_WR_Data(0x02);	
    LCD_WR_Data(0x00);
    LCD_WR_Data(0x81);

    LCD_WR_CMD(0x2B);
    LCD_WR_Data(0x00);
    LCD_WR_Data(0x03);
    LCD_WR_Data(0x00);
    LCD_WR_Data(0x82);

    DELAY_MS(200);
}

//��������Partial Mode 
void LCD_PTLON(Site_t site,Size_t size)
{	
    LCD_WR_CMD(0x2a);   //Partial Mode On  �ֲ�ģʽ
    LCD_WR_Data((uint8)(site.x>>8));  //��8λ Sx
    LCD_WR_Data((uint8)site.x);	  //��8λ Sx
    LCD_WR_Data((uint8)((site.x+size.W-1)>>8)); 
    LCD_WR_Data((uint8)(site.x+size.W-1));

    LCD_WR_CMD(0x2B);//Row Address Set  �е�ַ����
    LCD_WR_Data((uint8)(site.y>>8));
    LCD_WR_Data((uint8)site.y);
    LCD_WR_Data((uint8)((site.y+size.H-1)>>8));
    LCD_WR_Data((uint8)(site.y+size.H-1));
}




