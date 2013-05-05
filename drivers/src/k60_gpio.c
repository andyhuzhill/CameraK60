/*
 * k60_gpio.c
 *
 *  Created on: Nov 21, 2012
 *      Author: scout
 *      E-mail: andyhuzhill@gmail.com
 *    HomePage: http://andyhuzhill.github.com
 *
 * Description: GPIO��������
 *    Compiler: CodeWarrior v10.2
 * 	   version: 1.0
 * 
 */

#include  "k60_gpio.h"


volatile struct GPIO_MemMap *GPIOx[5] = {PTA_BASE_PTR, PTB_BASE_PTR, PTC_BASE_PTR, PTD_BASE_PTR, PTE_BASE_PTR}; //�������ָ�����鱣�� GPIOx �ĵ�ַ
volatile struct PORT_MemMap *PORTX[5] = {PORTA_BASE_PTR, PORTB_BASE_PTR, PORTC_BASE_PTR, PORTD_BASE_PTR, PORTE_BASE_PTR};


///////////////////////////////////////////////////////////////////////////////
//            �ڲ����ú���ʵ��
///////////////////////////////////////////////////////////////////////////////

/*
 * @˵��: ��ö˿ںŶ�Ӧ�˿ڼĴ�����ַָ��
 * @����: port: �˿���
 * @����ֵ: �˿ڶ�Ӧ�Ķ˿ڼĴ�����ַָ��
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
 * @˵��: ��ö˿ںŶ�Ӧ��GPIO�˿ڼĴ�����ַָ��
 * @����: port: �˿ں�
 * @����ֵ: GPIO�˿ڼĴ�����ַָ��
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
//              �ⲿ�ӿں���ʵ��
///////////////////////////////////////////////////////////////////////////////

/*
 * @˵��: ��ʼ��GPIO��
 * @����:  port: �˿��� PORT_A~GPIO_E
 *  		pin: ���� ȡֵ1 ~ 31
 * 		    dir: ����������� 
 * 		  state: ��ʼ״̬
 * @����ֵ: 0 �������أ� ����ֵΪ�쳣
 */
uint8 
gpio_init(GPIO_TypeDef port, uint8 pin, GPIOMode_TypeDef dir, GPIOState_TypeDef state)
{
    GPIO_MemMapPtr pt = GPIO_get_pt_addr(port);
    PORT_MemMapPtr p  = GPIO_get_port_addr(port);
    
    if(!pt) return -1;
    if(!p) return -1;
    
    //ѡ��IO��ʱ�ӿ�������
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
    
    //�趨ͨ�ö˿����ſ��ƼĴ���ֵ���趨ΪGPIO����
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
//    //ѡ���ܽ� PORTx_PCRx ��ÿ���˿ڶ��и��Ĵ��� PORTx_PCRx
//    PORT_PCR_REG(PORTX[portx], n) = (0 | PORT_PCR_MUX(1) | cfg);
//
//    //�˿ڷ���������뻹�����
//    if( ( (cfg & 0x01) == GPI) || (cfg == GPI_UP) ||     (cfg == GPI_UP_PF)      )
//        //   ���λΪ0������   ||   ��������ģʽ  ||   ��������������Դ�˲���
//    {
//        GPIO_PDDR_REG(GPIOx[portx]) &= ~(1 << n);  //���ö˿ڷ���Ϊ����
//    }
//    else
//    {
//        GPIO_PDDR_REG(GPIOx[portx]) |= (1 << n);    //���ö˿ڷ���Ϊ���
//        if(data == 1)//output
//        {
//            GPIO_PSOR_REG(GPIOx[portx]) |= (1 << n);                 //�Զ˿�������ƣ����Ϊ1
//        }
//        else
//        {
//            GPIO_PCOR_REG(GPIOx[portx]) |= (1 << n);                 //�Զ˿�������ƣ����Ϊ0
//        }
//    }
//}

/*
 * @˵��: ST����ʼ��GPIO�� :)
 * @����:  port: �˿���  PORT_A~GPIO_E
 *  	   init: ��ʼ�����ýṹ��
 * @����ֵ: 0 �������أ� ����ֵΪ�쳣
 */
uint8 
GPIO_init(GPIO_TypeDef port, GPIO_InitTypeDef *InitStruct)
{
	uint32 pinpos = 0x00, pos = 0x00, currentpin = 0x00;
    GPIO_MemMapPtr pt = GPIO_get_pt_addr(port);
    PORT_MemMapPtr p  = GPIO_get_port_addr(port);
    
    // ������
    if(!pt) return -1;
    if(!p) return -1;
    
    //ѡ��IO��ʱ�ӿ�������
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
    	//�õ�����λ��
    	currentpin = (InitStruct->Pin) & pos;
    	
    	if(currentpin == pos)
    	{
    		//��������ΪGPIO����
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
 * @˵��: �趨ָ���˿�����״̬
 * @����: port: �˿��� PORT_A~GPIO_E
 * 		   pin: ����
 * 		 state: ״̬
 * @����ֵ: ��
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
 * @˵��: �趨ָ���˿�״̬
 * @����: port: �˿��� PORT_A~GPIO_E
 * 		  byte: ״̬
 * @����ֵ: ��
 */
void
GPIO_write(GPIO_TypeDef port, uint32 byte)
{
    GPIO_MemMapPtr pt = GPIO_get_pt_addr(port);
    
    GPIO_PDOR_REG(pt) = byte;
}

/*
 * @˵��: �õ�ָ���˿�����״̬
 * @����: port:  �˿��� PORT_A~GPIO_E
 * 	       pin:  ���� 1~31
 * @����ֵ ָ������״̬ 0 = �͵�ƽ 1 = �ߵ�ƽ
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
 * @˵��: �õ�ָ���˿�״̬
 * @����: port: �˿��� PORT_A~GPIO_E
 * @����ֵ: ָ���˿�״̬ 32λ�޷�����
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
 * @˵��: �˿�ָ������״̬��ת
 * @����: port: �˿��� PORT_A~GPIO_E
 * 		   pin: ���� 1~31
 * @����ֵ: ��
 */
void
GPIO_toggle_bit(GPIO_TypeDef port, uint8 pin)
{
    GPIO_MemMapPtr pt = GPIO_get_pt_addr(port);
    
    BSET(GPIO_PTOR_REG(pt), pin);
}

/*
 * @˵��: �˿�״̬��ת port = ~port;
 * @����: port: �˿��� PORT_A~GPIO_E
 * @����ֵ: ��
 */
void
GPIO_toggle(GPIO_TypeDef port)
{
    GPIO_MemMapPtr pt = GPIO_get_pt_addr(port);
    
    GPIO_PTOR_REG(pt) = ~(0x0u);
}