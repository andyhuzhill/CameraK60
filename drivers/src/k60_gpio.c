/*
 * k60_gpio.c
 *
 *  Created on: Nov 21, 2012
 *      Author: scout
 *      E-mail: andyhuzhill@gmail.com
 *    HomePage: http://andyhuzhill.github.com
 *
 * Description: GPIO驱动程序
 *    Compiler: CodeWarrior v10.2
 * 	   version: 1.0
 * 
 */

#include  "k60_gpio.h"


volatile struct GPIO_MemMap *GPIOx[5] = {PTA_BASE_PTR, PTB_BASE_PTR, PTC_BASE_PTR, PTD_BASE_PTR, PTE_BASE_PTR}; //定义五个指针数组保存 GPIOx 的地址
volatile struct PORT_MemMap *PORTX[5] = {PORTA_BASE_PTR, PORTB_BASE_PTR, PORTC_BASE_PTR, PORTD_BASE_PTR, PORTE_BASE_PTR};


///////////////////////////////////////////////////////////////////////////////
//            内部调用函数实现
///////////////////////////////////////////////////////////////////////////////

/*
 * @说明: 获得端口号对应端口寄存器地址指针
 * @参数: port: 端口名
 * @返回值: 端口对应的端口寄存器地址指针
 */
static PORT_MemMapPtr 
GPIO_get_port_addr(GPIO_TypeDef port)
{
    PORT_MemMapPtr p;
    switch(port)
    {
    case GPIO_A:
        p = PORTA_BASE_PTR;
        break;
    case GPIO_B:
        p = PORTB_BASE_PTR;
        break;
    case GPIO_C:
        p = PORTC_BASE_PTR;
        break;
    case GPIO_D:
        p = PORTD_BASE_PTR;
        break;
    case GPIO_E:
        p = PORTE_BASE_PTR;
        break;
    default:
        return 0;
    }
    return p;
}

/*
 * @说明: 获得端口号对应的GPIO端口寄存器地址指针
 * @参数: port: 端口号
 * @返回值: GPIO端口寄存器地址指针
 */
static GPIO_MemMapPtr 
GPIO_get_pt_addr(GPIO_TypeDef port)
{
    GPIO_MemMapPtr p;
    
    switch(port)
    {
    case GPIO_A:
        p = PTA_BASE_PTR;
        break;
    case GPIO_B:
        p = PTB_BASE_PTR;
        break;
    case GPIO_C:
        p = PTC_BASE_PTR;
        break;
    case GPIO_D:
        p = PTD_BASE_PTR;
        break;
    case GPIO_E:
        p = PTE_BASE_PTR;
        break;
    default:
        return 0;
        break;
    }
    return p;
}

///////////////////////////////////////////////////////////////////////////////
//              外部接口函数实现
///////////////////////////////////////////////////////////////////////////////

/*
 * @说明: 初始化GPIO口
 * @参数:  port: 端口名 PORT_A~GPIO_E
 *  		pin: 引脚 取值1 ~ 31
 * 		    dir: 输入输出方向 
 * 		  state: 初始状态
 * @返回值: 0 正常返回， 其他值为异常
 */
uint8 
gpio_init(GPIO_TypeDef port, uint8 pin, GPIOMode_TypeDef dir, GPIOState_TypeDef state)
{
    GPIO_MemMapPtr pt = GPIO_get_pt_addr(port);
    PORT_MemMapPtr p  = GPIO_get_port_addr(port);
    
    if(!pt) return -1;
    if(!p) return -1;
    
    //选择IO口时钟控制掩码
    switch(port)
    {
    case GPIO_A:
        SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
        break;
    case GPIO_B:
        SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
        break;
    case GPIO_C:
        SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
        break;
    case GPIO_D:
        SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
        break;
    case GPIO_E:
        SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
        break;
    default:
        return -1;
        break;
    }
    
    //设定通用端口引脚控制寄存器值，设定为GPIO功能
    PORT_PCR_REG(p, pin) = (0| PORT_PCR_MUX(1));
    
    if(Mode_OUT == dir)
    {
        GPIO_PDDR_REG(pt) |= 0x1u << pin;
        if(High == state)
        {
            BSET(GPIO_PDOR_REG(pt), pin);
        }
        else
        {
            BCLR(GPIO_PDOR_REG(pt), pin);
        }
    }
    else
    {
        BCLR(GPIO_PDDR_REG(pt), pin);
    }
    return 0;
}

//
//void gpio_init (PORTx portx, uint8 n, GPIO_CFG cfg, uint8 data)
//{
//    //选择功能脚 PORTx_PCRx ，每个端口都有个寄存器 PORTx_PCRx
//    PORT_PCR_REG(PORTX[portx], n) = (0 | PORT_PCR_MUX(1) | cfg);
//
//    //端口方向控制输入还是输出
//    if( ( (cfg & 0x01) == GPI) || (cfg == GPI_UP) ||     (cfg == GPI_UP_PF)      )
//        //   最低位为0则输入   ||   输入上拉模式  ||   输入上拉，带无源滤波器
//    {
//        GPIO_PDDR_REG(GPIOx[portx]) &= ~(1 << n);  //设置端口方向为输入
//    }
//    else
//    {
//        GPIO_PDDR_REG(GPIOx[portx]) |= (1 << n);    //设置端口方向为输出
//        if(data == 1)//output
//        {
//            GPIO_PSOR_REG(GPIOx[portx]) |= (1 << n);                 //对端口输出控制，输出为1
//        }
//        else
//        {
//            GPIO_PCOR_REG(GPIOx[portx]) |= (1 << n);                 //对端口输出控制，输出为0
//        }
//    }
//}

/*
 * @说明: ST风格初始化GPIO口 :)
 * @参数:  port: 端口名  PORT_A~GPIO_E
 *  	   init: 初始化配置结构体
 * @返回值: 0 正常返回， 其他值为异常
 */
uint8 
GPIO_init(GPIO_TypeDef port, GPIO_InitTypeDef *InitStruct)
{
	uint32 pinpos = 0x00, pos = 0x00, currentpin = 0x00;
    GPIO_MemMapPtr pt = GPIO_get_pt_addr(port);
    PORT_MemMapPtr p  = GPIO_get_port_addr(port);
    
    // 检查参数
    if(!pt) return -1;
    if(!p) return -1;
    
    //选择IO口时钟控制掩码
    switch(port)
    {
    case GPIO_A:
        SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
        break;
    case GPIO_B:
        SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
        break;
    case GPIO_C:
        SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
        break;
    case GPIO_D:
        SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
        break;
    case GPIO_E:
        SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
        break;
        default:
        return -1;
        break;
    }
    
    for(pinpos = 0x00; pinpos < 0x20; pinpos++)
    {
    	pos = ((uint32)0x01) << pinpos;
    	//得到引脚位置
    	currentpin = (InitStruct->Pin) & pos;
    	
    	if(currentpin == pos)
    	{
    		//配置引脚为GPIO功能
    		PORT_PCR_REG(p,pinpos) = (0 |PORT_PCR_MUX(1));
    		
    		if(Mode_OUT == InitStruct->MODE)
    		{
    			BSET(GPIO_PDDR_REG(pt), pinpos);
    			if (High == InitStruct->STATE)
    			{
    				BSET(GPIO_PDOR_REG(pt), pinpos);
    			}else
    			{
    				BCLR(GPIO_PDOR_REG(pt), pinpos);
    			}// end of state
    		}else // Mode_In
    		{
    			BCLR(GPIO_PDOR_REG(pt), pinpos);
    			if (InitStruct->IRQC > 0xc) return -1;
    			PORT_PCR_REG(p, pinpos) |= PORT_PCR_IRQC(InitStruct->IRQC);
    			if (PullUp == InitStruct->STATE)
    			{
    				PORT_PCR_REG(p, pinpos) |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
    			}else if (PullDown == InitStruct->STATE)
    			{
    				PORT_PCR_REG(p, pinpos) |= PORT_PCR_PE_MASK;
    			}else
    			{
    				PORT_PCR_REG(p, pinpos) &= ~PORT_PCR_PE_MASK;
    			}
    		} //end of Mode
    	} // end of pin
    }// end of for
    return 0;
}

/*
 * @说明: 设定指定端口引脚状态
 * @参数: port: 端口名 PORT_A~GPIO_E
 * 		   pin: 引脚
 * 		 state: 状态
 * @返回值: 无
 */		  
void
GPIO_write_bit(GPIO_TypeDef port, uint8 pin, GPIOState_TypeDef state)
{
    GPIO_MemMapPtr pt = GPIO_get_pt_addr(port);
    
    if(High == state)
        BCLR(GPIO_PDOR_REG(pt), pin);
    else
        BSET(GPIO_PDOR_REG(pt), pin);
}

/*
 * @说明: 设定指定端口状态
 * @参数: port: 端口名 PORT_A~GPIO_E
 * 		  byte: 状态
 * @返回值: 无
 */
void
GPIO_write(GPIO_TypeDef port, uint32 byte)
{
    GPIO_MemMapPtr pt = GPIO_get_pt_addr(port);
    
    GPIO_PDOR_REG(pt) = byte;
}

/*
 * @说明: 得到指定端口引脚状态
 * @参数: port:  端口名 PORT_A~GPIO_E
 * 	       pin:  引脚 1~31
 * @返回值 指定引脚状态 0 = 低电平 1 = 高电平
 */
uint8
GPIO_read_bit(GPIO_TypeDef port, uint8 pin)
{
    uint8 state;
    GPIO_MemMapPtr pt = GPIO_get_pt_addr(port);
    if(!pt) return -1;
    
    state = BGET(GPIO_PDIR_REG(pt), pin);
    return state;
}

/*
 * @说明: 得到指定端口状态
 * @参数: port: 端口名 PORT_A~GPIO_E
 * @返回值: 指定端口状态 32位无符号数
 */
uint32
GPIO_read(GPIO_TypeDef port)
{
    uint32 state;
    GPIO_MemMapPtr pt = GPIO_get_pt_addr(port);
    if(!pt) return -1;
    
    state = GPIO_PDIR_REG(pt);
    return state;  
}

/*
 * @说明: 端口指定引脚状态翻转
 * @参数: port: 端口名 PORT_A~GPIO_E
 * 		   pin: 引脚 1~31
 * @返回值: 无
 */
void
GPIO_toggle_bit(GPIO_TypeDef port, uint8 pin)
{
    GPIO_MemMapPtr pt = GPIO_get_pt_addr(port);
    
    BSET(GPIO_PTOR_REG(pt), pin);
}

/*
 * @说明: 端口状态翻转 port = ~port;
 * @参数: port: 端口名 PORT_A~GPIO_E
 * @返回值: 无
 */
void
GPIO_toggle(GPIO_TypeDef port)
{
    GPIO_MemMapPtr pt = GPIO_get_pt_addr(port);
    
    GPIO_PTOR_REG(pt) = ~(0x0u);
}