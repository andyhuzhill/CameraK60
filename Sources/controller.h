/*
 * =================================================================================
 * controller.h
 *
 *  Created on: Apr 2, 2013
 *      Author: scout
 *		E-mail: andyhuzhill@gmail.com
 *    Homepage: http://andyhuzhill.github.com
 *	   
 * Description: 
 *    Revision:
*/

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "common.h"
#include "derivative.h"

void controllerInit(void);

/*
 *  更新控制器的值
 *  steerActual  舵机实际的控制输出量
 *  motorActual  电机实际的控制输出量
 *  steerDesired 舵机预期的控制输出量
 *  motorDesired 电机预期的控制输出量
 */
void controllerUpdatePID(
        float steerActual, float motorActual,
        float steerDesired, float motorDesired);

/*
 * 复位所有PID控制器
 */
void controllerResetPID(void);


/*
 * 得到控制器实际的输出值
 */
void controllerGetOutput(int16* steer, int16* motor);


/*
 * 舵机控制器初始化
 */
void steerInit(void);

/*
 * 舵机改变占空比
 * duty 要达到的占空比
 */
void steerSetDuty(uint8 duty);

/*
 * 电机控制初始化
 */
void motorInit(void);

/*
 * 电机调速
 * speed 想要达到的速度
 * realspeed 实际的速度
 */
void motorSetSpeed(uint32 speed, uint32 realspeed);

#endif /* CONTROLLER_H_ */
