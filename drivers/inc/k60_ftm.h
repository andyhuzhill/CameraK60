/*
 * k60_ftm.h
 *
 *  Created on: Nov 28, 2012
 *      Author: scout
 *      E-mail: andyhuzhill@gmail.com
 *    HomePage: http://andyhuzhill.github.com
 *
 * Description:	FTM 模块驱动程序
 *    Compiler: CodeWarrior 10.2
 *     version: 1.0
 */

#ifndef K60_FTM_H_
#define K60_FTM_H_

#include "common.h"


extern volatile struct FTM_MemMap *FTMx[3];

#define FTM_PRECISON 100u //定义占空比精度，100为精度1%， 100 为精度 0.1%

#define FTM_IRQ_EN(FTMn,CHn)        FTM_CnSC_REG(FTMx[FTMn],CHn) |= FTM_CnSC_CHIE_MASK       //开启 FTMn_CHn 中断
#define FTM_IRQ_DIS(FTMn,CHn)       FTM_CnSC_REG(FTMx[FTMn],CHn) &= ~FTM_CnSC_CHIE_MASK      //关闭 FTMn_CHn 中断


typedef enum FTMn{
	FTM_0 = 0,
	FTM_1 = 1,
	FTM_2 = 2
} FTM_TypeDef;

typedef enum{
	//			FTM_0	FTM_1	FTM_2
	CH_0 = 0,  //	PTC1	PTA8	PTA10
	CH_1 = 1,  //	PTC2	PTA9	PTA11
	CH_2 = 2,  // 	PTC3	x		x
	CH_3 = 3,  // 	PTC4	x		x
	CH_4 = 4,  //	PTD4	x		x
	CH_5 = 5,  //	PTD5	x		x
	CH_6 = 6,  //	PTD6	x		x
	CH_7 = 7,  // 	PTD7	x		x
}Channel_TypeDef;

typedef enum{
	Rising = 0,  //上升沿捕获
	Falling = 1, //下降沿捕获
	Rising_Falling  = 2 //跳变沿捕获
}Cap_TypeDef;

/*
 * @说明: 初始化FTM的PWM功能，并设置频率、占空比
 * @参数: ftmn 模块号
 * 		  chan 通道号
 * 		  freq	  频率 Hz
 * 		  duty	  占空比
 * @返回值: 0 为成功 否则为失败
 */
uint8
FTM_PWM_init(FTM_TypeDef ftmn, Channel_TypeDef chan, uint32 freq, uint32 duty);

/*
 * @说明: 设置通道占空比
 * @参数: ftmn 模块号
 * 		  chan 通道号
 * 		  duty 占空比		  
 * @返回值: 0为正常，非0 为异常
 */
uint8
FTM_PWM_set_duty(FTM_TypeDef ftmn, Channel_TypeDef chan, uint32 duty);

/*
 * @说明: 设置通道频率
 * @参数:  ftmn 模块号
 * 		   chan  通道号
 * 		   freq  频率
 * @返回值: 0为正常， 非0为异常
 */
uint8
FTM_PWM_set_freq(FTM_TypeDef ftmn, Channel_TypeDef chan, uint32 freq);

/*
 * @说明: 输入捕获初始化
 * @参数: ftmn 模块号
 * 		  chan 通道号
 * 		  capcfg 捕获类型
 * @返回值: 0为正常， 非0为异常
 */
uint8
FTM_Capture_init(FTM_TypeDef ftmn, Channel_TypeDef chan, Cap_TypeDef capcfg);

#endif /* K60_FTM_H_ */
