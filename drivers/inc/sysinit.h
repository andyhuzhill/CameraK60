/*
 * sysinit.h
 *
 *  Created on: Nov 23, 2012
 *      Author: scout
 *      E-mail: andyhuzhill@gmail.com
 *    HomePage: http://andyhuzhill.github.com
 *
 * Description: 系统初始化程序
 *    Compiler: CodeWarrior 10.2
 * 	   version: 1.0
 */

#ifndef SYSINIT_H_
#define SYSINIT_H_

#include "common.h"
#include "derivative.h"

//全局变量声明
uint32 core_clk_khz;    //芯片内核时钟频率，kHz
uint32 bus_clk_khz;  //芯片总线时钟频率，kHz

enum PLL_option
{
  PLL_48 = 48,
  PLL_50 = 50,
  PLL_60=60,
  PLL_70=70,
  PLL_80=80,
  PLL_90=90,
  PLL_96=96,
  PLL_100=100,
  PLL_110=110,
  PLL_120=120,
  PLL_130=130,
  PLL_150=150,
  PLL_170=170,
  PLL_180=180,
  PLL_200=200,
  PLL_209=209
};

//      函数声明

/*
 * @说明: 系统初始化设定 该函数在 CodeWarrior的启动代码中已经被调用过了，不必重新
 *          调用
 * @参数: 无
 * @返回值: 无
 */
void
__init_user(void);

/*
 * @说明: 跟踪时钟初始化设定。用于调试
 * @参数: 无
 * @返回值: 无
 */
void
trace_clk_init(void);

/*
 * @说明: FlexBus时钟初始化设定
 * @参数: 无
 * @返回值: 无
 */
void
fb_clk_init(void);

/*
 * @说明: pll初始化设定
 * @参数: clk_option: 时钟频率选项，由洪定义配置。
 * @返回值: 芯片内核时钟频率（kHz)
 * @补充: 芯片使用PLL 来产生芯片所需各种时钟，芯片复位后，
 *       MCG默认进入FET模式。
 *       需按照 FEI->FEB->PBE->PEE的顺序进行配置。
 */
uint32
pll_init(uint8 clk_option);

/*
 * @说明: 系统分频器设定。
 * @参数: outdiv1-4: 预分频值
 * @返回值: 无
 * @补充: 此函数必须在RAM里面执行，否则会产生错误e2448.当Flash时钟分频改变
 *          时，必须禁止FLASH预存取功能。在时钟分频改变后，必须延时一段时间
 *          后才能重新使能预取功能。
 */
__relocate_code__
void 
set_sys_dividers(uint32 outdiv1, uint32 outdiv2, uint32 outdiv3, 
            uint32 outdiv4);

#endif /* SYSINIT_H_ */
