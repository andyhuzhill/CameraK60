#include "common.h"
#include "LCD_CH.h"

FATFS myfs;                    // Work area (file system object) for logical drive
FIL myfsrc;            // file objects
FRESULT myres;                 // FatFs function common result code
UINT mybr;               // File R/W count

/*******************************************************************************
* Function Name  : GetGBKCode_from_sd
* Description    : 从SD卡字库中读取自摸数据到指定的缓冲区 
* Input          : pBuffer---数据保存地址  
*				   					c--汉字字符低字节码 
* Output         : None
* Return         : 0(success)  -1(fail)
* Attention		 	 : None
*******************************************************************************/ 
int GetGBKCode_from_sd(unsigned char* pBuffer,const unsigned char * c)
{ 
    unsigned char High8bit,Low8bit;
    unsigned int pos;
    High8bit=*c;     /* 取高8位数据 */
    Low8bit=*(c+1);  /* 取低8位数据 */
    
    pos = ((High8bit-0xb0)*94+Low8bit-0xa1)*2*16 ;	
    f_mount(0, &myfs);
    myres = f_open(&myfsrc , "0:/HZLIB.bin", FA_OPEN_EXISTING | FA_READ);
    
    if ( myres == FR_OK ) 
    {
        f_lseek (&myfsrc, pos);														 //指针偏移
        myres = f_read( &myfsrc, pBuffer, 32, &mybr );		 //16*16大小的汉字 其字模 占用16*2个字节
        
        f_close(&myfsrc);
        
        return 0;  
    }
    else
    {
        return -1;    
    }
}


