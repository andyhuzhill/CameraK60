/*
 * k60_wdog.h
 *
 *  Created on: Nov 24, 2012
 *      Author: scout
 *      E-mail: andyhuzhill@gmail.com
 *    HomePage: http://andyhuzhill.github.com
 *
 * Description: ���Ź�ģ��
 *    Compiler: CodeWarroir 10.2
 * 	   version: 1.0
 */

#ifndef K60_WDOG_H_
#define K60_WDOG_H_

#include "common.h"

///////////////////////////////////////////////////////////////////////////////
//              ��������
///////////////////////////////////////////////////////////////////////////////

/*
 * @˵��: ��ʼ�����Ź�
 * @����: ��
 * @����ֵ: ��
 */
void
WDOG_init(void);

/*
 * @˵��: �رտ��Ź�
 * @����: ��
 * @����ֵ: ��
 */
void
WDOG_disable(void);

/*
 * @˵��: �������Ź�
 * @����: ��
 * @����ֵ: ��
 */
void
WDOG_enable(void);

/*
 * @˵��: ��λ���Ź��� ι��
 * @����: ��
 * @����ֵ: ��
 */
void
WDOG_feed(void);

/*
 * @˵��: �������Ź��ж�
 * @����: ��
 * @����ֵ: ��
 */
void
WDOG_enable_int(void);

/*
 * @˵��: �رտ��Ź��ж�
 * @����: ��
 * @����ֵ: ��
 */
void
WDOG_disable_int(void);

/*
 * @˵��:���� ���Ź�
 * @����: ��
 * @����ֵ: ��
 */
void
WDOG_unlock(void);

#endif /* K60_WDOG_H_ */
