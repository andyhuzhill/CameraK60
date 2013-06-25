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
	Size_t size = {50, 50};
	Size_t imgsize = {80, 60};
	int speed = 0;
	int8 status = 0;
	DisableInterrupts;  //关全局中断

	LCD_Init(RED);
//	ledInit();
//	steerInit();
//	imgInit();      //摄像头初始化
//	motorInit();    //电机控制初始化

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
					LCD_Img_Binary_Z(site, size, (uint16*)img_bin_buff, imgsize);
					printf("img:\n");
					for(int i=0; i< sizeof(img_bin_buff); i++)
						printf("%d,",img_bin_buff[i]);
					printf("\n");
					break;
			default:
				break;
			}
		}
//		speed = imgProcess();
//		motorSetSpeed(speed);
	}
}
