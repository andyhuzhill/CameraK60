/*
 * k60_ftm.c
 *
 *  Created on: Nov 28, 2012
 *      Author: scout
 *      E-mail: andyhuzhill@gmail.com
 *    HomePage: http://andyhuzhill.github.com
 *
 * Description: FTMģ����������
 *    Compiler: CodeWarrior 10.2
 *     version: 1.0
 */

#include "k60_ftm.h"

//��������ָ�����鱣��FTMn�ĵ�ַ
volatile struct FTM_MemMap *FTMx[3] = {FTM0_BASE_PTR, FTM1_BASE_PTR, FTM2_BASE_PTR};

//����ϵͳ����ʱ��Ƶ�� �� ��ֵ��sysinit.c�ļ��л��
extern uint32 periph_clk_khz;

/*
 * @˵��: ��ʼ��FTM��PWM���ܣ�������Ƶ�ʡ�ռ�ձ�
 * @����: ftmn ģ���
 * 		  chan ͨ����
 * 		  freq	  Ƶ�� Hz
 * 		  duty	  ռ�ձ�
 * @����ֵ: 0 Ϊ�ɹ� ����Ϊʧ��
 */
uint8
FTM_PWM_init(FTM_TypeDef ftmn, Channel_TypeDef chan, uint32 freq, uint32 duty)
{
	uint32 clk_hz = (periph_clk_khz * 1000) >> 1;  // busƵ��/2
	uint16 mod;			//	��ʱ��ģ��ֵ
	uint8  ps;  		//  ��Ƶϵ��
	uint16 cv;			//  ͨ��chan��ƥ��ֵ

	//�������Ƿ���ȷ
	assert((ftmn == FTM_0) || (ftmn == FTM_1) || (ftmn == FTM_2) && (chan <= CH_1));
	assert( freq <= (clk_hz >> 1)); //���Ƶ���Ƿ������� Ƶ�ʱ���С��ʱ�Ӷ���֮һ

	//����Ƶ������
	mod = (clk_hz >> 16) / freq;
	for(ps = 0; (mod >> ps) >= 1; ps++);
	if(freq < 1000) ps++;       

	mod = (clk_hz >> ps) / freq;
	cv = (duty * (mod -0 +1)) / FTM_PRECISON;  //  ռ�ձ� = cv / mod 

	//����ʱ�Ӻ�IO����
	switch(ftmn)
	{
	case FTM_0:
	{
		SIM_SCGC6 |= SIM_SCGC6_FTM0_MASK; 	//ʹ��FTM0ʱ��
		switch(chan)
		{
		case CH_0:
		case CH_1:
		case CH_2:
		case CH_3:
			SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;  // PTCʱ��ʹ��
			PORT_PCR_REG(PORTC_BASE_PTR, chan+1) = PORT_PCR_MUX(4);  //PTC1 ����
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

	//�������ģʽΪ���ض���PWM
	FTM_CnSC_REG(FTMx[ftmn], chan) &= ~FTM_CnSC_ELSA_MASK;
	FTM_CnSC_REG(FTMx[ftmn], chan) = FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK;

	//����ʱ�Ӻͷ�Ƶ  
	FTM_SC_REG(FTMx[ftmn]) = FTM_SC_CPWMS_MASK | FTM_SC_PS(ps) \
			| FTM_SC_CLKS(1); // û��ʹ������ж�
	FTM_CNTIN_REG(FTMx[ftmn]) = 0;   //���������� ��Cnv - CNTIN)
	FTM_MOD_REG(FTMx[ftmn]) = mod;   //EPWM���� MOD-CNTIN + 0x0001
	FTM_CNTIN_REG(FTMx[ftmn]) = 0;   //��������ʼ��ֵ
	FTM_CnV_REG(FTMx[ftmn], chan) = cv;
	FTM_CNT_REG(FTMx[ftmn]) = 0;   //������ ֻ�е�16λ����

	return 0;
}

/*
 * @˵��: ����ͨ��ռ�ձ�
 * @����: ftmn ģ���
 * 		  chan ͨ����
 * 		  duty ռ�ձ�		  
 * @����ֵ: 0Ϊ��������0 Ϊ�쳣
 */
uint8
FTM_PWM_set_duty(FTM_TypeDef ftmn, Channel_TypeDef chan, uint32 duty)
{
	uint32 cv;
	uint32 mod;

	assert( (ftmn == FTM_0) || ((ftmn == FTM_1 || ftmn == FTM_2) && (chan <= CH_1)));
	assert(duty <= FTM_PRECISON);

	//ռ�ձ� = (CnV-CNTIN)/(MOD-CNTIN+1)
	mod = FTM_MOD_REG(FTMx[ftmn]);

	cv = (duty * (mod -0 +1)) / FTM_PRECISON;

	//���� FTMͨ��ֵ
	FTM_CnV_REG(FTMx[ftmn], chan) = cv;

	return 0;
}

/*
 * @˵��: ����ͨ��Ƶ��
 * @����:  ftmn ģ���
 * 		   chan  ͨ����
 * 		   freq  Ƶ��
 * @����ֵ: 0Ϊ������ ��0Ϊ�쳣
 */
uint8
FTM_PWM_set_freq(FTM_TypeDef ftmn, Channel_TypeDef chan, uint32 freq)
{
	uint32 clk_hz = (periph_clk_khz * 1000) >> 1;        //����Ƶ��/2
	uint32 mod;
	uint8 ps;

	assert( freq <= (clk_hz >> 1) );              //�ö��Լ�� Ƶ�� �Ƿ����� ,Ƶ�ʱ���С��ʱ�Ӷ���֮һ

	/*       ����Ƶ������        */
	mod = (clk_hz >> 16 ) / freq ;
	for(ps = 0; (mod >> ps) >= 1; ps++);
	mod = (clk_hz >> ps) / freq;


	/******************** ����ʱ�Ӻͷ�Ƶ ********************/
	FTM_SC_REG(FTMx[ftmn])    = FTM_SC_CPWMS_MASK | FTM_SC_PS(ps) | FTM_SC_CLKS(1);  //û��ʹ������ж�
	FTM_CNTIN_REG(FTMx[ftmn]) = 0;                                                      // Channel (n) Value  �����������ȣ�(CnV - CNTIN).
	FTM_MOD_REG(FTMx[ftmn])   = mod;                                                    //Modulo valueģ��, EPWM������Ϊ ��MOD - CNTIN + 0x0001
	FTM_CNTIN_REG(FTMx[ftmn]) = 0;                                                      //Counter Initial Value ��������ʼ��ֵ
	FTM_CNT_REG(FTMx[ftmn])   = 0;                                                      //��������ֻ�е�16λ����
	
	return 0;
}