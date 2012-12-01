/*
 * common.h
 *
 *  Created on: Nov 21, 2012
 *      Author: scout
 *      E-mail: andyhuzhill@gmail.com
 *    HomePage: http://andyhuzhill.github.com
 *
 * Description: definitions used for all the file
 *    Compiler: CodeWarrior v10.2
 * 	   version: 1.0
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <MK60DZ10.h>

//存储器段的宏定义
#if defined(__CWCC__)
#pragma define_section relocate_code ".relocate_code" ".relocate_code" ".relocate_code" far_abs RX
#pragma define_section relocate_data ".relocate_data" ".relocate_data" ".relocate_data" RW
#pragma define_section relocate_const ".relocate_const" ".relocate_const" ".relocate_const" far_abs R
#define __relocate_code__   __declspec(relocate_code)
#define __relocate_data__   __declspec(relocate_data)
#define __relocate_const__  __declspec(relocate_const)
#elif defined(__IAR_SYSTEMS_ICC__)
#define __relocate_code__ __ramfunc
#else
#error Not supported compiler type
#endif


/*
 *  用于中断的宏
 */

#define ARM_INTERRUPT_LEVEL_BITS 	4 //中断优先级
#define EINT	asm(" CPSIE i")       //开总中断
#define DINT    asm(" CPSID i")       //关总中断


//
#define TRUE 1
#define FALSE 0
#define NULL 0

/*
 * 类型定义
 */

typedef unsigned char uint8;
typedef unsigned short int  uint16;
typedef unsigned long int uint32;

typedef volatile uint8 vuint8;
typedef volatile uint16 vuint16;
typedef volatile uint32 vuint32;

typedef signed char int8;
typedef short int int16;
typedef long int int32;

typedef volatile int8 vint8;
typedef volatile int16 vint16;
typedef volatile int32 vint32;


/*
 * 方便寄存器位运算的宏定义
 */

//置位寄存器reg的bit位
#define BSET(reg, bit) ((reg) |= (1 << bit))
//复位寄存器reg的bit位
#define BCLR(reg, bit) ((reg) &= ~(1 << bit))
//读取寄存器reg bit位上的值
#define BGET(reg, bit) ((reg) >> (bit) &1)

/*
 * @说明: 设置CPU进入STOP模式
 * @参数: 无
 * @返回值:无
 */
void 
stop(void);

/*
 * @说明: 设置CPU进入WAIT模式
 * @参数: 无
 * @返回值:无
 */
void 
wait(void);

/*
 * @说明: 更改中断向量表偏移寄存器的值 
 * @参数:   vector : 要更改的值
 * @返回值:	无
 */
void 
write_vectab(uint16 vector);

/*
 * @说明: 使能irq中断
 * @参数:   irq : 中断号
 * @返回值: 无
 */
void 
enable_irq(uint16 irq);

/*
 * @说明: 禁止irq中断
 * @参数: irq : 中断号
 * @返回值: 无
 */
void 
disable_irq(uint16 irq);

/*
 * @说明: 设置irq中断和优先级
 * @参数: irq :  中断号
 * 		  prio:  优先级
 * @返回值: 无
 */
void 
set_irq_priority(uint16 irq, uint16 prio);


//延时函数
#include "delay.h"
#define DELAY() 		time_delay_ms(500)
#define DELAY_MS(ms) 	time_delay_ms(ms)

/*
 * 断言
 */
#define USE_ASSERT 1

#if defined(USE_ASSERT)
void assert_failed(uint8 *file, uint32 line);
#define assert(expr) ((expr) ? (void)0 :assert_failed((uint8 *)__FILE__, __LINE__))
#else
#define assert(expr) ((void)0)
#endif


#endif /* COMMON_H_ */
