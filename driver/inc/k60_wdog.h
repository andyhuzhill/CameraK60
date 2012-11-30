/*
 * k60_wdog.h
 *
 *  Created on: Nov 24, 2012
 *      Author: scout
 *      E-mail: andyhuzhill@gmail.com
 *    HomePage: http://andyhuzhill.github.com
 *
 * Description: 看门狗模块
 *    Compiler: CodeWarroir 10.2
 * 	   version: 1.0
 */

#ifndef K60_WDOG_H_
#define K60_WDOG_H_

#include "common.h"

///////////////////////////////////////////////////////////////////////////////
//              函数声明
///////////////////////////////////////////////////////////////////////////////

/*
 * @说明: 初始化看门狗
 * @参数: 无
 * @返回值: 无
 */
void
WDOG_init(void);

/*
 * @说明: 关闭看门狗
 * @参数: 无
 * @返回值: 无
 */
void
WDOG_disable(void);

/*
 * @说明: 启动看门狗
 * @参数: 无
 * @返回值: 无
 */
void
WDOG_enable(void);

/*
 * @说明: 复位看门狗， 喂狗
 * @参数: 无
 * @返回值: 无
 */
void
WDOG_feed(void);

/*
 * @说明: 启动看门狗中断
 * @参数: 无
 * @返回值: 无
 */
void
WDOG_enable_int(void);

/*
 * @说明: 关闭看门狗中断
 * @参数: 无
 * @返回值: 无
 */
void
WDOG_disable_int(void);

/*
 * @说明:解锁 看门狗
 * @参数: 无
 * @返回值: 无
 */
void
WDOG_unlock(void);

#endif /* K60_WDOG_H_ */
