/**
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


//uint8 nrf_buff[CAMERA_SIZE + MAX_ONCE_TX_NUM];
//uint8 *img_bin_buff = (uint8 *)(((uint8 *)&nrf_buff) +COM_LEN);

extern  uint8 nrf_buff[CAMERA_SIZE + MAX_ONCE_TX_NUM];
extern  uint8 *srcImg;  

void
ledInit(void)
{
	GPIO_InitTypeDef initGPIO;

	initGPIO.Pin  = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |
			GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	initGPIO.MODE = Mode_OUT;
	initGPIO.STATE = High;
	initGPIO.IRQC = None_IRQ;

	GPIO_init(PORT_D, &initGPIO);
}

extern IMG_STATE img_flag;
int 
main(void)
{   
	crtl_e com;
	Site_t site = {0, 0};
	Size_t size = {80, 60};
	Site_t strsite = {0,70};
	int speed = 0;
	int8 status = 0;
	FATFS fs;
	FIL file;
	int res;
	uint32 filesize;

	DisableInterrupts;  //关全局中断

	LCD_Init(YELLOW);
	gpio_init(PORT_E, 6, Mode_IN, High);

#ifdef AT2401
	NRF_Init();
#endif

	EnableInterrupts;   //开全局中断

	for (;;) 
	{
		do{
			status = NRF_MSG_receive(&com, nrf_buff);
		}while( status == NRF_RESULT_RX_NOVALID);

		if (status == NRF_RESULT_RX_VALID){
			switch(com){
			case COM_IMG:
				LCD_Img_Binary(site, size, (uint16*)srcImg);
				
//				outimg(img_bin_buff);
				
				img_flag = IMG_FINISH;
				imgProcess();
				
				printf("\nimg\n");
				for(int i = 0; i<600; ++i){
					printf("%d,", srcImg[i]);
				}
				
				break;
			default:
				break;
			}
		}
	}
}
