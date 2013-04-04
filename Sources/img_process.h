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

#define IMG_H (CAMERA_H /10) //��ʮ��ȡͼ��
#define IMG_W (CAMERA_W /8)  //ͼ��һ���ֽ���

/*
 * ��ԭ��320X240���������320X24�����飨ÿ��40�ֽڣ���24�У�
 */
void
imgResize(const uint8 *srcImg);

/*
 * �˲� ������������ȥ��
 */
void
imgFilter(void);


/*
 * ˵��: ��ȡͼ������
 */

void
imgGetMidLine(void);

int 
imgProcess(void);




#endif /* IMG_PROCESS_H_ */
