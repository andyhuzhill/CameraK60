/*
 * delay.c
 *
 *  Created on: Nov 27, 2012
 *      Author: scout
 *      E-mail: andyhuzhill@gmail.com
 *    HomePage: http://andyhuzhill.github.com
 *
 * Description: ʹ��SysTick��ʱ������ʱ����
 *    Compiler: CodeWarrior 10.2
 *     version: 1.0
 */

#include "delay.h"

///////////////////////////////////////////////////////////////////////////////
//			�ڲ���������
///////////////////////////////////////////////////////////////////////////////
static uint32 fac_us=0;
static uint32 fac_ms=0;

///////////////////////////////////////////////////////////////////////////////
//			�ⲿ�ӿں�������
///////////////////////////////////////////////////////////////////////////////


/*
 * @˵��: ��ʼ����ʱ����  ʹ����ʱ����֮ǰ������ô˺���
 * @����: ϵͳʱ��Ƶ��
 * @����ֵ: ��
 */
void
DELAY_init(uint32 SysClk)
{
	SYST_CSR |= 0x04; //����ϵͳ�δ�ʱ��ԴΪϵͳ�ں�ʱ��
	fac_us = SysClk;
	fac_ms = (uint32) fac_us*1000;
}

/*
 * @˵��: ��ʱn΢��
 * @����: us ΢��
 * @����ֵ: ��
 */
void
delay_us(uint32 us)
{
	uint32 tmp;
	SYST_RVR = us*fac_us;  //����ʱ��
	SYST_CVR = 0x00;  		//���������
	SYST_CSR |= 0x01;       //��ʼ����
	do
	{
		tmp = SYST_CSR;
	} while( tmp & 0x01 &&!(tmp&(1<<16))); //�ȴ�ʱ�䵽��
	SYST_CSR &= ~0x01; 	//�رռ�����
}

/*
 * @˵��: ��ʱn����
 * @����: ms ����
 * @����ֵ: ��
 */
void
delay_ms(uint32 ms)
{
	uint32 tmp;
	uint16 cnt;
	
	for(cnt = 0; cnt < ms; cnt++)
	{
		SYST_RVR = fac_ms;
		SYST_CVR = 0x00;
		SYST_CSR |= 0x01;
		do
		{
			tmp = SYST_CSR;
		}while(tmp&0x01 && !(tmp&(1<<16)));
		SYST_CSR &= ~0x01;
	}
}

/*
*  �������ƣ�time_delay_ms
*  ����˵������ʱ������ʹ�ö����Ķ�ʱ����ʱ��׼ȷ
*  ����˵����ms   ��ʱʱ�䣬��λΪms
*  �������أ���
*************************************************************************/
void 
time_delay_ms(uint32 ms)
{
    /* Make sure the clock to the LPTMR is enabled */
    SIM_SCGC5 |= SIM_SCGC5_LPTIMER_MASK;

    /* Set the compare value to the number of ms to delay */
    LPTMR0_CMR = ms;

    /* Set up LPTMR to use 1kHz LPO with no prescaler as its clock source */
    LPTMR0_PSR = LPTMR_PSR_PCS(1) | LPTMR_PSR_PBYP_MASK;

    /* Start the timer */
    LPTMR0_CSR = LPTMR_CSR_TEN_MASK;

    /* Wait for counter to reach compare value */
    while (!(LPTMR0_CSR & LPTMR_CSR_TCF_MASK));

    /* Clear Timer Compare Flag */
    LPTMR0_CSR &= ~LPTMR_CSR_TEN_MASK;

    return;
}


