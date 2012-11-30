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
 * @˵��: ����CPU����STOPģʽ
 * @����: ��
 * @����ֵ:��
 */
void 
stop(void)
{
	//���� SLEEPDEEPλʹ��STOPģʽ
	SCB_SCR |= SCB_SCR_SLEEPDEEP_MASK;
	//WFIָ�����STOP
	asm("WFI");
	
}

/*
 * @˵��: ����CPU����WAITģʽ
 * @����: ��
 * @����ֵ:��
 */
void 
wait(void)
{
	SCB_SCR &= ~SCB_SCR_SLEEPDEEP_MASK;
	asm("WFI");
}

/*
 * @˵��: �����ж�������ƫ�ƼĴ�����ֵ 
 * @����:   vector : Ҫ���ĵ�ֵ
 * @����ֵ:	��
 */
void 
write_vectab(uint16 vector)
{
	SCB_VTOR = vector;
}

/*
 * @˵��: ʹ��irq�ж�
 * @����:   irq : �жϺ�
 * @����ֵ: ��
 */

void 
enable_irq(uint16 irq)
{
	int div;
	
	//ȷ��irq����Ч��
	if (irq > 91) irq = 91;
	
	//ȷ����ӦNVICISER
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
 * @˵��: ��ֹirq�ж�
 * @����: irq : �жϺ�
 * @����ֵ: ��
 */
void 
disable_irq(uint16 irq)
{
	uint16 div;
	
	//ȷ��irqΪ��Чirq
	if (irq > 91) irq = 91;
	
	//ȷ����Ӧ��NVICISER
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
 * @˵��: ����irq�жϺ����ȼ�
 * @����: irq :  �жϺ�
 * 		  prio:  ���ȼ�
 * @����ֵ: ��
 */
void 
set_irq_priority(uint16 irq, uint16 prio)
{
   uint8 *prio_reg;
   
   //ȷ��irq�����ȼ���Ч
   if (irq > 91) irq = 91;

   if (prio > 15) irq = 15;

   //ȷ����Ӧ��NVICISER
   prio_reg = (uint8 *)(((uint32)&NVICIP0) + irq);
   //�������ȼ�
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
