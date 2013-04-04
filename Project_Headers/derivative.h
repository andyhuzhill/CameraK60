/*
 * Note: This file is recreated by the project wizard whenever the MCU is
 *       changed and should not be edited by hand
 */

/* Include the derivative-specific header file */


/*  
 *   ���ŷ���:
 *   PTC14          �������ʹ��
 *   PTA8           �������PWM1    FTM1 CH0
 *   PTA9           �������PWM2    FTM1 CH1
 *   PTC1           �������PWM     FTM0 CH0
 *   PTA11          ��������������  FTM2 CH1
 *   
 *   PTB0~PTB7      ����ͷ���ݶ˿�
 *   PTB8           PCLK
 *   PTA29          ���ж�
 *   PTB10~PTB11    ����ͷ���ƶ˿�
 *   
 *   PTA14~PTA17    SPI-2.4G ���ƶ˿�
 *   PTE27~PTE28    ����
 *   
 *   PTD0~PTD7      LCD���ݶ˿�
 *   PTC19          LCDRST
 *   PTC9~PTC12     LCD���ƶ˿�
 *   
 *   PTC18          SD-Check
 *   PTE0~PTE5      SDcard �ӿ�
 * 
 */

#ifndef __DERIVATIVE_H_
#define __DERIVATIVE_H_

#include "common.h"
#include "dma.h"
#include "ov7725.h"
#include "k60_gpio.h"
#include "ftm.h"
#include "nrf24l0.h"

#include "lcd.h"
#include "pit.h"

#include "controller.h"
#include "img_process.h"

#define ABS(x)   ((x)>0 ? (x): (-x))
#define MAX(x,y) ((x)>(y) ? (x) : (y))
#define MIN(x,y) ((x)>(y) ? (y) : (x))

#define STEER_FTM               FTM0
#define STEER_CHN               CH0
#define STEER_FREQ              300         //��λHz
#define STEER_DEFAULT_DUTY      50

#define MOTOR1_FTM              FTM1
#define MOTOR1_CHN              CH0
#define MOTOR1_FREQ             10000        //��λHz
#define MOTOR1_DEFAULT_DUTY     10

#define MOTOR2_FTM              FTM1
#define MOTOR2_CHN              CH1
#define MOTOR2_FREQ             10000        //��λHz
#define MOTOR2_DEFAULT_DUTY     0

#define MOTOR_EN_PORT           PORT_C
#define MOTOR_EN_PIN            14

#define ENCODER_FTM             FTM2
#define ENCODER_CHN             CH1

#endif

