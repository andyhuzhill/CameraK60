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
 * TODO: �������뺯��
 * TODO: ʹ��assert���к����������
 */

#include "k60_gpio.h"

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
    case PORT_A:
        p = PORTA_BASE_PTR;
        break;
    case PORT_B:
        p = PORTB_BASE_PTR;
        break;
    case PORT_C:
        p = PORTC_BASE_PTR;
        break;
    case PORT_D:
        p = PORTD_BASE_PTR;
        break;
    case PORT_E:
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
    case PORT_A:
        p = PTA_BASE_PTR;
        break;
    case PORT_B:
        p = PTB_BASE_PTR;
        break;
    case PORT_C:
        p = PTC_BASE_PTR;
        break;
    case PORT_D:
        p = PTD_BASE_PTR;
        break;
    case PORT_E:
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

#if 0
/*
 * @˵��: ��ʼ��GPIO��
 * @����:  port: �˿��� PORT_A~PORT_E
 *  		pin: ���� ȡֵ1 ~ 31
 * 		    dir: ����������� 
 * 		  state: ��ʼ״̬
 * @����ֵ: 0 �������أ� ����ֵΪ�쳣
 */
uint8 
GPIO_init(GPIO_TypeDef port, uint8 pin, GPIOMode_TypeDef dir, GPIOState_TypeDef state)
{
    GPIO_MemMapPtr pt = GPIO_get_pt_addr(port);
    PORT_MemMapPtr p  = GPIO_get_port_addr(port);
    
    if(!pt) return -1;
    if(!p) return -1;
    
    //ѡ��IO��ʱ�ӿ�������
    switch(port)
    {
    case PORT_A:
        SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
        break;
    case PORT_B:
        SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
        break;
    case PORT_C:
        SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
        break;
    case PORT_D:
        SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
        break;
    case PORT_E:
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
        if(State_High == state)
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
#endif

/*
 * @˵��: ST����ʼ��GPIO�� :)
 * @����:  port: �˿���  PORT_A~PORT_E
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
    case PORT_A:
        SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;
        break;
    case PORT_B:
        SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
        break;
    case PORT_C:
        SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;
        break;
    case PORT_D:
        SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
        break;
    case PORT_E:
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
    	currentpin = (InitStruct->PIN) & pos;
    	
    	if(currentpin == pos)
    	{
    		//��������ΪGPIO����
    		PORT_PCR_REG(p,pinpos) = (0 |PORT_PCR_MUX(1));
    		
    		if(Mode_OUT == InitStruct->MODE)
    		{
    			BSET(GPIO_PDDR_REG(pt), pinpos);
    			if (State_High == InitStruct->STATE)
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
    			if (State_PullUp == InitStruct->STATE)
    			{
    				PORT_PCR_REG(p, pinpos) |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
    			}else if (State_PullDown == InitStruct->STATE)
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
 * @����: port: �˿��� PORT_A~PORT_E
 * 		   pin: ����
 * 		 state: ״̬
 * @����ֵ: ��
 */		  
void
GPIO_write_bit(GPIO_TypeDef port, uint8 pin, GPIOState_TypeDef state)
{
    GPIO_MemMapPtr pt = GPIO_get_pt_addr(port);
    
    if(State_High == state)
        BCLR(GPIO_PDOR_REG(pt), pin);
    else
        BSET(GPIO_PDOR_REG(pt), pin);
}

/*
 * @˵��: �趨ָ���˿�״̬
 * @����: port: �˿��� PORT_A~PORT_E
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
 * @����: port:  �˿��� PORT_A~PORT_E
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
 * @����: port: �˿��� PORT_A~PORT_E
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
 * @����: port: �˿��� PORT_A~PORT_E
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
 * @����: port: �˿��� PORT_A~PORT_E
 * @����ֵ: ��
 */
void
GPIO_toggle(GPIO_TypeDef port)
{
    GPIO_MemMapPtr pt = GPIO_get_pt_addr(port);
    
    GPIO_PTOR_REG(pt) = ~(0x0u);
}