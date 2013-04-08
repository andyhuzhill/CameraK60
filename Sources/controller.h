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

/*
 *  ���¿�������ֵ
 *  steerActual  ���ʵ�ʵĿ��������
 *  steerDesired ���Ԥ�ڵĿ��������
 */
void steerUpdatePID(float steerActual, float steerDesired);


void motorUpdataPID(float motorActual, float motorDesired);

/*
 * ��λ����PID������
 */
void controllerResetPID(void);

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
void motorSetSpeed(uint32 realspeed, uint32 speed);

#endif /* CONTROLLER_H_ */
