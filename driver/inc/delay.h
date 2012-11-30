/* 
 * delay.h
 *
 *  Created on: Nov 27, 2012
 *      Author: scout
 *      E-mail: andyhuzhill@gmail.com
 *    HomePage: http://andyhuzhill.github.com
 *
 * Description: ʹ��SysTick��ʱ����ʱ
 *    Compiler: CodeWarrior 10.2
 *     version: 1.0
 */

#ifndef DELAY_H_
#define DELAY_H_

#include "common.h"


/*
 * @˵��: ��ʼ����ʱ����   ʹ����ʱ����ǰ������ô˺���
 * @����: ϵͳʱ��Ƶ��
 * @����ֵ: ��
 */
void
DELAY_init(uint32 SysClk);

/*
 * @˵��: ��ʱn΢��
 * @����: us ΢��
 * @����ֵ: ��
 */
void
delay_us(uint32 us);

/*
 * @˵��: ��ʱn����
 * @����: ms ����
 * @����ֵ: ��
 */
void
delay_ms(uint32 ms);

#endif /* DELAY_H_ */
