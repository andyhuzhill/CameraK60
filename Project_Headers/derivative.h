/**
 * Note: This file is recreated by the project wizard whenever the MCU is
 *       changed and should not be edited by hand
 */

/* Include the derivative-specific header file */


/**  
 *   ���ŷ���:
 *
 * Motor �� EN--5V
 * PWM1--PTC1 FTM0-CH0
 * PWM2--PTC2 FTM0-CH1
 *
 * S-D5 : PTC3--FTM0-CH2
 *
 * encoder : PTA9--FTM1--CH1
 *           PTA8--FTM1--CH0
 *
 * Camera : SDA--PTB11 HREF--PTA28(���ж�) D0~~~D7
 *          SCL--PTB10 VSYN--PTA29(���ж�) PTB0~~~~PTB7
 *          PCLK--PTB8
 *
 * UART3 : RX-PTC16 TX-PTC17
 *
 * NRF-2401: SPI_PTA14-PCS0 PTA15-SCK PTA16-SOUT PTA17-SIN
 *
 * ������ٶȣ�SPI2_ PTB20-PCS0 PTB21-SCK PTB22-SOUT PTB23-SIN
 *
 * ���뿪�أ�PTD12,PTD13
 *
 * ���� ��PTC11
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

#define STEER_FTM               FTM0
#define STEER_CHN               CH2
#define STEER_FREQ              300         //��λHz
#define STEER_DEFAULT_DUTY      FTM_PRECISON /2          //��λ ǧ��֮һ

#define MOTOR1_FTM              FTM0
#define MOTOR1_CHN              CH1
#define MOTOR1_FREQ             300        //��λHz
#define MOTOR1_DEFAULT_DUTY     1000          //��λ ǧ��֮һ

#define MOTOR2_FTM              FTM0
#define MOTOR2_CHN              CH0
#define MOTOR2_FREQ             300        //��λHz
#define MOTOR2_DEFAULT_DUTY     800            //��λǧ��֮һ

#define ENCODER_FTM             FTM1
#define ENCODER_CHN             CH1

//  ������PID����
#define PID_STEER_KP  20
#define PID_STEER_KI  0.0
#define PID_STEER_KD  30.0
#define PID_STEER_INTEGRATION_LIMIT     20.0

//  ������PID����
#define PID_MOTOR_KP  0.1
#define PID_MOTOR_KI  0.0
#define PID_MOTOR_KD  40.0
#define PID_MOTOR_INTEGRATION_LIMIT     20.0

#define STEER_MAX  590
#define STEER_MIN  410

#define MAX_SPEED 10
#define MIN_SPEED 5

//#define  SDCARD			//SD������

//#define  SERIAL
//#define  SERIALIMG
#define  AT2401

#define  CLOSE_LOOP		//�ջ�����

#endif

