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
 *  ���¿�������ֵ
 *  steerActual  ���ʵ�ʵĿ��������
 *  motorActual  ���ʵ�ʵĿ��������
 *  steerDesired ���Ԥ�ڵĿ��������
 *  motorDesired ���Ԥ�ڵĿ��������
 */
void controllerUpdatePID(
        float steerActual, float motorActual,
        float steerDesired, float motorDesired);

/*
 * ��λ����PID������
 */
void controllerResetPID(void);


/*
 * �õ�������ʵ�ʵ����ֵ
 */
void controllerGetOutput(int16* steer, int16* motor);


/*
 * �����������ʼ��
 */
void steerInit(void);

/*
 * ����ı�ռ�ձ�
 * duty Ҫ�ﵽ��ռ�ձ�
 */
void steerSetDuty(uint8 duty);

/*
 * ������Ƴ�ʼ��
 */
void motorInit(void);

/*
 * �������
 * speed ��Ҫ�ﵽ���ٶ�
 * realspeed ʵ�ʵ��ٶ�
 */
void motorSetSpeed(uint32 speed, uint32 realspeed);

#endif /* CONTROLLER_H_ */
