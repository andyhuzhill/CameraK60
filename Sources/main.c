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


uint8 nrf_buff[CAMERA_SIZE + MAX_ONCE_TX_NUM];
uint8 *img_bin_buff = (uint8 *)(((uint8 *)&nrf_buff) +COM_LEN);


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

int 
main(void)
{   
	crtl_e com;
	Site_t site = {0, 0};
	Size_t size = {80, 60};
	Site_t strsite = {0,70};
	int speed = 0;
	int8 status = 0;
	uint8 rxbuff[4]={0};
	int8 i;
	FATFS fs;
	FIL file;
	int res;
	uint32 filesize;

	DisableInterrupts;  //关全局中断

	LCD_Init(GREEN);
	gpio_init(PORT_E, 6, Mode_IN, High);

#ifdef AT2401
	NRF_Init();
#endif

	EnableInterrupts;   //开全局中断
	
	while(1)
	{
		status = NRF_ISR_Rx_Dat(rxbuff);
		
		if(status == RX_DR){
			for(i=0;i<4; i++){
				printf("%d,",rxbuff[i]);
			}
			printf("\n");
		}

	}
	
	for (;;) 
	{
		do{
			status = NRF_MSG_receive(&com, nrf_buff);
		}while( status == NRF_RESULT_RX_NOVALID);

		if (status == NRF_RESULT_RX_VALID){
			switch(com){
			case COM_IMG:
				LCD_Img_Binary(site, size, (uint16*)img_bin_buff);
				LCD_Str(strsite,(uint8 *)"              ", BLUE, GREEN);

				if(!(GPIOE_PDIR & (1 << 6))){
					f_mount(0,&fs);
					res = f_open(&file, "0:/img.img", FA_OPEN_ALWAYS | FA_WRITE | FA_READ);

					filesize = f_size(&file);

					f_lseek(&file, filesize);

					f_printf(&file, "img\n");

					for(int i=0; i< 600; i++){
						f_printf(&file, "%d,", img_bin_buff[i]);
					}
					f_printf(&file, "\n");

					f_close(&file);
					
					LCD_Str(strsite, (uint8 *)"SD write Ok!", BLUE, YELLOW);
				}
				break;
			default:
				break;
			}
		}
	}
}
