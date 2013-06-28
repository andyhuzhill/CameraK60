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
  float desired;      //< ����Ҫ�ﵽ��ֵ
  float error;        //< ���
  float prevError;    //< ��һ�ε����
  float integ;        //< ���ֺ�
  float deriv;        //< ΢��
  float kp;           //< ����ϵ��
  float ki;           //< ����ϵ��
  float kd;           //< ΢��ϵ��
  float iLimit;       //< ������
} PidObject;

float UpdatePID(PidObject *pid, const float measured);

void pidInit(PidObject *pid, const float desired, const float kp,
        const float ki, const float kd);

void pidSetKp(PidObject *pid, const float kp);

void pidSetKi(PidObject *pid, const float ki);

void pidSetKd(PidObject *pid, const float kd);
/*
 * �����������ʼ��
 */
void steerInit(void);

int32
steerUpdate(int8 error);

/*
 * ����ı�ռ�ձ�
 * duty Ҫ�ﵽ��ռ�ձ�
 */
void steerSetDuty(int32 duty);

/*
 * ������Ƴ�ʼ��
 */
void motorInit(void);

/*
 * �������
 * speed ��Ҫ�ﵽ���ٶ�
 * realspeed ʵ�ʵ��ٶ�
 */
void 
motorSetSpeed(int32 speed);

/**
 * ɲ��
 */
void
stopcar(void);

#endif /* CONTROLLER_H_ */
