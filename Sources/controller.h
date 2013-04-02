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



#endif /* CONTROLLER_H_ */
