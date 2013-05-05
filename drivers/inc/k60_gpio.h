/*
 * k60_gpio.h
 *
 *  Created on: Nov 21, 2012
 *      Author: scout
 *      E-mail: andyhuzhill@gmail.com
 *    HomePage: http://andyhuzhill.github.com
 *
 * Description: GPIO驱动 程序声明
 *    Compiler: CoderWarrior v10.2
 * 	   version: 1.0
 *
 */

#ifndef K60_GPIO_H_
#define K60_GPIO_H_

#include "common.h"
#include "gpio_cfg.h"


extern volatile struct GPIO_MemMap *GPIOx[5]; //定义五个指针数组保存 GPIOx 的地址
extern volatile struct PORT_MemMap *PORTX[5];


////////////////////////////////////////////////////////////////////////////////
//		结构体与常量声明
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
    PullDown = 2,                       //输入时，下拉电阻使能
    PullUp = 3,                           //输入时，上拉电阻使能
    NoPull = 4                           //输入时，无上下拉电阻
}GPIOState_TypeDef;

typedef enum{
	None_IRQ         = 0,             // 关闭中断和DMA请求
	DMA_onRising  = 1,            // 上升沿DMA请求
	DMA_onFalling = 2,            // 下降沿DMA请求
	DMA_onEither  = 3,            // 上下沿DMA请求
	IRQ_Zero          = 8,            // 零电平中断
	IRQ_Rising        = 9,            // 上升沿中断
	IRQ_Both          = 10,          // 上下沿都触发中断
	IRQ_One           = 11           // 一电平中断
}GPIOIRQ_TypeDef;

/*
 * 144脚K60引脚定义 
 * PA 有 PTA0~PTA19、 PTA24~PTA29
 * PB 有 PTB0~PTB11、 PTB16~PTB23
 * PC 有 PTC0~PTC19
 * PD 有 PTD0~PTD15
 * PE 有 PTE0~PTE12、 PTE24~PTE28
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
    GPIOState_TypeDef STATE; //输出状态时 可选 State_High State_Low
    					                      //输入状态时，可选 NoPull, State_PullUp,\
    										     State_PullDown
    GPIOIRQ_TypeDef IRQC;   //中断类型
}GPIO_InitTypeDef;


//定义管脚方向
typedef enum GPIO_CFG
{
    //这里的值不能改！！！
    GPI           = 0,                          //定义管脚输入方向      GPIOx_PDDRn里，0表示输入，1表示输出
    GPO         = 1,                          //定义管脚输出方向

    GPI_DOWN    = 0x02,                       //输入下拉              PORTx_PCRn需要PE=1，PS=0
    GPI_UP      = 0x03,                       //输入上拉              PORTx_PCRn需要PE=1，PS=1
    GPI_PF      = 0x10,                       //输入，带无源滤波器,滤波范围：10 MHz ~ 30 MHz 。不支持高速接口（>=2MHz）  0b10000           Passive Filter Enable
    GPI_DOWN_PF = GPI_DOWN | GPI_PF ,         //输入下拉，带无源滤波器
    GPI_UP_PF   = GPI_UP   | GPI_PF ,         //输入上拉，带无源滤波器

    GPO_HDS     = 0x41,                        //输出高驱动能力   0b100 0001    High drive strength
    GPO_SSR     = 0x05,                        //输出慢变化率          0b101     Slow slew rate
    GPO_HDS_SSR = GPO_HDS | GPO_SSR,           //输出高驱动能力、慢变化率
} GPIO_CFG;  //最低位为0，肯定是输入；GPI_UP 和 GPI_UP_PF的最低位为1，其他为输出

#define HIGH  1u
#define LOW   0u



///////////////////////////////////////////////////////////////////////////////
//			接口函数声明
///////////////////////////////////////////////////////////////////////////////

/*
 * @说明: 初始化GPIO口
 * @参数:  port: 端口名 PORT_A~GPIO_E
 *          pin: 引脚 取值1 ~ 31
 *          dir: 输入输出方向 
 *        state: 初始状态
 * @返回值: 0 正常返回， 其他值为异常
 */
uint8 
gpio_init(GPIO_TypeDef port, uint8 pin, GPIOMode_TypeDef dir, GPIOState_TypeDef state);

/*
 * @说明: 初始化GPIO口
 * @参数:  port: 端口名  PORT_A~GPIO_E
 *  	   init: 初始化配置结构体
 * @返回值: 0 正常返回， 其他值为异常
 */
uint8 
GPIO_init(GPIO_TypeDef port, GPIO_InitTypeDef *InitStruct);

/*
 * @说明: 设定指定端口引脚状态
 * @参数: port: 端口名 PORT_A~GPIO_E
 * 		   pin: 引脚 取值 1~31
 * 		 state: 状态
 * @返回值: 无
 */		  
void
GPIO_write_bit(GPIO_TypeDef port, uint8 pin, GPIOState_TypeDef state);

/*
 * @说明: 设定指定端口状态
 * @参数: port: 端口名 PORT_A~GPIO_E
 * 		  byte: 状态
 * @返回值: 无
 */
void
GPIO_write(GPIO_TypeDef port, uint32 byte);

/*
 * @说明: 得到指定端口引脚状态
 * @参数: port:  端口名 PORT_A~GPIO_E
 * 	       pin:  引脚  取值 1 ~31
 * @返回值 指定引脚状态 0 = 低电平 1 = 高电平
 */
uint8
GPIO_read_bit(GPIO_TypeDef port, uint8 pin);

/*
 * @说明: 得到指定端口状态
 * @参数: port: 端口名 PORT_A~GPIO_E
 * @返回值: 指定端口状态 32位无符号数
 */
uint32
GPIO_read(GPIO_TypeDef port);

/*
 * @说明: 端口指定引脚状态翻转
 * @参数: port: 端口名
 * 		   pin: 引脚 取值 1 ~ 31
 * @返回值: 无
 */
void
GPIO_toggle_bit(GPIO_TypeDef port, uint8 pin);

/*
 * @说明: 端口状态翻转
 * @参数: port: 端口名
 * @返回值: 无
 */
void
GPIO_toggle(GPIO_TypeDef port);

#endif /* K60_GPIO_H_ */
