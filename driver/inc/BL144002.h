#ifndef	_BL14002_H_
#define _BL14002_H_

#define	LCD_H	132		//高
#define LCD_W	132		//宽

#define	WR		PTC9_OUT
#define	RD		PTC10_OUT
#define	CS		PTC11_OUT
#define	RS		PTC12_OUT
#define	RST	PTC19_OUT

#define P0			PTD_BYTE0_OUT
#define	PIN			PTD_BYTE0_IN
#define PDDR_OUT()	(DDRD_BYTE0 = 0xff)
#define PDDR_IN()	(DDRD_BYTE0 = 0)

#define	BL14002_DELAY()		//do{asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");asm("nop");}while(0)

typedef	struct
{
    uint16 x;
    uint16 y;
}Site_t;

typedef	struct
{
    uint16 W;		//宽
    uint16 H;		//高
}Size_t;

#define LCD_RAMWR()		LCD_WR_CMD(0x2C)				        //设置 Memory Write 
#define LCD_RAMRD()     do{LCD_WR_CMD(0x2E);rd_data();}while(0) //设置 Memory Read 

uint8  	rd_data(void);
void 	lcd_initialize();
void 	LCD_PTLON(Site_t site,Size_t size);		//开窗

/*******************下面为写命令，写数据*********************/
/*****由于函数较短，执行次数频繁，所以写成宏定义，加快速度*****/
#define LCD_WR_CMD(command_data_8)		do\
        {\
    RD=1;\
    BL14002_DELAY();\
    RS=0;\
    BL14002_DELAY();\
    CS=0;\
    BL14002_DELAY();\
    P0=(uint8)(command_data_8);\
    WR=0;\
    BL14002_DELAY();\
    WR=1;\
    BL14002_DELAY();\
    CS=1;\
        }while(0)	//wr=0;wr=1;产生一个上升沿

#define LCD_WR_Data(low_8_data)		do\
        {\
    RD=1;\
    RS=1;\
    CS=0;\
    BL14002_DELAY();\
    P0=(uint8)(low_8_data);\
    WR=0;\
    WR=1;\
    CS=1;\
        }while(0) 	//wr=0;在这里写入数据到RAM 

#define LCD_WR_16Data(DATA)        LCD_WR_Data((uint8)((DATA)>>8) );LCD_WR_Data( (uint8)(DATA) ) //写16位数据

#define LCD_WR_16Data_BIG(DATA)    LCD_WR_Data((uint8)(DATA) );LCD_WR_Data( (uint8)((DATA)>>8) ) //写16位数据，大端

#endif	//_BL14002_H_