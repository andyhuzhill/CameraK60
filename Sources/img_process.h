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

#define IMG_H (CAMERA_H /10) //隔十行取图像
#define IMG_W (CAMERA_W /8)  //图像一行字节数

/*
 * 将原来320X240的数组存入320X24的数组（每行40字节，共24行）
 */
void
imgResize(const uint8 *srcImg);

/*
 * 滤波 将孤立的噪声去掉
 */
void
imgFilter(void);


/*
 * 说明: 提取图像中线
 */

void
imgGetMidLine(void);

int 
imgProcess(void);




#endif /* IMG_PROCESS_H_ */
