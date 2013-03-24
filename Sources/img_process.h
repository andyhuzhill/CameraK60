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

#define IMG_H (CAMERA_H /10) //������ȡͼ��
#define IMG_W (CAMERA_W /8) //ͼ��һ���ֽ���

void
imgResize(const uint8 *srcImg);

/*
 * ˵��: ��ȡͼ������
 */

void
imgGetMidLine(void);


#endif /* IMG_PROCESS_H_ */
