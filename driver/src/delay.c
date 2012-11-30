/*
 * delay.c
 *
 *  Created on: Nov 27, 2012
 *      Author: scout
 *      E-mail: andyhuzhill@gmail.com
 *    HomePage: http://andyhuzhill.github.com
 *
 * Description: 使用SysTick定时器的延时函数
 *    Compiler: CodeWarrior 10.2
 *     version: 1.0
 */

#include "delay.h"

///////////////////////////////////////////////////////////////////////////////
//			内部变量定义
///////////////////////////////////////////////////////////////////////////////
static uint32 fac_us=0;
static uint32 fac_ms=0;

///////////////////////////////////////////////////////////////////////////////
//			外部接口函数定义
///////////////////////////////////////////////////////////////////////////////


/*
 * @说明: 初始化延时函数  使用延时函数之前必须调用此函数
 * @参数: 系统时钟频率
 * @返回值: 无
 */
void
DELAY_init(uint32 SysClk)
{
	SYST_CSR |= 0x04; //设置系统滴答时间源为系统内核时间
	fac_us = SysClk;
	fac_ms = (uint32) fac_us*1000;
}

/*
 * @说明: 延时n微秒
 * @参数: us 微秒
 * @返回值: 无
 */
void
delay_us(uint32 us)
{
	uint32 tmp;
	SYST_RVR = us*fac_us;  //加载时间
	SYST_CVR = 0x00;  		//清零计数器
	SYST_CSR |= 0x01;       //开始计数
	do
	{
		tmp = SYST_CSR;
	} while( tmp & 0x01 &&!(tmp&(1<<16))); //等待时间到达
	SYST_CSR &= ~0x01; 	//关闭计数器
}

/*
 * @说明: 延时n毫秒
 * @参数: ms 毫秒
 * @返回值: 无
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

