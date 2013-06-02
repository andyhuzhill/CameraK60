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

#define IMG_H (50) 
#define IMG_W (50)  

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
void
imgResize(void);

/*
 * �˲� ������������ȥ��
 */
void
imgFilter(void);


/*
 * �����ߺ���
 * Ӱ�쵽�ı��� leftBlack[] �� rightBlack[]
 */

void
imgFindLine(void);

/*
 * ˵��: ��ȡͼ������
 */
void
imgGetMidLine(void);


/**
 *  ʹ����С���˷������ܵ�����
 *  �������:  BaseLine��ʼ�� < FinalLine��ֹ��
 *  �������:  k, б�� b ������  
 */
void 
imgLeastsq(int8 BaseLine, int8 FinalLine, float *k, int8 *b);

///*
// * ʹ���������ղ�ֵ�������
// */
//void
//imgLagrange(int row1, int row2,int row3);
//
///**
// * ʹ��������������߷��������
// * ����ֵ�� ����
// */
//int
//imgArea(int row1, int row2, int row3,int *area);


/*
 * �����߼��
 */
int
imgStartLine(void);

#endif /* IMG_PROCESS_H_ */
