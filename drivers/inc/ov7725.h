#ifndef __OV7725_H
#define __OV7725_H 


#define	CAMERA_DMA_CH 	    DMA_CH0		    //��������ͷ��DMA�ɼ�ͨ��
#define CAMERA_W            320           	//��������ͷͼ����
#define CAMERA_H            240				//��������ͷͼ��߶�
#define CAMERA_INTERLACE    1              	//����ͷ����ɼ����� n - 1,����1��ʾ�����вɼ���2��ʾ���вɼ�

#define CAMERA_DMA_NUM      (CAMERA_W /8)    //DMA�ɼ�����
#define CAMERA_SIZE         (CAMERA_W * CAMERA_H /8)        //ͼ��ռ�ÿռ��С


#define CAMERA_DMA_LINE     (CAMERA_H/CAMERA_INTERLACE)     //ʵ�ʲɼ�����


extern   uint8 *	    IMG_BUFF;       //ͼ�񻺳���ָ��


//����ͼ��ɼ�״̬
typedef enum 
{
    IMG_NOTINIT=0,
	IMG_FINISH,			    //ͼ��ɼ����
	IMG_FAIL,				//ͼ��ɼ�ʧ��(�ɼ���������)
	IMG_GATHER,				//ͼ��ɼ���
	IMG_START,				//��ʼ�ɼ�ͼ��
	IMG_STOP,				//��ֹͼ��ɼ�
	
}IMG_STATE;

typedef struct
{
	uint8 Address;			       /*�Ĵ�����ַ*/
	uint8 Value;		           /*�Ĵ���ֵ*/
}Register_Info;

//extern 	uint8 Ov7725_vsync;


extern	uint8   ov7725_init(uint8 *imgaddr);
extern	void    ov7725_exti_init();
extern	void    ov7725_get_img();

extern	int  	ov7725_read_reg(uint8 LCD_Reg,uint16 LCD_RegValue);
extern	int  	ov7725_write_reg(uint8 LCD_Reg,uint16 LCD_RegValue);

#endif























