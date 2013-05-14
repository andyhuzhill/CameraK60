#ifndef __OV7725_H
#define __OV7725_H 

#define	CAMERA_DMA_CH 	    DMA_CH0		    //��������ͷ��DMA�ɼ�ͨ��
#define CAMERA_W            80           	//��������ͷͼ����
#define CAMERA_H            60				//��������ͷͼ��߶�
#define CAMERA_INTERLACE    1              	//����ͷ����ɼ����� n - 1,����1��ʾ�����вɼ���2��ʾ���вɼ�

#define CAMERA_DMA_NUM      (CAMERA_W /8)    //DMA�ɼ�����
#define CAMERA_SIZE         (CAMERA_W * CAMERA_H /8)        //ͼ��ռ�ÿռ��С


#define CAMERA_DMA_LINE     (CAMERA_H/CAMERA_INTERLACE)     //ʵ�ʲɼ�����

extern   uint8 *	    IMG_BUFF;       //ͼ�񻺳���ָ��

//����ͼ��ɼ�״̬
typedef enum 
{
    IMG_NOTINIT =0,
    IMG_READY   =1,              //׼���ɼ�ͼ��
    IMG_START   =2,               //��ʼ�ɼ�ͼ��
    IMG_GATHER  =3,             //ͼ��ɼ���
    IMG_FINISH  =4,			//ͼ��ɼ����
    IMG_PROCESS =5,            //���ڴ���ͼ��
    IMG_FAIL    =6,  				//ͼ��ɼ�ʧ��(�ɼ���������)
    IMG_STOP    =7,				//��ֹͼ��ɼ�

}IMG_STATE;

typedef struct
{
    uint8 Address;			       /*�Ĵ�����ַ*/
    uint8 Value;		           /*�Ĵ���ֵ*/
}Register_Info;

extern	uint8   ov7725_init(uint8 *imgaddr);
extern	void    ov7725_exti_init();
extern	void    ov7725_get_img();

#endif























