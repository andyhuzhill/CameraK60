/**
 * Note: This file is recreated by the project wizard whenever the MCU is
 *       changed and should not be edited by hand
 */

/* Include the derivative-specific header file */


/*  
 *   引脚分配:
 *   PTC14          电机控制使能
 *   PTA8           电机控制PWM1    FTM1 CH0
 *   PTA9           电机控制PWM2    FTM1 CH1
 *   PTC1           舵机控制PWM     FTM0 CH0
 *   PTA11          编码器捕获输入  FTM2 CH1
 *   
 *   PTB0~PTB7      摄像头数据端口
 *   PTB8           PCLK
 *   PTA29          场中断
 *   PTB10~PTB11    摄像头控制端口
 *   
 *   PTA14~PTA17    SPI-2.4G 控制端口
 *   PTE27~PTE28    ？？
 *   
 *   PTD0~PTD7      LCD数据端口
 *   PTC19          LCDRST
 *   PTC9~PTC12     LCD控制端口
 *   
 *   PTC18          SD-Check
 *   PTE0~PTE5      SDcard 接口
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
#define STEER_FREQ              300         //单位Hz
#define STEER_DEFAULT_DUTY      50          //单位 百分之一

#define MOTOR1_FTM              FTM1
#define MOTOR1_CHN              CH0
#define MOTOR1_FREQ             10000        //单位Hz
#define MOTOR1_DEFAULT_DUTY     0          //单位 百分之一

#define MOTOR2_FTM              FTM1
#define MOTOR2_CHN              CH1
#define MOTOR2_FREQ             10000        //单位Hz
#define MOTOR2_DEFAULT_DUTY     0            //单位百分之一

#define MOTOR_EN_PORT           PORT_C
#define MOTOR_EN_PIN            14

#define ENCODER_FTM             FTM2
#define ENCODER_CHN             CH1

//  定义舵机PID参数
#define PID_STEER_KP  3.5
#define PID_STEER_KI  2.0
#define PID_STEER_KD  0.0
#define PID_STEER_INTEGRATION_LIMIT     20.0

//  定义电机PID参数
#define PID_MOTOR_KP  1
#define PID_MOTOR_KI  0.0
#define PID_MOTOR_KD  0.0
#define PID_MOTOR_INTEGRATION_LIMIT     20.0


#if  1
#define DUTY2PWM(duty)  (738*(duty)-3535)
#define PWM2DUTY(pwm)   (((pwm)+3535)/738)
#else
#define DUTY2PWM(duty)  (441*(duty)-3546)
#define PWM2DUTY(pwm)   (((pwm)+3546)/441)
#endif 

#endif

