/*
 * k60_wdog.c
 *
 *  Created on: Nov 24, 2012
 *      Author: scout
 *      E-mail: andyhuzhill@gmail.com
 *    HomePage: http://andyhuzhill.github.com
 *
 * Description: 看门狗模块
 *    Compiler: CodeWarrior 10.2
 * 	   version: 1.0
 */

#include "k60_wdog.h"


/*
 * @说明: 初始化看门狗
 * @参数: 无
 * @返回值: 无
 */
void
WDOG_init(void)
{
    //首先解锁看门狗
    WDOG_unlock();
    //预分频
    WDOG_PRESC = 0x0400; //看门狗使用频率 40MHz/5 = 9.6MHz
    //阈值 看门狗超时时间设为1s
    WDOG_TOVALH = 0x0092;
    WDOG_TOVALL = 0x7c00;
    
    //使用总线时间，禁止窗口模式，禁止看门狗中断，开启看门狗
    WDOG_STCTRLH = 0x01d3;
}

/*
 * @说明: 关闭看门狗
 * @参数: 无
 * @返回值: 无
 */
void
WDOG_disable(void)
{
    //首先解锁看门狗，才能给看门狗寄存器写值
    WDOG_unlock();
    
    // 清除看门狗使能位以关闭看门狗
    WDOG_STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;
}

/*
 * @说明: 启动看门狗
 * @参数: 无
 * @返回值: 无
 */
void
WDOG_enable(void)
{
    //解锁看门狗
    WDOG_unlock();
    //开启看门狗
    WDOG_STCTRLH |= WDOG_STCTRLH_WDOGEN_MASK;
}

/*
 * @说明: 复位看门狗， 喂狗
 * @参数: 无
 * @返回值: 无
 */
void
WDOG_feed(void)
{
    WDOG_unlock();
    
    WDOG_PRESC = 0x0400;
    
    //刷新序列
    WDOG_REFRESH = 0xa602;
    WDOG_REFRESH = 0xb480;
}

/*
 * @说明: 启动看门狗中断
 * @参数: 无
 * @返回值: 无
 */
void
WDOG_enable_int(void)
{
    WDOG_unlock();
    //开看门狗中断
    WDOG_STCTRLH |= WDOG_STCTRLH_IRQRSTEN_MASK;
    //启动IRQ中断
    enable_irq(22);
}

/*
 * @说明: 关闭看门狗中断
 * @参数: 无
 * @返回值: 无
 */
void
WDOG_disable_int(void)
{
    WDOG_unlock();
    //关闭看门狗中断
    WDOG_STCTRLH &= ~WDOG_STCTRLH_IRQRSTEN_MASK;
}

/*
 * @说明:解锁 看门狗
 * @参数: 无
 * @返回值: 无
 */
void
WDOG_unlock(void)
{
  /* 注意: 不要单步调试此函数！！
   * 看门狗解锁需要在20个时钟周期内写入指令序列，单步调试会引起CPU复位
   */  
    
    //关总中断
    DINT;
    
    WDOG_UNLOCK = 0xc520;
    WDOG_UNLOCK = 0xd928;
    
    //开总中断
    EINT;
}
