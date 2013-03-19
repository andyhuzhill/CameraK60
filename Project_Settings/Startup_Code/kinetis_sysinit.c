/*
 *    kinetis_sysinit.c - Default init routines for
 *                     		Kinetis ARM systems
 *    Copyright � 2010 Freescale semiConductor Inc. All Rights Reserved.
 */

#include "kinetis_sysinit.h"
#include "derivative.h"
#include "isr.h"

typedef void (*const tIsrFunc)(void);
typedef struct {
  uint32_t * __ptr;
  tIsrFunc __fun[119];
} tVectorTable;

extern uint32_t __vector_table[];

#pragma overload void __init_hardware();
void __init_hardware()
{
	  SCB_VTOR = (uint32_t)__vector_table; /* Set the interrupt vector table position */
	/*
		Disable the Watchdog because it may reset the core before entering main().
		There are 2 unlock words which shall be provided in sequence before
		accessing the control register.
	*/
	*(volatile unsigned short *)KINETIS_WDOG_UNLOCK_ADDR = KINETIS_WDOG_UNLOCK_SEQ_1;
	*(volatile unsigned short *)KINETIS_WDOG_UNLOCK_ADDR = KINETIS_WDOG_UNLOCK_SEQ_2;
	*(volatile unsigned short *)KINETIS_WDOG_STCTRLH_ADDR = KINETIS_WDOG_DISABLED_CTRL;

}

void isr_default(void)
{
  /* Write your interrupt code here ... */

}
/* end of isr_default */

void isrINT_NMI(void)
{
  /* Write your interrupt code here ... */

}
/* end of isrINT_NMI */

#ifdef __cplusplus
extern "C" {
#endif
extern uint32_t __SP_INIT[];
extern void __thumb_startup( void );
#ifdef __cplusplus
}
#endif

#pragma define_section vectortable ".vectortable" ".vectortable" ".vectortable" far_abs R
static __declspec(vectortable) tVectorTable __vect_table = { /* Interrupt vector table */
   __SP_INIT,                                              /* 0 (0x00000000) (prior: -) */
  {
   (tIsrFunc)__thumb_startup,                              /* 1 (0x00000004) (prior: -) */
   (tIsrFunc)isrINT_NMI,                                   /* 2 (0x00000008) (prior: -2) */
   (tIsrFunc)VECTOR_003,                               /* 3 (0x0000000C) (prior: -1) */
   (tIsrFunc)VECTOR_004,                               /* 4 (0x00000010) (prior: -) */
   (tIsrFunc)VECTOR_005,                               /* 5 (0x00000014) (prior: -) */
   (tIsrFunc)VECTOR_006,                               /* 6 (0x00000018) (prior: -) */
   (tIsrFunc)VECTOR_007,                               /* 7 (0x0000001C) (prior: -) */
   (tIsrFunc)VECTOR_008,                               /* 8 (0x00000020) (prior: -) */
   (tIsrFunc)VECTOR_009,                               /* 9 (0x00000024) (prior: -) */
   (tIsrFunc)VECTOR_010,                               /* 10 (0x00000028) (prior: -) */
   (tIsrFunc)VECTOR_011,                               /* 11 (0x0000002C) (prior: -) */
   (tIsrFunc)VECTOR_012,                               /* 12 (0x00000030) (prior: -) */
   (tIsrFunc)VECTOR_013,                               /* 13 (0x00000034) (prior: -) */
   (tIsrFunc)VECTOR_014,                               /* 14 (0x00000038) (prior: -) */
   (tIsrFunc)VECTOR_015,                               /* 15 (0x0000003C) (prior: -) */
   (tIsrFunc)VECTOR_016,                               /* 16 (0x00000040) (prior: -) */
   (tIsrFunc)VECTOR_017,                               /* 17 (0x00000044) (prior: -) */
   (tIsrFunc)VECTOR_018,                               /* 18 (0x00000048) (prior: -) */
   (tIsrFunc)VECTOR_019,                               /* 19 (0x0000004C) (prior: -) */
   (tIsrFunc)VECTOR_020,                               /* 20 (0x00000050) (prior: -) */
   (tIsrFunc)VECTOR_021,                               /* 21 (0x00000054) (prior: -) */
   (tIsrFunc)VECTOR_022,                               /* 22 (0x00000058) (prior: -) */
   (tIsrFunc)VECTOR_023,                               /* 23 (0x0000005C) (prior: -) */
   (tIsrFunc)VECTOR_024,                               /* 24 (0x00000060) (prior: -) */
   (tIsrFunc)VECTOR_025,                               /* 25 (0x00000064) (prior: -) */
   (tIsrFunc)VECTOR_026,                               /* 26 (0x00000068) (prior: -) */
   (tIsrFunc)VECTOR_027,                               /* 27 (0x0000006C) (prior: -) */
   (tIsrFunc)VECTOR_028,                               /* 28 (0x00000070) (prior: -) */
   (tIsrFunc)VECTOR_029,                               /* 29 (0x00000074) (prior: -) */
   (tIsrFunc)VECTOR_030,                               /* 30 (0x00000078) (prior: -) */
   (tIsrFunc)VECTOR_031,                               /* 31 (0x0000007C) (prior: -) */
   (tIsrFunc)VECTOR_032,                               /* 32 (0x00000080) (prior: -) */
   (tIsrFunc)VECTOR_033,                               /* 33 (0x00000084) (prior: -) */
   (tIsrFunc)VECTOR_034,                               /* 34 (0x00000088) (prior: -) */
   (tIsrFunc)VECTOR_035,                               /* 35 (0x0000008C) (prior: -) */
   (tIsrFunc)VECTOR_036,                               /* 36 (0x00000090) (prior: -) */
   (tIsrFunc)VECTOR_037,                               /* 37 (0x00000094) (prior: -) */
   (tIsrFunc)VECTOR_038,                               /* 38 (0x00000098) (prior: -) */
   (tIsrFunc)VECTOR_039,                               /* 39 (0x0000009C) (prior: -) */
   (tIsrFunc)VECTOR_040,                               /* 40 (0x000000A0) (prior: -) */
   (tIsrFunc)VECTOR_041,                               /* 41 (0x000000A4) (prior: -) */
   (tIsrFunc)VECTOR_042,                               /* 42 (0x000000A8) (prior: -) */
   (tIsrFunc)VECTOR_043,                               /* 43 (0x000000AC) (prior: -) */
   (tIsrFunc)VECTOR_044,                               /* 44 (0x000000B0) (prior: -) */
   (tIsrFunc)VECTOR_045,                               /* 45 (0x000000B4) (prior: -) */
   (tIsrFunc)VECTOR_046,                               /* 46 (0x000000B8) (prior: -) */
   (tIsrFunc)VECTOR_047,                               /* 47 (0x000000BC) (prior: -) */
   (tIsrFunc)VECTOR_048,                               /* 48 (0x000000C0) (prior: -) */
   (tIsrFunc)VECTOR_049,                               /* 49 (0x000000C4) (prior: -) */
   (tIsrFunc)VECTOR_050,                               /* 50 (0x000000C8) (prior: -) */
   (tIsrFunc)VECTOR_051,                               /* 51 (0x000000CC) (prior: -) */
   (tIsrFunc)VECTOR_052,                               /* 52 (0x000000D0) (prior: -) */
   (tIsrFunc)VECTOR_053,                               /* 53 (0x000000D4) (prior: -) */
   (tIsrFunc)VECTOR_054,                               /* 54 (0x000000D8) (prior: -) */
   (tIsrFunc)VECTOR_055,                               /* 55 (0x000000DC) (prior: -) */
   (tIsrFunc)VECTOR_056,                               /* 56 (0x000000E0) (prior: -) */
   (tIsrFunc)VECTOR_057,                               /* 57 (0x000000E4) (prior: -) */
   (tIsrFunc)VECTOR_058,                               /* 58 (0x000000E8) (prior: -) */
   (tIsrFunc)VECTOR_059,                               /* 59 (0x000000EC) (prior: -) */
   (tIsrFunc)VECTOR_060,                               /* 60 (0x000000F0) (prior: -) */
   (tIsrFunc)VECTOR_061,                               /* 61 (0x000000F4) (prior: -) */
   (tIsrFunc)VECTOR_062,                               /* 62 (0x000000F8) (prior: -) */
   (tIsrFunc)VECTOR_063,                               /* 63 (0x000000FC) (prior: -) */
   (tIsrFunc)VECTOR_064,                               /* 64 (0x00000100) (prior: -) */
   (tIsrFunc)VECTOR_065,                               /* 65 (0x00000104) (prior: -) */
   (tIsrFunc)VECTOR_066,                               /* 66 (0x00000108) (prior: -) */
   (tIsrFunc)VECTOR_067,                               /* 67 (0x0000010C) (prior: -) */
   (tIsrFunc)VECTOR_068,                               /* 68 (0x00000110) (prior: -) */
   (tIsrFunc)VECTOR_069,                               /* 69 (0x00000114) (prior: -) */
   (tIsrFunc)VECTOR_070,                               /* 70 (0x00000118) (prior: -) */
   (tIsrFunc)VECTOR_071,                               /* 71 (0x0000011C) (prior: -) */
   (tIsrFunc)VECTOR_072,                               /* 72 (0x00000120) (prior: -) */
   (tIsrFunc)VECTOR_073,                               /* 73 (0x00000124) (prior: -) */
   (tIsrFunc)VECTOR_074,                               /* 74 (0x00000128) (prior: -) */
   (tIsrFunc)VECTOR_075,                               /* 75 (0x0000012C) (prior: -) */
   (tIsrFunc)VECTOR_076,                               /* 76 (0x00000130) (prior: -) */
   (tIsrFunc)VECTOR_077,                               /* 77 (0x00000134) (prior: -) */
   (tIsrFunc)VECTOR_078,                               /* 78 (0x00000138) (prior: -) */
   (tIsrFunc)VECTOR_079,                               /* 79 (0x0000013C) (prior: -) */
   (tIsrFunc)VECTOR_080,                               /* 80 (0x00000140) (prior: -) */
   (tIsrFunc)VECTOR_081,                               /* 81 (0x00000144) (prior: -) */
   (tIsrFunc)VECTOR_082,                               /* 82 (0x00000148) (prior: -) */
   (tIsrFunc)VECTOR_083,                               /* 83 (0x0000014C) (prior: -) */
   (tIsrFunc)VECTOR_084,                               /* 84 (0x00000150) (prior: -) */
   (tIsrFunc)VECTOR_085,                               /* 85 (0x00000154) (prior: -) */
   (tIsrFunc)VECTOR_086,                               /* 86 (0x00000158) (prior: -) */
   (tIsrFunc)VECTOR_087,                               /* 87 (0x0000015C) (prior: -) */
   (tIsrFunc)VECTOR_088,                               /* 88 (0x00000160) (prior: -) */
   (tIsrFunc)VECTOR_089,                               /* 89 (0x00000164) (prior: -) */
   (tIsrFunc)VECTOR_090,                               /* 90 (0x00000168) (prior: -) */
   (tIsrFunc)VECTOR_091,                               /* 91 (0x0000016C) (prior: -) */
   (tIsrFunc)VECTOR_092,                               /* 92 (0x00000170) (prior: -) */
   (tIsrFunc)VECTOR_093,                               /* 93 (0x00000174) (prior: -) */
   (tIsrFunc)VECTOR_094,                               /* 94 (0x00000178) (prior: -) */
   (tIsrFunc)VECTOR_095,                               /* 95 (0x0000017C) (prior: -) */
   (tIsrFunc)VECTOR_096,                               /* 96 (0x00000180) (prior: -) */
   (tIsrFunc)VECTOR_097,                               /* 97 (0x00000184) (prior: -) */
   (tIsrFunc)VECTOR_098,                               /* 98 (0x00000188) (prior: -) */
   (tIsrFunc)VECTOR_099,                               /* 99 (0x0000018C) (prior: -) */
   (tIsrFunc)VECTOR_100,                               /* 100 (0x00000190) (prior: -) */
   (tIsrFunc)VECTOR_101,                               /* 101 (0x00000194) (prior: -) */
   (tIsrFunc)VECTOR_102,                               /* 102 (0x00000198) (prior: -) */
   (tIsrFunc)VECTOR_103,                               /* 103 (0x0000019C) (prior: -) */
   (tIsrFunc)VECTOR_104,                               /* 104 (0x000001A0) (prior: -) */
   (tIsrFunc)VECTOR_105,                               /* 105 (0x000001A4) (prior: -) */
   (tIsrFunc)VECTOR_106,                               /* 106 (0x000001A8) (prior: -) */
   (tIsrFunc)VECTOR_107,                               /* 107 (0x000001AC) (prior: -) */
   (tIsrFunc)VECTOR_108,                               /* 108 (0x000001B0) (prior: -) */
   (tIsrFunc)VECTOR_109,                               /* 109 (0x000001B4) (prior: -) */
   (tIsrFunc)VECTOR_110,                               /* 110 (0x000001B8) (prior: -) */
   (tIsrFunc)VECTOR_111,                               /* 111 (0x000001BC) (prior: -) */
   (tIsrFunc)VECTOR_112,                               /* 112 (0x000001C0) (prior: -) */
   (tIsrFunc)VECTOR_113,                               /* 113 (0x000001C4) (prior: -) */
   (tIsrFunc)VECTOR_114,                               /* 114 (0x000001C8) (prior: -) */
   (tIsrFunc)VECTOR_115,                               /* 115 (0x000001CC) (prior: -) */
   (tIsrFunc)VECTOR_116,                               /* 116 (0x000001D0) (prior: -) */
   (tIsrFunc)VECTOR_117,                               /* 117 (0x000001D4) (prior: -) */
   (tIsrFunc)VECTOR_118,                               /* 118 (0x000001D8) (prior: -) */
   (tIsrFunc)VECTOR_119                                /* 119 (0x000001DC) (prior: -) */
  }
};


