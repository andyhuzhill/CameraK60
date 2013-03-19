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
 * @说明: 系统初始化设定 该函数在 CodeWarrior的启动代码中已经被调用过了，不必重新
 *          调用
 * @参数: 无
 * @返回值: 无
 */
#pragma overload void __init_user();
void
__init_user(void)
{
    //使能IO端口时钟
    SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
            | SIM_SCGC5_PORTB_MASK
            | SIM_SCGC5_PORTC_MASK
            | SIM_SCGC5_PORTD_MASK
            | SIM_SCGC5_PORTE_MASK );

    //开启系统时钟,得到内核/系统时钟, 总线时钟
    core_clk_khz=pll_init(CORE_CLK_MHZ);
    bus_clk_khz = core_clk_khz / (((SIM_CLKDIV1 //外设时钟（总线时钟，KHz）
            & SIM_CLKDIV1_OUTDIV2_MASK) >> 24)+ 1);  
    //使能跟踪时钟，用于调试
    trace_clk_init();   
    //FlexBus时钟初始化
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
 * @说明: 跟踪时钟初始化设定。用于调试
 * @参数: 无
 * @返回值: 无
 */
void
trace_clk_init(void)
{
    //设置跟踪时钟为内核时钟
    SIM_SOPT2 |= SIM_SOPT2_TRACECLKSEL_MASK;    
    //在PTA6引脚上使能TRACE_CLKOUT功能
    PORTA_PCR6 = ( PORT_PCR_MUX(0x7));
}

/*
 * @说明: FlexBus时钟初始化设定
 * @参数: 无
 * @返回值: 无
 */
void 
fb_clk_init(void)
{
    //使能FlexBus模块时钟
    SIM_SCGC7 |= SIM_SCGC7_FLEXBUS_MASK;
    //在PTC3引脚上使能FB_CLKOUT功能
    PORTC_PCR3 = ( PORT_PCR_MUX(0x5));
}
/*
 * @说明: pll初始化设定
 * @参数: clk_option: 时钟频率选项，由洪定义配置。
 * @返回值: 芯片内核时钟频率（kHz)
 * @补充: 芯片使用PLL 来产生芯片所需各种时钟，芯片复位后，
 *       MCG默认进入FET模式。
 *       需按照 FEI->FEB->PBE->PEE的顺序进行配置。
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

    //默认开始状态为FEI模式，切换到FBE模式
    MCG_C2 = 0;
    //初始化晶振后释放锁定状态的振荡器和GPIO
    SIM_SCGC4 |= SIM_SCGC4_LLWU_MASK;
    LLWU_CS |= LLWU_CS_ACKISO_MASK;
    //选择外部晶振（50MHz）作为时钟源，设置FLL分频器，清IREFS启动外部晶振
    // CLKS = 2 FREDIV = 3 IREFS = 0 IRCLKEN = 0 IREFSTEN = 0
    MCG_C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(5);
    //等待参考时钟状态位清零
    while (MCG_S & MCG_S_IREFST_MASK); 
    //等待时钟状态位显示时钟源来自外部参考时钟
    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2); 

    //已进入FBE模式，切换到PBE模式
    // 配置 PLL 参考分频器 PLLCLKEN= 0 PLLSTEN=0  PRDIV =5
    //设置PLL分频，为外部时钟源的25分频，得到2MHz时钟
    MCG_C5 = MCG_C5_PRDIV(prdiv); 
    //确保MCG_C6处于复位状态，禁止LOLIE、PLL、和时钟控制器，清PLL VCO分频器
    MCG_C6 = 0x0;
    //选择PLL VCO分频器，系统时钟分频器取决于时钟选项
    switch (clk_option) 
    {
    case PLL_48:
        //设置系统分频器
        //MCG=PLL, core = MCG, bus = MCG, FlexBus = MCG, Flash clock= MCG/2
        set_sys_dividers(0,0,0,1);
        //设置VCO分频器，使能PLL为48MHz, LOLIE=0, PLLS=1, CME=0, VDIV=0
        MCG_C6 = MCG_C6_PLLS_MASK; //VDIV = 0 (倍频24倍)
        pll_freq = 48;
        break;
    case PLL_50:
        //设置系统分频器
        //MCG=PLL, core = MCG, bus = MCG, FlexBus = MCG, Flash clock= MCG/2
        set_sys_dividers(0,0,0,1);
        //设置VCO分频器，使能PLL为50MHz, LOLIE=0, PLLS=1, CME=0, VDIV=1
        MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV(1); //VDIV = 1 (倍频25倍)
        pll_freq = 50;
        break;
    case PLL_60:
        // 设置系统选项分频
        // MCG=PLL, core = MCG, bus = MCG/2, FlexBus = MCG/2, Flash clock= MCG/3
        set_sys_dividers(0, 1, 1, 2); //core=60M, bus=30M, FlexBus=30M, Flash Clk=20M
        // 设置 VCO 分频并使能 50MHz PLL, LOLIE=0, PLLS=1, CME=0, VDIV=1
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
        //设置系统分频器
        //MCG=PLL, core = MCG, bus = MCG/2, FlexBus = MCG/2, Flash clock= MCG/4
        set_sys_dividers(0,1,1,3);
        //设置VCO分频器，使能PLL为96MHz, LOLIE=0, PLLS=1, CME=0, VDIV=24
        MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV(24); //VDIV = 24 (倍频48倍)
        pll_freq = 96;
        break;
    case PLL_100:
        //设置系统分频器
        //MCG=PLL, core = MCG, bus = MCG/2, FlexBus = MCG/2, Flash clock= MCG/4
        set_sys_dividers(0,1,1,3);
        //设置VCO分频器，使能PLL为100MHz, LOLIE=0, PLLS=1, CME=0, VDIV=26
        MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV(26); //VDIV = 26 (倍频50倍)
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
    while (!(MCG_S & MCG_S_PLLST_MASK)); // 等待PLL稳定
    while (!(MCG_S & MCG_S_LOCK_MASK));  // 等待PLL锁定

    //已进入PBE模式，切换到PEE模式
    //通过清零CLKS位来进入PEE模式
    // CLKS=0, FRDIV=3, IREFS=0, IRCLKEN=0, IREFSTEN=0
    MCG_C1 &= ~MCG_C1_CLKS_MASK;
    //等待时钟状态位更新
    while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x3);

    //此时进入PEE模式
    return (uint32)(pll_freq*1000);//返回内核时钟（KHz）
}

/*
 * @说明: 系统分频器设定。
 * @参数: outdiv1-4: core, bus, FlexBus, Flash预分频值
 * @返回值: 无
 * @补充: 此函数必须在RAM里面执行，否则会产生错误e2448.当Flash时钟分频改变
 *          时，必须禁止FLASH预存取功能。在时钟分频改变后，必须延时一段时间
 *          后才能重新使能预取功能。
 */
__relocate_code__
void 
set_sys_dividers(uint32 outdiv1, uint32 outdiv2, uint32 outdiv3, 
        uint32 outdiv4)
{
    uint32 temp_reg;
    uint8 i;

    //保存FMC_PFAPR当前的值
    temp_reg = FMC_PFAPR;

    //通过M&PFD置位M0PFD来禁止预取功能
    FMC_PFAPR |= FMC_PFAPR_M7PFD_MASK 
            | FMC_PFAPR_M6PFD_MASK 
            | FMC_PFAPR_M5PFD_MASK
            | FMC_PFAPR_M4PFD_MASK 
            | FMC_PFAPR_M3PFD_MASK 
            | FMC_PFAPR_M2PFD_MASK
            | FMC_PFAPR_M1PFD_MASK 
            | FMC_PFAPR_M0PFD_MASK;

    //给时钟分频器设置期望值  
    SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(outdiv1) 
                        						        | SIM_CLKDIV1_OUTDIV2(outdiv2) 
                        						        | SIM_CLKDIV1_OUTDIV3(outdiv3) 
                        						        | SIM_CLKDIV1_OUTDIV4(outdiv4);

    //等待分频器改变
    for (i = 0 ; i < outdiv4 ; i++){}

    //恢复FMC_PFAPR的原始值
    FMC_PFAPR = temp_reg; 
} //set_sys_dividers
