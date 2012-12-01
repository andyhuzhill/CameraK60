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

//�洢���εĺ궨��
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
 *  �����жϵĺ�
 */

#define ARM_INTERRUPT_LEVEL_BITS 	4 //�ж����ȼ�
#define EINT	asm(" CPSIE i")       //�����ж�
#define DINT    asm(" CPSID i")       //�����ж�


//
#define TRUE 1
#define FALSE 0
#define NULL 0

/*
 * ���Ͷ���
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
 * ����Ĵ���λ����ĺ궨��
 */

//��λ�Ĵ���reg��bitλ
#define BSET(reg, bit) ((reg) |= (1 << bit))
//��λ�Ĵ���reg��bitλ
#define BCLR(reg, bit) ((reg) &= ~(1 << bit))
//��ȡ�Ĵ���reg bitλ�ϵ�ֵ
#define BGET(reg, bit) ((reg) >> (bit) &1)

/*
 * @˵��: ����CPU����STOPģʽ
 * @����: ��
 * @����ֵ:��
 */
void 
stop(void);

/*
 * @˵��: ����CPU����WAITģʽ
 * @����: ��
 * @����ֵ:��
 */
void 
wait(void);

/*
 * @˵��: �����ж�������ƫ�ƼĴ�����ֵ 
 * @����:   vector : Ҫ���ĵ�ֵ
 * @����ֵ:	��
 */
void 
write_vectab(uint16 vector);

/*
 * @˵��: ʹ��irq�ж�
 * @����:   irq : �жϺ�
 * @����ֵ: ��
 */
void 
enable_irq(uint16 irq);

/*
 * @˵��: ��ֹirq�ж�
 * @����: irq : �жϺ�
 * @����ֵ: ��
 */
void 
disable_irq(uint16 irq);

/*
 * @˵��: ����irq�жϺ����ȼ�
 * @����: irq :  �жϺ�
 * 		  prio:  ���ȼ�
 * @����ֵ: ��
 */
void 
set_irq_priority(uint16 irq, uint16 prio);


//��ʱ����
#include "delay.h"
#define DELAY() 		time_delay_ms(500)
#define DELAY_MS(ms) 	time_delay_ms(ms)

/*
 * ����
 */
#define USE_ASSERT 1

#if defined(USE_ASSERT)
void assert_failed(uint8 *file, uint32 line);
#define assert(expr) ((expr) ? (void)0 :assert_failed((uint8 *)__FILE__, __LINE__))
#else
#define assert(expr) ((void)0)
#endif


#endif /* COMMON_H_ */
