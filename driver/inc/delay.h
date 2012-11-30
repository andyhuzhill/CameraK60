/* 
 * delay.h
 *
 *  Created on: Nov 27, 2012
 *      Author: scout
 *      E-mail: andyhuzhill@gmail.com
 *    HomePage: http://andyhuzhill.github.com
 *
 * Description: 使用SysTick定时器延时
 *    Compiler: CodeWarrior 10.2
 *     version: 1.0
 */

#ifndef DELAY_H_
#define DELAY_H_

#include "common.h"


/*
 * @说明: 初始化延时函数   使用延时函数前必须调用此函数
 * @参数: 系统时钟频率
 * @返回值: 无
 */
void
DELAY_init(uint32 SysClk);

/*
 * @说明: 延时n微秒
 * @参数: us 微秒
 * @返回值: 无
 */
void
delay_us(uint32 us);

/*
 * @说明: 延时n毫秒
 * @参数: ms 毫秒
 * @返回值: 无
 */
void
delay_ms(uint32 ms);

#endif /* DELAY_H_ */
