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

#include "MK60DZ10.h"

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


/*ARM Cortex M4 implementation for interrupt priority shift*/
#define ARM_INTERRUPT_LEVEL_BITS          4

/***********************************************************************/
// function prototypes for arm_cm4.c
void stop (void);
void wait (void);
void write_vtor (int);
void enable_irq (int);
void disable_irq (int);
void set_irq_priority (int, int);

/***********************************************************************/
  /*!< Macro to enable all interrupts. */
#define EnableInterrupts asm(" CPSIE i");

  /*!< Macro to disable all interrupts. */
#define DisableInterrupts asm(" CPSID i");
/***********************************************************************/


// define for printf
#define TERM_PORT UART3_BASE_PTR
#define TERMINAL_BAUD 115200

#include "io.h"
#include "stdlib.h"
#include "stdio.h"

/*
 * Misc. Defines
 */
#ifdef  FALSE
#undef  FALSE
#endif
#define FALSE   (0)

#ifdef  TRUE
#undef  TRUE
#endif
#define TRUE    (1)

#ifdef  NULL
#undef  NULL
#endif
#define NULL    (0)

#ifdef  ON
#undef  ON
#endif
#define ON      (1)

#ifdef  OFF
#undef  OFF
#endif
#define OFF     (0)

#define ERROR (0)
#define SUCCESS (1)

/***********************************************************************/
/*
 * The basic data types
 */
typedef unsigned char       uint8;  /*  8 bits */
typedef unsigned short int  uint16; /* 16 bits */
typedef unsigned long int   uint32; /* 32 bits */

typedef char                int8;   /*  8 bits */
typedef short int           int16;  /* 16 bits */
typedef int                 int32;  /* 32 bits */

typedef volatile int8       vint8;  /*  8 bits */
typedef volatile int16      vint16; /* 16 bits */
typedef volatile int32      vint32; /* 32 bits */

typedef volatile uint8      vuint8;  /*  8 bits */
typedef volatile uint16     vuint16; /* 16 bits */
typedef volatile uint32     vuint32; /* 32 bits */


/*
 * 方便寄存器位运算的宏定义
 */

//置位寄存器reg的bit位
#define BSET(reg, bit) ((reg) |= (1 << bit))
//复位寄存器reg的bit位
#define BCLR(reg, bit) ((reg) &= ~(1 << bit))
//读取寄存器reg bit位上的值
#define BGET(reg, bit) ((reg) >> (bit) &1)



//延时函数
#include "lptmr.h"
#define DELAY() 		time_delay_ms(500)
#define DELAY_MS(ms) 	time_delay_ms(ms)

/*
 * 断言
 */
#include "assert.h"


#endif /* COMMON_H_ */
