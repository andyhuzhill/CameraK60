/**
 * =======================================================================
 * img_process.h
 *
 *  Created on: 2013-3-23
 *      Author: scout
 *      E-mail: andyhuzhill@gmail.com
 *
 * Description: 
 *
 *     Version:
 * =========================================================================
*/

#ifndef IMG_PROCESS_H_
#define IMG_PROCESS_H_

#include "common.h"
#include "derivative.h"

#define IMG_H (CAMERA_H) 
#define IMG_W (CAMERA_W)  
#define IMG_MID (40)

/** ͼ��ɼ���ʼ��
 */
void
imgInit(void);

/**
 * ͼ����������
 */
int 
imgProcess(void);

/*
 * ��ԭ��320X240���������320X24�����飨ÿ��40�ֽڣ���24�У�
 */
__relocate_code__
void
imgResize(void);

/*
 * �˲� ������������ȥ��
 */
__relocate_code__
void
imgFilter(void);


/*
 * �����ߺ���
 * Ӱ�쵽�ı��� leftBlack[] �� rightBlack[]
 */
__relocate_code__
void
imgFindLine(void);

/*
 * ˵��: ��ȡͼ������
 */
__relocate_code__
void
imgGetMidLine(void);


/*
 * ������ƽ��ֵ
 */
__relocate_code__
int
imgAverage(int8_t start, int8_t end);

/**
 *  ʹ����С���˷������ܵ�����
 *  �������:  BaseLine��ʼ�� < FinalLine��ֹ��
 *  �������:  k, б�� b ������  
 */
//__relocate_code__
//void 
//imgLeastsq(int8 BaseLine, int8 FinalLine, float *k, int8 *b);


/*
 * �����߼��
 */
__relocate_code__
void
imgStartLine(void);

#endif /* IMG_PROCESS_H_ */
