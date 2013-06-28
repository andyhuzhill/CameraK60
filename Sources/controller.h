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

#include "derivative.h"

typedef struct
{
  float desired;      //< 设置要达到的值
  float error;        //< 误差
  float prevError;    //< 上一次的误差
  float integ;        //< 积分和
  float deriv;        //< 微分
  float kp;           //< 比例系数
  float ki;           //< 积分系数
  float kd;           //< 微分系数
  float iLimit;       //< 积分限
} PidObject;

float UpdatePID(PidObject *pid, const float measured);

void pidInit(PidObject *pid, const float desired, const float kp,
        const float ki, const float kd);

void pidSetKp(PidObject *pid, const float kp);

void pidSetKi(PidObject *pid, const float ki);

void pidSetKd(PidObject *pid, const float kd);
/*
 * 舵机控制器初始化
 */
void steerInit(void);

int32
steerUpdate(int8 error);

/*
 * 舵机改变占空比
 * duty 要达到的占空比
 */
void steerSetDuty(int32 duty);

/*
 * 电机控制初始化
 */
void motorInit(void);

/*
 * 电机调速
 * speed 想要达到的速度
 * realspeed 实际的速度
 */
void 
motorSetSpeed(int32 speed);

/**
 * 刹车
 */
void
stopcar(void);

#endif /* CONTROLLER_H_ */
