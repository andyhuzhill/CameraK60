/*
 * k60_gpio.h
 *
 *  Created on: Nov 21, 2012
 *      Author: scout
 *      E-mail: andyhuzhill@gmail.com
 *    HomePage: http://andyhuzhill.github.com
 *
 * Description: GPIO���� ��������
 *    Compiler: CoderWarrior v10.2
 * 	   version: 1.0
 *
 */

#ifndef K60_GPIO_H_
#define K60_GPIO_H_

#include "common.h"
#include "gpio_cfg.h"


extern volatile struct GPIO_MemMap *GPIOx[5]; //�������ָ�����鱣�� GPIOx �ĵ�ַ
extern volatile struct PORT_MemMap *PORTX[5];


////////////////////////////////////////////////////////////////////////////////
//		�ṹ���볣������
////////////////////////////////////////////////////////////////////////////////

typedef enum{
    GPIO_A 	= 0,
    GPIO_B 	= 1,
    GPIO_C 	= 2,
    GPIO_D 	= 3,
    GPIO_E  	= 4
}GPIO_TypeDef;

typedef enum{
    Mode_IN 	 = 0,
    Mode_OUT 	 = 1
}GPIOMode_TypeDef;


typedef enum{
    Low 	=0,
    High	=1,
    PullDown = 2,                       //����ʱ����������ʹ��
    PullUp = 3,                           //����ʱ����������ʹ��
    NoPull = 4                           //����ʱ��������������
}GPIOState_TypeDef;

typedef enum{
	None_IRQ         = 0,             // �ر��жϺ�DMA����
	DMA_onRising  = 1,            // ������DMA����
	DMA_onFalling = 2,            // �½���DMA����
	DMA_onEither  = 3,            // ������DMA����
	IRQ_Zero          = 8,            // ���ƽ�ж�
	IRQ_Rising        = 9,            // �������ж�
	IRQ_Both          = 10,          // �����ض������ж�
	IRQ_One           = 11           // һ��ƽ�ж�
}GPIOIRQ_TypeDef;

/*
 * 144��K60���Ŷ��� 
 * PA �� PTA0~PTA19�� PTA24~PTA29
 * PB �� PTB0~PTB11�� PTB16~PTB23
 * PC �� PTC0~PTC19
 * PD �� PTD0~PTD15
 * PE �� PTE0~PTE12�� PTE24~PTE28
 */

#define GPIO_Pin_0                 ((uint32)0x00000001)  /* Pin 0 selected */
#define GPIO_Pin_1                 ((uint32)0x00000002)  /* Pin 1 selected */
#define GPIO_Pin_2                 ((uint32)0x00000004)  /* Pin 2 selected */
#define GPIO_Pin_3                 ((uint32)0x00000008)  /* Pin 3 selected */
#define GPIO_Pin_4                 ((uint32)0x00000010)  /* Pin 4 selected */
#define GPIO_Pin_5                 ((uint32)0x00000020)  /* Pin 5 selected */
#define GPIO_Pin_6                 ((uint32)0x00000040)  /* Pin 6 selected */
#define GPIO_Pin_7                 ((uint32)0x00000080)  /* Pin 7 selected */
#define GPIO_Pin_8                 ((uint32)0x00000100)  /* Pin 8 selected */
#define GPIO_Pin_9                 ((uint32)0x00000200)  /* Pin 9 selected */
#define GPIO_Pin_10                ((uint32)0x00000400)  /* Pin 10 selected */
#define GPIO_Pin_11                ((uint32)0x00000800)  /* Pin 11 selected */
#define GPIO_Pin_12                ((uint32)0x00001000)  /* Pin 12 selected */
#define GPIO_Pin_13                ((uint32)0x00002000)  /* Pin 13 selected */
#define GPIO_Pin_14                ((uint32)0x00004000)  /* Pin 14 selected */
#define GPIO_Pin_15                ((uint32)0x00008000)  /* Pin 15 selected */
#define GPIO_Pin_16                ((uint32)0x00010000)  /* Pin 16 selected */
#define GPIO_Pin_17                ((uint32)0x00020000)  /* Pin 17 selected */
#define GPIO_Pin_18                ((uint32)0x00040000)  /* Pin 18 selected */
#define GPIO_Pin_19                ((uint32)0x00080000)  /* Pin 19 selected */
#define GPIO_Pin_20                ((uint32)0x00100000)  /* Pin 20 selected */
#define GPIO_Pin_21                ((uint32)0x00200000)  /* Pin 21 selected */
#define GPIO_Pin_22                ((uint32)0x00400000)  /* Pin 22 selected */
#define GPIO_Pin_23                ((uint32)0x00800000)  /* Pin 23 selected */
#define GPIO_Pin_24                ((uint32)0x01000000)  /* Pin 24 selected */
#define GPIO_Pin_25                ((uint32)0x02000000)  /* Pin 25 selected */
#define GPIO_Pin_26                ((uint32)0x04000000)  /* Pin 26 selected */
#define GPIO_Pin_27                ((uint32)0x08000000)  /* Pin 27 selected */
#define GPIO_Pin_28                ((uint32)0x10000000)  /* Pin 28 selected */
#define GPIO_Pin_29                ((uint32)0x20000000)  /* Pin 29 selected */
#define GPIO_Pin_30                ((uint32)0x40000000)  /* Pin 30 selected */
#define GPIO_Pin_31                ((uint32)0x80000000)  /* Pin 31 selected */

typedef struct {
    uint32 Pin;
    GPIOMode_TypeDef MODE;
    GPIOState_TypeDef STATE; //���״̬ʱ ��ѡ State_High State_Low
    					                      //����״̬ʱ����ѡ NoPull, State_PullUp,\
    										     State_PullDown
    GPIOIRQ_TypeDef IRQC;   //�ж�����
}GPIO_InitTypeDef;


//����ܽŷ���
typedef enum GPIO_CFG
{
    //�����ֵ���ܸģ�����
    GPI           = 0,                          //����ܽ����뷽��      GPIOx_PDDRn�0��ʾ���룬1��ʾ���
    GPO         = 1,                          //����ܽ��������

    GPI_DOWN    = 0x02,                       //��������              PORTx_PCRn��ҪPE=1��PS=0
    GPI_UP      = 0x03,                       //��������              PORTx_PCRn��ҪPE=1��PS=1
    GPI_PF      = 0x10,                       //���룬����Դ�˲���,�˲���Χ��10 MHz ~ 30 MHz ����֧�ָ��ٽӿڣ�>=2MHz��  0b10000           Passive Filter Enable
    GPI_DOWN_PF = GPI_DOWN | GPI_PF ,         //��������������Դ�˲���
    GPI_UP_PF   = GPI_UP   | GPI_PF ,         //��������������Դ�˲���

    GPO_HDS     = 0x41,                        //�������������   0b100 0001    High drive strength
    GPO_SSR     = 0x05,                        //������仯��          0b101     Slow slew rate
    GPO_HDS_SSR = GPO_HDS | GPO_SSR,           //������������������仯��
} GPIO_CFG;  //���λΪ0���϶������룻GPI_UP �� GPI_UP_PF�����λΪ1������Ϊ���

#define HIGH  1u
#define LOW   0u



///////////////////////////////////////////////////////////////////////////////
//			�ӿں�������
///////////////////////////////////////////////////////////////////////////////

/*
 * @˵��: ��ʼ��GPIO��
 * @����:  port: �˿��� PORT_A~GPIO_E
 *          pin: ���� ȡֵ1 ~ 31
 *          dir: ����������� 
 *        state: ��ʼ״̬
 * @����ֵ: 0 �������أ� ����ֵΪ�쳣
 */
uint8 
gpio_init(GPIO_TypeDef port, uint8 pin, GPIOMode_TypeDef dir, GPIOState_TypeDef state);

/*
 * @˵��: ��ʼ��GPIO��
 * @����:  port: �˿���  PORT_A~GPIO_E
 *  	   init: ��ʼ�����ýṹ��
 * @����ֵ: 0 �������أ� ����ֵΪ�쳣
 */
uint8 
GPIO_init(GPIO_TypeDef port, GPIO_InitTypeDef *InitStruct);

/*
 * @˵��: �趨ָ���˿�����״̬
 * @����: port: �˿��� PORT_A~GPIO_E
 * 		   pin: ���� ȡֵ 1~31
 * 		 state: ״̬
 * @����ֵ: ��
 */		  
void
GPIO_write_bit(GPIO_TypeDef port, uint8 pin, GPIOState_TypeDef state);

/*
 * @˵��: �趨ָ���˿�״̬
 * @����: port: �˿��� PORT_A~GPIO_E
 * 		  byte: ״̬
 * @����ֵ: ��
 */
void
GPIO_write(GPIO_TypeDef port, uint32 byte);

/*
 * @˵��: �õ�ָ���˿�����״̬
 * @����: port:  �˿��� PORT_A~GPIO_E
 * 	       pin:  ����  ȡֵ 1 ~31
 * @����ֵ ָ������״̬ 0 = �͵�ƽ 1 = �ߵ�ƽ
 */
uint8
GPIO_read_bit(GPIO_TypeDef port, uint8 pin);

/*
 * @˵��: �õ�ָ���˿�״̬
 * @����: port: �˿��� PORT_A~GPIO_E
 * @����ֵ: ָ���˿�״̬ 32λ�޷�����
 */
uint32
GPIO_read(GPIO_TypeDef port);

/*
 * @˵��: �˿�ָ������״̬��ת
 * @����: port: �˿���
 * 		   pin: ���� ȡֵ 1 ~ 31
 * @����ֵ: ��
 */
void
GPIO_toggle_bit(GPIO_TypeDef port, uint8 pin);

/*
 * @˵��: �˿�״̬��ת
 * @����: port: �˿���
 * @����ֵ: ��
 */
void
GPIO_toggle(GPIO_TypeDef port);

#endif /* K60_GPIO_H_ */
