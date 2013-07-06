#ifndef _NRF24L0_MSG_H_
#define	_NRF24L0_MSG_H_

#define COM_LEN     2   //com����ռ�õ��ֽ�
#define IS_CAM      1   //�Ƿ�����ͷģ��,1��ʾ������ͷ��������ͼ��0��ʾҺ��ģ�飬������ʾ�ͷ��ؿ����źš�



//��������COMִ�к������ؽ��
//COM�����Ҫִ�ж�Ӧ�ĺ���
//MSG��Ϣ�����ͷ�����֪ͨ������Ҫִ���κκ���
typedef	enum
{
	NRF_RESULT_FALSE=0,					//�١�ʧ�ܡ��¼�û����
	NRF_RESULT_TRUE	=1,					//�桢�ɹ����¼�������
	NRF_RESULT_QUIT,					//�˳�
	
    NRF_RESULT_NULL,                    //�������κδ���
    
    //��������
    NRF_RESULT_RX_NO,                   //û���յ�����
	NRF_RESULT_RX_VALID,            	//������Ч����
    NRF_RESULT_RX_NOVALID,              //���յ����ݣ�����Ч
    

	/*** EVENT �¼� ***/
//	NRF_RESULT_EVENT_FALSE,				//û�¼������������¼���������
//	NRF_RESULT_EVENT_FINIST,			//�¼��������
//	NRF_RESULT_EVENT_CONTINUE,			//�¼��������䣨����¼������ˣ���Ҫ�������䣩	
    
}nrf_result_e;

//����������Ϣ����
typedef	enum
{
	////////////////// COM���� /////////////////////
	//��ҪУ����յ������Ƿ���ȷ
	//���͵����ݣ� XXX_COM ��~XXX_COM    �������������Ҫ���͵���Ϣ���ɶ�Ӧ��ִ�к������д���

	//ͼ�������
	COM_IMG,		    //����ͼ������:��������󣬾�ֱ�ӽ�ͼ������
	
	//�¼�����
	COM_EVENT_QUERY,	//�����¼���Ϣ��ѯ������֪ͨ�Է��Ƿ����¼�������
    //��Ҫ��Ӷ��ȷ��
    //COM_EVENT_REPLY,	//�����¼���Ϣ��Ӧ���Է�����֪ͨ���ҷ����л�Ӧ��
    
	/*  ��Ҫ��ӹ��ܣ�������λ��  */

	
	COM_MAX		,		//������������Ŀ													********************************
	
}crtl_e;



#define   NRF_MSG_send_state()			NRF_ISR_Tx_State()		//��ѯ����״̬
nrf_result_e NRF_MSG_send(crtl_e  com,uint8 * sendbuf);    		//�������ݣ����ͳ�����com��Ӧ�ĺ�������
nrf_result_e NRF_MSG_receive(crtl_e  *com,uint8 * rebuf);   		//��������,�����д���

#endif	//_NRF24L0_MSG_H_