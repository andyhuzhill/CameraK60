/*
 * k60_ftm.h
 *
 *  Created on: Nov 28, 2012
 *      Author: scout
 *      E-mail: andyhuzhill@gmail.com
 *    HomePage: http://andyhuzhill.github.com
 *
 * Description:	FTM ģ����������
 *    Compiler: CodeWarrior 10.2
 *     version: 1.0
 */

#ifndef K60_FTM_H_
#define K60_FTM_H_

#include "common.h"

#define FTM_PRECISON 100u //����ռ�ձȾ��ȣ�100Ϊ����1%�� 100 Ϊ���� 0.1%

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

/*
 * @˵��: ��ʼ��FTM��PWM���ܣ�������Ƶ�ʡ�ռ�ձ�
 * @����: ftmn ģ���
 * 		  chan ͨ����
 * 		  freq	  Ƶ�� Hz
 * 		  duty	  ռ�ձ�
 * @����ֵ: 0 Ϊ�ɹ� ����Ϊʧ��
 */
uint8
FTM_PWM_init(FTM_TypeDef ftmn, Channel_TypeDef chan, uint32 freq, uint32 duty);

/*
 * @˵��: ����ͨ��ռ�ձ�
 * @����: ftmn ģ���
 * 		  chan ͨ����
 * 		  duty ռ�ձ�		  
 * @����ֵ: 0Ϊ��������0 Ϊ�쳣
 */
uint8
FTM_PWM_set_duty(FTM_TypeDef ftmn, Channel_TypeDef chan, uint32 duty);

/*
 * @˵��: ����ͨ��Ƶ��
 * @����:  ftmn ģ���
 * 		   chan  ͨ����
 * 		   freq  Ƶ��
 * @����ֵ: 0Ϊ������ ��0Ϊ�쳣
 */
uint8
FTM_PWM_set_freq(FTM_TypeDef ftmn, Channel_TypeDef chan, uint32 freq);



#endif /* K60_FTM_H_ */
