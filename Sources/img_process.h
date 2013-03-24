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

#define IMG_H (CAMERA_H /10) //隔五行取图像
#define IMG_W (CAMERA_W /8) //图像一行字节数

void
imgResize(const uint8 *srcImg);

/*
 * 说明: 提取图像中线
 */

void
imgGetMidLine(void);


#endif /* IMG_PROCESS_H_ */
