/**
 * Note: This file is recreated by the project wizard whenever the MCU is
 *       changed and should not be edited by hand
 */

/* Include the derivative-specific header file */


/**  
 *   引脚分配:
 *
 * Motor ： EN--5V
 * PWM1--PTC1 FTM0-CH0
 * PWM2--PTC2 FTM0-CH1
 *
 * S-D5 : PTA10--FTM2-CH0
 *
 * encoder : PTA9--FTM1--CH1
 *           PTA8--FTM1--CH0
 *
 * Camera : SDA--PTB11 HREF--PTA28(行中断) D0~~~D7
 *          SCL--PTB10 VSYN--PTA29(场中断) PTB0~~~~PTB7
 *          PCLK--PTB8
 *
 * UART3 : RX-PTC16 TX-PTC17
 *
 * NRF-2401: SPI_PTA14-PCS0 PTA15-SCK PTA16-SOUT PTA17-SIN
 *
 * 三轴加速度：SPI2_ PTB20-PCS0 PTB21-SCK PTB22-SOUT PTB23-SIN
 *
 * 拨码开关：PTD12,PTD13
 *
 * 光电管 ：PTC11
 * 
 */

#ifndef __DERIVATIVE_H_
#define __DERIVATIVE_H_

#include "common.h"
#include "dma.h"
#include "ov7725.h"
#include "k60_gpio.h"
#include "ftm.h"
#include "pit.h"
#include "port.h"

#include "ff.h"
#include "nrf24l0.h"
#include "nrf24l0_msg.h"
#include "lcd.h"

#include "controller.h"
#include "img_process.h"

#define ABS(x)   ((x)>=0 ? (x): (-x))
#define MAX(x,y) ((x)>(y) ? (x) : (y))
#define MIN(x,y) ((x)>(y) ? (y) : (x))

#define STEER_MID 				(5100)
#define STEER_MAX  				(6400)
#define STEER_MIN  				(3800)

#define STEER_FTM               FTM2
#define STEER_CHN               CH0
#define STEER_FREQ              300         //单位Hz
#define STEER_DEFAULT_DUTY      STEER_MID          //单位 万分之一

#define MOTOR1_FTM              FTM0
#define MOTOR1_CHN              CH1
#define MOTOR1_FREQ             12000        //单位Hz
#define MOTOR1_DEFAULT_DUTY     FTM_PRECISON        //单位 万分之一

#define MOTOR2_FTM              FTM0
#define MOTOR2_CHN              CH0
#define MOTOR2_FREQ             12000        //单位Hz
#define MOTOR2_DEFAULT_DUTY     FTM_PRECISON            //单位万分之一

#define ENCODER_FTM             FTM1
#define ENCODER_PIN             PTA8

//  定义舵机PID参数
#define PID_STEER_KP  20
#define PID_STEER_KI  0.0
#define PID_STEER_KD  300.0
#define PID_STEER_INTEGRATION_LIMIT     20.0

//  定义电机PID参数
#define PID_MOTOR_KP  30.0	
#define PID_MOTOR_KI  0.0
#define PID_MOTOR_KD  1000.0
#define PID_MOTOR_INTEGRATION_LIMIT     10.0

//#define  SDCARD			//SD卡调试

//#define  SERIAL			//串口调试

//#define SENDIMG			//无线图像发送

#ifdef SENDIMG
	#ifndef AT2401
		#define  AT2401
	#endif 
#endif


//#define SPEED				//无线速度发送

#ifdef SPEED
#ifndef AT2401
#define AT2401
#endif
#endif

#define  CLOSE_LOOP		//定义是否闭环调速

typedef enum{
	LOWEST  = 0,
	MID     = 1,
	FASTER  = 2,
	FASTEST = 3,
}speedChoice;

extern int8 maxspeed ;
extern int8 minspeed ;

#endif

