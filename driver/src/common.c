/*
 * common.c
 *
 *  Created on: Nov 21, 2012
 *      Author: scout
 *      E-mail: andyhuzhill@gmail.com
 *    HomePage: http://andyhuzhill.github.com
 *
 * Description: common functions
 *    Compiler: CodeWarrior v10.2 
 * 	   version: 1.0
 */

#include "common.h"

/*
 * @说明: 设置CPU进入STOP模式
 * @参数: 无
 * @返回值:无
 */
void 
stop(void)
{
	//设置 SLEEPDEEP位使能STOP模式
	SCB_SCR |= SCB_SCR_SLEEPDEEP_MASK;
	//WFI指令进入STOP
	asm("WFI");
	
}

/*
 * @说明: 设置CPU进入WAIT模式
 * @参数: 无
 * @返回值:无
 */
void 
wait(void)
{
	SCB_SCR &= ~SCB_SCR_SLEEPDEEP_MASK;
	asm("WFI");
}

/*
 * @说明: 更改中断向量表偏移寄存器的值 
 * @参数:   vector : 要更改的值
 * @返回值:	无
 */
void 
write_vectab(uint16 vector)
{
	SCB_VTOR = vector;
}

/*
 * @说明: 使能irq中断
 * @参数:   irq : 中断号
 * @返回值: 无
 */

void 
enable_irq(uint16 irq)
{
	int div;
	
	//确保irq是有效的
	if (irq > 91) irq = 91;
	
	//确定对应NVICISER
	div = irq/32;
	
	switch(div)
	{
	case 0x0:
		NVICICPR0 = 1 << (irq%32);
		NVICISER0 = 1 << (irq%32);
		break;
	case 0x1:
		NVICICPR1 = 1 << (irq%32);
		NVICISER1 = 1 << (irq%32);
		break;
	case 0x2:
		NVICICPR2 = 1 << (irq%32);
		NVICISER2 = 1 << (irq%32);
	default:
		break;
	}
}

/*
 * @说明: 禁止irq中断
 * @参数: irq : 中断号
 * @返回值: 无
 */
void 
disable_irq(uint16 irq)
{
	uint16 div;
	
	//确定irq为有效irq
	if (irq > 91) irq = 91;
	
	//确定对应的NVICISER
	div = irq/32;
	
    switch (div)
    {
    	case 0x0:
               NVICICER0 |= 1 << (irq%32);
              break;
    	case 0x1:
              NVICICER1 |= 1 << (irq%32);
              break;
    	case 0x2:
              NVICICER2 |= 1 << (irq%32);
              break;
    }          
}

/*
 * @说明: 设置irq中断和优先级
 * @参数: irq :  中断号
 * 		  prio:  优先级
 * @返回值: 无
 */
void 
set_irq_priority(uint16 irq, uint16 prio)
{
   uint8 *prio_reg;
   
   //确保irq和优先级有效
   if (irq > 91) irq = 91;

   if (prio > 15) irq = 15;

   //确定对应的NVICISER
   prio_reg = (uint8 *)(((uint32)&NVICIP0) + irq);
   //设置优先级
   *prio_reg = ( (prio&0xF) << (8 - ARM_INTERRUPT_LEVEL_BITS) );             
}


#if defined(USE_ASSERT)
const char ASSERT_FAILED_STR[]= "Assertion failed in %s at line %d\n";
void
assert_failed(uint8 *file, uint32 line)
{
	printf(ASSERT_FAILED_STR, file, line);
	
	while(1);
}
#endif
