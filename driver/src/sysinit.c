/*
 * sysinit.c
 *
 *  Created on: Nov 23, 2012
 *      Author: scout
 *      E-mail: andyhuzhill@gmail.com
 *    HomePage: http://andyhuzhill.github.com
 *
 * Description:
 *    Compiler: 
 * 	   version:
 */

#include "sysinit.h"
#include "uart.h"

/*
 * @˵��: ϵͳ��ʼ���趨 �ú����� CodeWarrior�������������Ѿ������ù��ˣ���������
 *          ����
 * @����: ��
 * @����ֵ: ��
 */
#pragma overload void __init_user();
void
__init_user(void)
{
    //ʹ��IO�˿�ʱ��
    SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
            | SIM_SCGC5_PORTB_MASK
            | SIM_SCGC5_PORTC_MASK
            | SIM_SCGC5_PORTD_MASK
            | SIM_SCGC5_PORTE_MASK );

    //����ϵͳʱ��,�õ��ں�/ϵͳʱ��, ����ʱ��
    core_clk_khz=pll_init(CORE_CLK_MHZ);
    bus_clk_khz = core_clk_khz / (((SIM_CLKDIV1 //����ʱ�ӣ�����ʱ�ӣ�KHz��
            & SIM_CLKDIV1_OUTDIV2_MASK) >> 24)+ 1);  
    //ʹ�ܸ���ʱ�ӣ����ڵ���
    trace_clk_init();   
    //FlexBusʱ�ӳ�ʼ��
    fb_clk_init();  

    /* Enable the pins for the selected UART */
    if (TERM_PORT == UART0_BASE_PTR)
    {
        /* Enable the UART0_TXD function on PTD6 */
        PORTD_PCR6 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin

        /* Enable the UART0_RXD function on PTD7 */
        PORTD_PCR7 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
    }

    if (TERM_PORT == UART1_BASE_PTR)
    {
        /* Enable the UART1_TXD function on PTC4 */
        PORTC_PCR4 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin

        /* Enable the UART1_RXD function on PTC3 */
        PORTC_PCR3 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
    }

    if (TERM_PORT == UART2_BASE_PTR)
    {
        /* Enable the UART2_TXD function on PTD3 */
        PORTD_PCR3 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin

        /* Enable the UART2_RXD function on PTD2 */
        PORTD_PCR2 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
    }

    if (TERM_PORT == UART3_BASE_PTR)
    {
        /* Enable the UART3_TXD function on PTC17 */
        PORTC_PCR17 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin

        /* Enable the UART3_RXD function on PTC16 */
        PORTC_PCR16 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
    }
    if (TERM_PORT == UART4_BASE_PTR)
    {
        /* Enable the UART3_TXD function on PTC17 */
        PORTE_PCR24 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin

        /* Enable the UART3_RXD function on PTC16 */
        PORTE_PCR25 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
    }
    if (TERM_PORT == UART5_BASE_PTR)
    {
        /* Enable the UART3_TXD function on PTC17 */
        PORTE_PCR8 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin

        /* Enable the UART3_RXD function on PTC16 */
        PORTE_PCR9 = PORT_PCR_MUX(0x3); // UART is alt3 function for this pin
    }
    /* UART0 and UART1 are clocked from the core clock, but all other UARTs are
     * clocked from the peripheral clock. So we have to determine which clock
     * to send to the uart_init function.
     */
    if ((TERM_PORT == UART0_BASE_PTR) | (TERM_PORT == UART1_BASE_PTR))
        uart_init (TERM_PORT, core_clk_khz, TERMINAL_BAUD);
    else
        uart_init (TERM_PORT, bus_clk_khz, TERMINAL_BAUD);
}



/*
 * @˵��: ����ʱ�ӳ�ʼ���趨�����ڵ���
 * @����: ��
 * @����ֵ: ��
 */
void
trace_clk_init(void)
{
    //���ø���ʱ��Ϊ�ں�ʱ��
    SIM_SOPT2 |= SIM_SOPT2_TRACECLKSEL_MASK;    
    //��PTA6������ʹ��TRACE_CLKOUT����
    PORTA_PCR6 = ( PORT_PCR_MUX(0x7));
}

/*
 * @˵��: FlexBusʱ�ӳ�ʼ���趨
 * @����: ��
 * @����ֵ: ��
 */
void 
fb_clk_init(void)
{
    //ʹ��FlexBusģ��ʱ��
    SIM_SCGC7 |= SIM_SCGC7_FLEXBUS_MASK;
    //��PTC3������ʹ��FB_CLKOUT����
    PORTC_PCR3 = ( PORT_PCR_MUX(0x5));
}
/*
 * @˵��: pll��ʼ���趨
 * @����: clk_option: ʱ��Ƶ��ѡ��ɺ鶨�����á�
 * @����ֵ: оƬ�ں�ʱ��Ƶ�ʣ�kHz)
 * @����: оƬʹ��PLL ������оƬ�������ʱ�ӣ�оƬ��λ��
 *       MCGĬ�Ͻ���FETģʽ��
 *       �谴�� FEI->FEB->PBE->PEE��˳��������á�
 */
uint32
pll_init(uint8 clk_option)
{
    uint8 pll_freq;
    uint8 prdiv;

    if (clk_option < 111)
        prdiv = 24;
    else if (clk_option < 201)
        prdiv = 12;
    else if (clk_option < 221)
        prdiv = 11;
    else
        return -1;

    //Ĭ�Ͽ�ʼ״̬ΪFEIģʽ���л���FBEģʽ
    MCG_C2 = 0;
    //��ʼ��������ͷ�����״̬��������GPIO
    SIM_SCGC4 |= SIM_SCGC4_LLWU_MASK;
    LLWU_CS |= LLWU_CS_ACKISO_MASK;
    //ѡ���ⲿ����50MHz����Ϊʱ��Դ������FLL��Ƶ������IREFS�����ⲿ����
    // CLKS = 2 FREDIV = 3 IREFS = 0 IRCLKEN = 0 IREFSTEN = 0
    MCG_C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(5);
    //�ȴ��ο�ʱ��״̬λ����
    while (MCG_S & MCG_S_IREFST_MASK); 
    //�ȴ�ʱ��״̬λ��ʾʱ��Դ�����ⲿ�ο�ʱ��
    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2); 

    //�ѽ���FBEģʽ���л���PBEģʽ
    // ���� PLL �ο���Ƶ�� PLLCLKEN= 0 PLLSTEN=0  PRDIV =5
    //����PLL��Ƶ��Ϊ�ⲿʱ��Դ��25��Ƶ���õ�2MHzʱ��
    MCG_C5 = MCG_C5_PRDIV(prdiv); 
    //ȷ��MCG_C6���ڸ�λ״̬����ֹLOLIE��PLL����ʱ�ӿ���������PLL VCO��Ƶ��
    MCG_C6 = 0x0;
    //ѡ��PLL VCO��Ƶ����ϵͳʱ�ӷ�Ƶ��ȡ����ʱ��ѡ��
    switch (clk_option) 
    {
    case PLL_48:
        //����ϵͳ��Ƶ��
        //MCG=PLL, core = MCG, bus = MCG, FlexBus = MCG, Flash clock= MCG/2
        set_sys_dividers(0,0,0,1);
        //����VCO��Ƶ����ʹ��PLLΪ48MHz, LOLIE=0, PLLS=1, CME=0, VDIV=0
        MCG_C6 = MCG_C6_PLLS_MASK; //VDIV = 0 (��Ƶ24��)
        pll_freq = 48;
        break;
    case PLL_50:
        //����ϵͳ��Ƶ��
        //MCG=PLL, core = MCG, bus = MCG, FlexBus = MCG, Flash clock= MCG/2
        set_sys_dividers(0,0,0,1);
        //����VCO��Ƶ����ʹ��PLLΪ50MHz, LOLIE=0, PLLS=1, CME=0, VDIV=1
        MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV(1); //VDIV = 1 (��Ƶ25��)
        pll_freq = 50;
        break;
    case PLL_60:
        // ����ϵͳѡ���Ƶ
        // MCG=PLL, core = MCG, bus = MCG/2, FlexBus = MCG/2, Flash clock= MCG/3
        set_sys_dividers(0, 1, 1, 2); //core=60M, bus=30M, FlexBus=30M, Flash Clk=20M
        // ���� VCO ��Ƶ��ʹ�� 50MHz PLL, LOLIE=0, PLLS=1, CME=0, VDIV=1
        MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV(6); //VDIV = 6 (2x30) 60MHz
        pll_freq = 60;
        break;
    case PLL_70:
        set_sys_dividers(0, 1, 1, 2); //core=70M, bus=35M, FlexBus=35M, Flash Clk=23.3M
        MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV(11); //VDIV = 11 (2x35) 70MHz
        pll_freq = 70;
        break;
    case PLL_80:
        set_sys_dividers(0, 1, 1, 3); //core=80M, bus=40M, FlexBus=40M, Flash Clk=20M
        MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV(16); //VDIV = 16 (2x40) 80MHz
        pll_freq = 80;
        break;
    case PLL_90:
        set_sys_dividers(0, 1, 1, 3); //core=90M, bus=45M, FlexBus=45M, Flash Clk=22.5M
        MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV(21); //VDIV = 21 (2x45) 90MHz
        pll_freq = 90;
        break;
    case PLL_96:
        //����ϵͳ��Ƶ��
        //MCG=PLL, core = MCG, bus = MCG/2, FlexBus = MCG/2, Flash clock= MCG/4
        set_sys_dividers(0,1,1,3);
        //����VCO��Ƶ����ʹ��PLLΪ96MHz, LOLIE=0, PLLS=1, CME=0, VDIV=24
        MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV(24); //VDIV = 24 (��Ƶ48��)
        pll_freq = 96;
        break;
    case PLL_100:
        //����ϵͳ��Ƶ��
        //MCG=PLL, core = MCG, bus = MCG/2, FlexBus = MCG/2, Flash clock= MCG/4
        set_sys_dividers(0,1,1,3);
        //����VCO��Ƶ����ʹ��PLLΪ100MHz, LOLIE=0, PLLS=1, CME=0, VDIV=26
        MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV(26); //VDIV = 26 (��Ƶ50��)
        pll_freq = 100;
        break;
    case PLL_110:
        set_sys_dividers(0, 2, 2, 4); //core=110M, bus=36.7M, FlexBus=36.7M, Flash Clk=22M
        MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV(31); //VDIV = 31 (2x55) 110MHz
        pll_freq = 110;
        break;
    case PLL_120:
        set_sys_dividers(0, 2, 2, 4); //core=120M, bus=40M, FlexBus=40M, Flash Clk=24M
        MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV(7); //VDIV = 7 (3.846x31) 119.2MHz
        pll_freq = 120;
        break;
    case PLL_130:
        set_sys_dividers(0, 2, 2, 5); //core=130M, bus=43M, FlexBus=43M, Flash Clk=21.6M
        MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV(10); //VDIV = 10 (3.846x34) 130.8MHz
        pll_freq = 130;
        break;
    case PLL_150:
        set_sys_dividers(0, 2, 2, 5); //core=150M, bus=50M, FlexBus=50M, Flash Clk=25M
        MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV(15); //VDIV = 15 (3.846x39) 150MHz
        pll_freq = 150;
        break;
    case PLL_170:
        set_sys_dividers(0, 3, 3, 6); //core=170M, bus=42.5M, FlexBus=42.5M, Flash Clk=24.3M
        MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV(20); //VDIV = 20 (3.846x44) 169.2MHz
        pll_freq = 170;
        break;
    case PLL_180:
        set_sys_dividers(0, 3, 3, 7); //core=180M, bus=45M, FlexBus=45M, Flash Clk=22.5M
        MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV(23); //VDIV = 23 (3.846x47) 180.8MHz
        pll_freq = 180;
        break;
    case PLL_200:
        set_sys_dividers(0, 3, 3, 7); //core=200M, bus=50M, FlexBus=50M, Flash Clk=25M
        MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV(28); //VDIV = 28 (3.846x52) 200MHz
        pll_freq = 200;
        break;
    case PLL_209:
        set_sys_dividers(0, 4, 4, 8); //core=209M, bus=42M, FlexBus=42M, Flash Clk=23.2M
        MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV(26); //VDIV = 26 (4.17x50) 208.5MHz
        pll_freq = 209;
        break;
    default:
        return -1;
        break;
    }//end_switch
    while (!(MCG_S & MCG_S_PLLST_MASK)); // �ȴ�PLL�ȶ�
    while (!(MCG_S & MCG_S_LOCK_MASK));  // �ȴ�PLL����

    //�ѽ���PBEģʽ���л���PEEģʽ
    //ͨ������CLKSλ������PEEģʽ
    // CLKS=0, FRDIV=3, IREFS=0, IRCLKEN=0, IREFSTEN=0
    MCG_C1 &= ~MCG_C1_CLKS_MASK;
    //�ȴ�ʱ��״̬λ����
    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x3);

    //��ʱ����PEEģʽ
    return (uint32)(pll_freq*1000);//�����ں�ʱ�ӣ�KHz��
}

/*
 * @˵��: ϵͳ��Ƶ���趨��
 * @����: outdiv1-4: core, bus, FlexBus, FlashԤ��Ƶֵ
 * @����ֵ: ��
 * @����: �˺���������RAM����ִ�У�������������e2448.��Flashʱ�ӷ�Ƶ�ı�
 *          ʱ�������ֹFLASHԤ��ȡ���ܡ���ʱ�ӷ�Ƶ�ı�󣬱�����ʱһ��ʱ��
 *          ���������ʹ��Ԥȡ���ܡ�
 */
__relocate_code__
void 
set_sys_dividers(uint32 outdiv1, uint32 outdiv2, uint32 outdiv3, 
        uint32 outdiv4)
{
    uint32 temp_reg;
    uint8 i;

    //����FMC_PFAPR��ǰ��ֵ
    temp_reg = FMC_PFAPR;

    //ͨ��M&PFD��λM0PFD����ֹԤȡ����
    FMC_PFAPR |= FMC_PFAPR_M7PFD_MASK 
            | FMC_PFAPR_M6PFD_MASK 
            | FMC_PFAPR_M5PFD_MASK
            | FMC_PFAPR_M4PFD_MASK 
            | FMC_PFAPR_M3PFD_MASK 
            | FMC_PFAPR_M2PFD_MASK
            | FMC_PFAPR_M1PFD_MASK 
            | FMC_PFAPR_M0PFD_MASK;

    //��ʱ�ӷ�Ƶ����������ֵ  
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(outdiv1) 
                        						        | SIM_CLKDIV1_OUTDIV2(outdiv2) 
                        						        | SIM_CLKDIV1_OUTDIV3(outdiv3) 
                        						        | SIM_CLKDIV1_OUTDIV4(outdiv4);

    //�ȴ���Ƶ���ı�
    for (i = 0 ; i < outdiv4 ; i++){}

    //�ָ�FMC_PFAPR��ԭʼֵ
    FMC_PFAPR = temp_reg; 
} //set_sys_dividers
