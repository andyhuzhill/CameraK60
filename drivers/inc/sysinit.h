/*
 * sysinit.h
 *
 *  Created on: Nov 23, 2012
 *      Author: scout
 *      E-mail: andyhuzhill@gmail.com
 *    HomePage: http://andyhuzhill.github.com
 *
 * Description: ϵͳ��ʼ������
 *    Compiler: CodeWarrior 10.2
 * 	   version: 1.0
 */

#ifndef SYSINIT_H_
#define SYSINIT_H_

#include "common.h"
#include "derivative.h"

//ȫ�ֱ�������
uint32 core_clk_khz;    //оƬ�ں�ʱ��Ƶ�ʣ�kHz
uint32 bus_clk_khz;  //оƬ����ʱ��Ƶ�ʣ�kHz

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

//      ��������

/*
 * @˵��: ϵͳ��ʼ���趨 �ú����� CodeWarrior�������������Ѿ������ù��ˣ���������
 *          ����
 * @����: ��
 * @����ֵ: ��
 */
void
__init_user(void);

/*
 * @˵��: ����ʱ�ӳ�ʼ���趨�����ڵ���
 * @����: ��
 * @����ֵ: ��
 */
void
trace_clk_init(void);

/*
 * @˵��: FlexBusʱ�ӳ�ʼ���趨
 * @����: ��
 * @����ֵ: ��
 */
void
fb_clk_init(void);

/*
 * @˵��: pll��ʼ���趨
 * @����: clk_option: ʱ��Ƶ��ѡ��ɺ鶨�����á�
 * @����ֵ: оƬ�ں�ʱ��Ƶ�ʣ�kHz)
 * @����: оƬʹ��PLL ������оƬ�������ʱ�ӣ�оƬ��λ��
 *       MCGĬ�Ͻ���FETģʽ��
 *       �谴�� FEI->FEB->PBE->PEE��˳��������á�
 */
uint32
pll_init(uint8 clk_option);

/*
 * @˵��: ϵͳ��Ƶ���趨��
 * @����: outdiv1-4: Ԥ��Ƶֵ
 * @����ֵ: ��
 * @����: �˺���������RAM����ִ�У�������������e2448.��Flashʱ�ӷ�Ƶ�ı�
 *          ʱ�������ֹFLASHԤ��ȡ���ܡ���ʱ�ӷ�Ƶ�ı�󣬱�����ʱһ��ʱ��
 *          ���������ʹ��Ԥȡ���ܡ�
 */
__relocate_code__
void 
set_sys_dividers(uint32 outdiv1, uint32 outdiv2, uint32 outdiv3, 
            uint32 outdiv4);

#endif /* SYSINIT_H_ */
