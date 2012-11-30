/*
 * k60_ftm.c
 *
 *  Created on: Nov 28, 2012
 *      Author: scout
 *      E-mail: andyhuzhill@gmail.com
 *    HomePage: http://andyhuzhill.github.com
 *
 * Description: FTM模块驱动程序
 *    Compiler: CodeWarrior 10.2
 *     version: 1.0
 */

#include "k60_ftm.h"

//定义三个指针数组保存FTMn的地址
volatile struct FTM_MemMap *FTMx[3] = {FTM0_BASE_PTR, FTM1_BASE_PTR, FTM2_BASE_PTR};

//引用系统总线时钟频率 ， 该值在sysinit.c文件中获得
extern uint32 periph_clk_khz;

/*
 * @说明: 初始化FTM的PWM功能，并设置频率、占空比
 * @参数: ftmn 模块号
 * 		  chan 通道号
 * 		  freq	  频率 Hz
 * 		  duty	  占空比
 * @返回值: 0 为成功 否则为失败
 */
uint8
FTM_PWM_init(FTM_TypeDef ftmn, Channel_TypeDef chan, uint32 freq, uint32 duty)
{
	uint32 clk_hz = (periph_clk_khz * 1000) >> 1;  // bus频率/2
	uint16 mod;			//	定时器模数值
	uint8  ps;  		//  分频系数
	uint16 cv;			//  通道chan的匹配值

	//检查参数是否正确
	assert((ftmn == FTM_0) || (ftmn == FTM_1) || (ftmn == FTM_2) && (chan <= CH_1));
	assert( freq <= (clk_hz >> 1)); //检查频率是否正常， 频率必须小于时钟二分之一

	//计算频率设置
	mod = (clk_hz >> 16) / freq;
	for(ps = 0; (mod >> ps) >= 1; ps++);
	if(freq < 1000) ps++;       

	mod = (clk_hz >> ps) / freq;
	cv = (duty * (mod -0 +1)) / FTM_PRECISON;  //  占空比 = cv / mod 

	//开启时钟和IO复用
	switch(ftmn)
	{
	case FTM_0:
	{
		SIM_SCGC6 |= SIM_SCGC6_FTM0_MASK; 	//使能FTM0时钟
		switch(chan)
		{
		case CH_0:
		case CH_1:
		case CH_2:
		case CH_3:
			SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;  // PTC时钟使能
			PORT_PCR_REG(PORTC_BASE_PTR, chan+1) = PORT_PCR_MUX(4);  //PTC1 复用
			break;
		case CH_4:
		case CH_5:
		case CH_6:
		case CH_7:
			SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
			PORT_PCR_REG(PORTD_BASE_PTR, chan) = PORT_PCR_MUX(4);
		default:
			return -1;
			break;
		}
		break;
	}
	case FTM_1:
	{
		SIM_SCGC6 |= SIM_SCGC6_FTM1_MASK;
		switch(chan)
		{
		case CH_0:
		case CH_1:
			SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
			PORT_PCR_REG(PORTA_BASE_PTR, chan +8) = PORT_PCR_MUX(3);
			break;
		default:
			return -1;
			break;
		}
		break;
	}
	case FTM_2:
	{
		SIM_SCGC3 |= SIM_SCGC3_FTM2_MASK;
		switch(chan)
		{
		case CH_0:
		case CH_1:
			SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
			PORT_PCR_REG(PORTA_BASE_PTR, chan + 10) = PORT_PCR_MUX(3);
			break;
		default:
			return -1;
			break;
		}
		break;
	}
	default:
		return -1;
		break;
	}

	//设置输出模式为边沿对齐PWM
	FTM_CnSC_REG(FTMx[ftmn], chan) &= ~FTM_CnSC_ELSA_MASK;
	FTM_CnSC_REG(FTMx[ftmn], chan) = FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;

	//配置时钟和分频  
	FTM_SC_REG(FTMx[ftmn]) = FTM_SC_CPWMS_MASK | FTM_SC_PS(ps) \
			| FTM_SC_CLKS(1); // 没有使能溢出中断
	FTM_CNTIN_REG(FTMx[ftmn]) = 0;   //设置脉冲宽度 （Cnv - CNTIN)
	FTM_MOD_REG(FTMx[ftmn]) = mod;   //EPWM周期 MOD-CNTIN + 0x0001
	FTM_CNTIN_REG(FTMx[ftmn]) = 0;   //计数器初始化值
	FTM_CnV_REG(FTMx[ftmn], chan) = cv;
	FTM_CNT_REG(FTMx[ftmn]) = 0;   //计数器 只有低16位可用

	return 0;
}

/*
 * @说明: 设置通道占空比
 * @参数: ftmn 模块号
 * 		  chan 通道号
 * 		  duty 占空比		  
 * @返回值: 0为正常，非0 为异常
 */
uint8
FTM_PWM_set_duty(FTM_TypeDef ftmn, Channel_TypeDef chan, uint32 duty)
{
	uint32 cv;
	uint32 mod;

	assert( (ftmn == FTM_0) || ((ftmn == FTM_1 || ftmn == FTM_2) && (chan <= CH_1)));
	assert(duty <= FTM_PRECISON);

	//占空比 = (CnV-CNTIN)/(MOD-CNTIN+1)
	mod = FTM_MOD_REG(FTMx[ftmn]);

	cv = (duty * (mod -0 +1)) / FTM_PRECISON;

	//配置 FTM通道值
	FTM_CnV_REG(FTMx[ftmn], chan) = cv;

	return 0;
}

/*
 * @说明: 设置通道频率
 * @参数:  ftmn 模块号
 * 		   chan  通道号
 * 		   freq  频率
 * @返回值: 0为正常， 非0为异常
 */
uint8
FTM_PWM_set_freq(FTM_TypeDef ftmn, Channel_TypeDef chan, uint32 freq)
{
	uint32 clk_hz = (periph_clk_khz * 1000) >> 1;        //总线频率/2
	uint32 mod;
	uint8 ps;

	assert( freq <= (clk_hz >> 1) );              //用断言检测 频率 是否正常 ,频率必须小于时钟二分之一

	/*       计算频率设置        */
	mod = (clk_hz >> 16 ) / freq ;
	for(ps = 0; (mod >> ps) >= 1; ps++);
	mod = (clk_hz >> ps) / freq;


	/******************** 配置时钟和分频 ********************/
	FTM_SC_REG(FTMx[ftmn])    = FTM_SC_CPWMS_MASK | FTM_SC_PS(ps) | FTM_SC_CLKS(1);  //没有使能溢出中断
	FTM_CNTIN_REG(FTMx[ftmn]) = 0;                                                      // Channel (n) Value  。设置脉冲宽度：(CnV - CNTIN).
	FTM_MOD_REG(FTMx[ftmn])   = mod;                                                    //Modulo value模数, EPWM的周期为 ：MOD - CNTIN + 0x0001
	FTM_CNTIN_REG(FTMx[ftmn]) = 0;                                                      //Counter Initial Value 计数器初始化值
	FTM_CNT_REG(FTMx[ftmn])   = 0;                                                      //计数器。只有低16位可用
	
	return 0;
}