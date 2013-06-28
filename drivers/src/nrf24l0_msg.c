#include "common.h"
#include "k60_gpio.h"
#include "port.h"
#include "spi.h"
#include "ov7725.h"
#include "nrf24l0.h"
#include "nrf24l0_msg.h"

uint32 rxbuflen = 0;           //���ڽ��շ����ؽ��յ��������ݡ���������һ�δ��ݽ�ȥ���Ǹ�����С��



static nrf_result_e  tx_com_img(uint8 * sendbuf);
static nrf_result_e  tx_com_event_query(uint8 * sendbuf);

static nrf_result_e  rx_com_img(uint8 * sendbuf);
static nrf_result_e  rx_com_event_query(uint8 * sendbuf);


typedef	nrf_result_e (* NRF_PFUNC)(uint8 *);               //���庯��ָ�룬���ڱ���������Ƶ�

//��������ִ�к����ṹ��
typedef	struct
{
	NRF_PFUNC	img_func;                   //����ͼ��
	NRF_PFUNC	event_query;                //�¼���ѯ
}com_func_t;



//Receive Command Vector Table  Һ�����Ʋ��ִ�����
NRF_PFUNC rx_com_table[COM_MAX] =
{	
    rx_com_img,	
    rx_com_event_query
};


//Transfer Command Vector Table ����ͷ���ִ�����
NRF_PFUNC tx_com_table[COM_MAX] =
{
	tx_com_img,
	tx_com_event_query
};

//�������ǰ�����ֽ���������ʶ�𣬺��������Ҫ���������
nrf_result_e NRF_MSG_send(crtl_e  com,uint8 * sendbuf)
{
    nrf_result_e result;
    ASSERT(com < COM_MAX);          //����ܳ���������Ŀ
    
    //��ǰ����д�����ݻ�����
    sendbuf[0]= com;
    sendbuf[1]= ~com;
    
    //ִ�ж�Ӧ�ķ��ͺ���
    result = (tx_com_table[(uint8)com] )(sendbuf);  //��Ӧ�ķ��ͺ���Ҫע��ǰ�������ֽڲ�����
    
    return result;
}

//���ͷ������Ӧ�ĺ���

//����ͼ��
nrf_result_e  tx_com_img(uint8 * sendbuf)
{
    //��Ϊֱ�Ӱ�ͼ��ɼ������������ﲻ��Ҫ����ͼ�񣬽����������ݾͺ�
    sendbuf[CAMERA_SIZE + 2*COM_LEN -2]=(uint8)~COM_IMG;         //��β��д�����������
    sendbuf[CAMERA_SIZE + 2*COM_LEN -1]=(uint8)COM_IMG;
    
    NRF_ISR_Tx_Dat(sendbuf, CAMERA_SIZE + 2*COM_LEN );      //�жϷ�ʽ��������  
    
    return NRF_RESULT_NULL;
}

//��ѯ�¼����ݲ���
nrf_result_e  tx_com_event_query(uint8 * sendbuf)
{
    return NRF_RESULT_NULL;
}



//���շ�

//��ѯ�Ƿ��н������ݣ������д���sendbuf Ϊ�ɹ�ʹ�õĻ�����
nrf_result_e NRF_MSG_receive(crtl_e  *com,uint8 * rebuf)
{
    nrf_result_e result;
    uint8  status;	//�����жϽ���/����״̬
    uint8  flag = 0;   //�����յ��������󣬾ͱ��Ϊ1
    uint8  rxflag  = 0;//����Ƿ��н��յ�����
    crtl_e  comtmp;
    do
    {
        flag = 0;
        status = NRF_ISR_Rx_Dat(rebuf); //�жϽ�������
        if(status != RX_DR) 
        {
            //û�н��յ�����
            if(rxflag)
            {
                //�н��յ����ݣ�������������Ч
                return NRF_RESULT_RX_NOVALID;
            }
            else
            {
                return NRF_RESULT_RX_NO;
            }
        }
        rxflag = 1;
        comtmp = rebuf[0];
        if((comtmp < COM_MAX) && (rebuf[1] ==  (uint8)~comtmp) )
        {
            flag = 1;
        }
    }while(flag==0);           //��com�����ʱ�򣬲�����ִ��
    
    *com = comtmp;

    //ִ�ж�Ӧ�ķ��ͺ���
    result = ((rx_com_table[comtmp]))(rebuf);  //��Ӧ�ķ��ͺ���Ҫע��ǰ�������ֽڲ�����
    
    return result;
}

//����ͼ��
nrf_result_e rx_com_img(uint8 * rebuf)
{
    //��Ϊ��ʶ���Ӧ��������ִ�У���Ҫ��ͼ��ɼ��������˳�
    //���ڵ�һ���Ѿ����յ����Ż���ô˺�����������Ҫ�����ɼ�ʣ���ͼ��
    
    uint32 len = (CAMERA_SIZE + 2*COM_LEN -1)/MAX_ONCE_TX_NUM ;
    uint8  status;	//�����жϽ���/����״̬
    uint8  *buftemp = rebuf;
    
    rxbuflen = CAMERA_SIZE + 2*COM_LEN; //֪ͨ������յ����ݳ���
    
    do
    {
        rebuf += MAX_ONCE_TX_NUM;         //�ƶ�����һ���������ݽ�����ʱ��û�۵��Ǹ�����
        
        //��Ҫ��ȡ len ���������� rebuf + MAX_ONCE_TX_NUM ��ַ����������
        do{
            status = NRF_ISR_Rx_Dat(rebuf); //�жϽ�������
        }while(status != RX_DR);                //�ȴ����յ�����
        
        len--;
        
    }while(len);
    
    //У��β�������Ƿ���ȷ
    if( 
            (buftemp[CAMERA_SIZE + 2*COM_LEN -2] ==   (uint8)~COM_IMG)
     &&     (buftemp[CAMERA_SIZE + 2*COM_LEN -1] ==   (uint8)COM_IMG)        )
    {
        return NRF_RESULT_RX_VALID;
    }
    
    return NRF_RESULT_RX_NOVALID;
    
}

//�¼���ѯ
nrf_result_e rx_com_event_query(uint8 * rebuf)
{
    
    return NRF_RESULT_NULL;
}