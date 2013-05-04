/*
 * common.h
 *
 *  Created on: Nov 21, 2012
 *      Author: scout
 *      E-mail: andyhuzhill@gmail.com
 *    HomePage: http://andyhuzhill.github.com
 *
 * Description: definitions used for all the file
 *    Compiler: CodeWarrior v10.2
 * 	   version: 1.0
 */

#ifndef COMMON_H_
#define COMMON_H_

#include "MK60DZ10.h"

//#define DEBUG               //切换调试输出的宏定义
//#define DEBUG_PRINT

#define CORE_CLK_MHZ PLL_100        //定义内核频率

//存储器段的宏定义
#if defined(__CWCC__)
#pragma define_section relocate_code ".relocate_code" ".relocate_code" ".relocate_code" far_abs RX
#pragma define_section relocate_data ".relocate_data" ".relocate_data" ".relocate_data" RW
#pragma define_section relocate_const ".relocate_const" ".relocate_const" ".relocate_const" far_abs R
#define __relocate_code__   __declspec(relocate_code)
#define __relocate_data__   __declspec(relocate_data)
#define __relocate_const__  __declspec(relocate_const)
#elif defined(__IAR_SYSTEMS_ICC__)
#define __relocate_code__ __ramfunc
#else
#error Not supported compiler type
#endif


/*ARM Cortex M4 implementation for interrupt priority shift*/
#define ARM_INTERRUPT_LEVEL_BITS          4

/***********************************************************************/
// function prototypes for arm_cm4.c
void stop (void);
void wait (void);
void write_vtor (int);
void enable_irq (int);
void disable_irq (int);
void set_irq_priority (int, int);

/***********************************************************************/
/*!< Macro to enable all interrupts. */
#define EnableInterrupts asm(" CPSIE i")

/*!< Macro to disable all interrupts. */
#define DisableInterrupts asm(" CPSID i")
/***********************************************************************/


// define for printf
#define TERM_PORT       UART3_BASE_PTR
#define TERMINAL_BAUD   115200

#include "io.h"
#include "stdlib.h"
#include "stdio.h"


//  K60 管脚配置
#include "k60_config.h"


#ifdef  FALSE
#undef  FALSE
#endif
#define FALSE   (0)

#ifdef  TRUE
#undef  TRUE
#endif
#define TRUE    (!FALSE)

#ifdef  NULL
#undef  NULL
#endif
#define NULL    (0)

#ifdef  ON
#undef  ON
#endif
#define ON      (1)

#ifdef  OFF
#undef  OFF
#endif
#define OFF     (0)

#define ERROR (0)
#define SUCCESS (1)

typedef enum {false = 0, true = !false} bool;

/*
 * 定义中断号
 */
typedef enum 
{
    DMA0_IRQn       = 0,     
    DMA1_IRQn       = 1,    
    DMA2_IRQn       = 2,
    DMA3_IRQn       = 3,
    DMA4_IRQn       = 4,
    DMA5_IRQn       = 5,
    DMA6_IRQn       = 6,
    DMA7_IRQn       = 7,
    DMA8_IRQn       = 8,
    DMA9_IRQn       = 9,
    DMA10_IRQn      = 10,
    DMA11_IRQn      = 11,
    DMA12_IRQn      = 12,
    DMA13_IRQn      = 13,
    DMA14_IRQn      = 14,             
    DMA15_IRQn      = 15,
    DMA_ERROR_IRQn  = 16,
    MCM_IRQn        = 17,
    FLASH_CMD_FINISH_IRQn = 18,
    FLASH_READ_IRQn = 19,
    MC_IRQn         = 20,
    LLWU_IRQn       = 21,
    WDOG_IRQn       = 22,
    RNGB_IRQn       = 23,
    I2C0_IRQn       = 24,
    I2C1_IRQn       = 25,
    SPI0_IRQn       = 26,
    SPI1_IRQn       = 27,
    SPI2_IRQn       = 28,
    CAN0_MESSGAGE_IRQn = 29,
    CAN0_BUSOFF_IRQn   = 30,
    CAN0_ERROR_IRQn    = 31,
    CAN0_TxWARN_IRQn   = 32,
    CAN0_RxWARN_IRQn   = 33,
    CAN0_WAKE_IRQn     = 34,
    CAN0_IMEU_IRQn     = 35,
    CAN0_LOST_IRQn     = 36,
    CAN1_MESSGAGE_IRQn = 37,
    CAN1_BUSOFF_IRQn   = 38,
    CAN1_ERROR_IRQn    = 39,
    CAN1_TxWARN_IRQn   = 40,
    CAN1_RxWARN_IRQn   = 41,
    CAN1_WAKE_IRQn     = 42,
    CAN1_IMEU_IRQn     = 43,
    CAN1_LOST_IRQn     = 44,
    UART0_IRQn         = 45,
    UART0_ERROR_IRQn   = 46,
    UART1_IRQn         = 47,
    UART1_ERROR_IRQn   = 48,
    UART2_IRQn         = 49,
    UART2_ERROR_IRQn   = 50,
    UART3_IRQn         = 51,
    UART3_ERROR_IRQn   = 52,
    UART4_IRQn         = 53,
    UART4_ERROR_IRQn   = 54,
    UART5_IRQn         = 55,
    UART5_ERROR_IRQn   = 56,
    ADC0_IRQn          = 57,
    ADC1_IRQn          = 58,
    CMP0_IRQn          = 59,
    CMP1_IRQn          = 60,
    CMP2_IRQn          = 61,
    FTM0_IRQn          = 62,
    FTM1_IRQn          = 63,
    FTM2_IRQn          = 64,  
    CMT_IRQn           = 65,
    RTC_IRQn           = 66,
    DESEVER_IRQn       = 67,
    PIT0_IRQn          = 68,  
    PIT1_IRQn          = 69,
    PIT2_IRQn          = 70,
    PIT3_IRQn          = 71,
    PDB_IRQn           = 72,
    USB_OTG_IRQn       = 73,
    USB_CHARGE_DETECT_IRQn = 74,
    ENET_1588_INT_IRQn = 75,
    ENET_TxINT_IRQn    = 76,
    ENET_RxINT_IRQn    = 77,
    ENET_ERROR_IRQn    = 78,
    I2S_IRQn           = 79,
    SDHC_IRQn          = 80,
    DAC0_IRQn          = 81,  
    DAC1_IRQn          = 82,
    TSI_IRQn           = 83,
    MCG_IRQn           = 84,
    LPT_IRQn           = 85,
    SEG_LCD_IRQn       = 86,
    PORTA_IRQn         = 87,
    PORTB_IRQn         = 88,
    PORTC_IRQn         = 89,
    PORTD_IRQn         = 90,
    PORTE_IRQn         = 91,
}IRQn_TypeDef;

/***********************************************************************/
/*
 * The basic data types
 */
typedef unsigned char       uint8;  /*  8 bits */
typedef unsigned short int  uint16; /* 16 bits */
typedef unsigned long int   uint32; /* 32 bits */

typedef char                int8;   /*  8 bits */
typedef short int           int16;  /* 16 bits */
typedef int                 int32;  /* 32 bits */

typedef volatile int8       vint8;  /*  8 bits */
typedef volatile int16      vint16; /* 16 bits */
typedef volatile int32      vint32; /* 32 bits */

typedef volatile uint8      vuint8;  /*  8 bits */
typedef volatile uint16     vuint16; /* 16 bits */
typedef volatile uint32     vuint32; /* 32 bits */


/*
 * 方便寄存器位运算的宏定义
 */

//置位寄存器reg的bit位
#define BSET(reg, bit) ((reg) |= (1 << bit))
//复位寄存器reg的bit位
#define BCLR(reg, bit) ((reg) &= ~(1 << bit))
//读取寄存器reg bit位上的值
#define BGET(reg, bit) ((reg) >> (bit) &1)

//延时函数
#include "lptmr.h"
#define DELAY() 		time_delay_ms(500)
#define DELAY_MS(ms) 	time_delay_ms(ms)

/*
 * 断言
 */
#include "assert.h"

#ifdef  DEBUG 
#define DEBUG_OUT(FORMAT,...)        do{printf("DEBUG_OUT:\n");printf(FORMAT,##__VA_ARGS__);printf("\n");}while(0)    /*无需打印调试信息时，请将宏内容注释掉*/
#else
#define DEBUG_OUT(FORMAT,...)
#endif


#endif /* COMMON_H_ */
