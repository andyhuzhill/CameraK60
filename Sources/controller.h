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

typedef enum
{
    ROAD_LOST           = 0,
    STRATIGHT           = 1,
    ROUND               = 2,
    STRATIGHT_TO_ROUND  = 3,
    BIG_S               = 4,
    SMALL_S             = 5,
    CROSS               = 6
}Road_type;

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

float UpdataPID(PidObject *pid, const float measured);

void pidInit(PidObject *pid, const float desired, const float kp,
        const float ki, const float kd);

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
void motorSetSpeed(uint32 realspeed, uint32 speed);

#endif /* CONTROLLER_H_ */
