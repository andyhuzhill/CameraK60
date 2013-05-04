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

/** 图像采集初始化
 */
void
imgInit(void);

/**
 * 图像处理主函数
 */
int 
imgProcess(void);

/*
 * 将原来320X240的数组存入320X24的数组（每行40字节，共24行）
 */
void
imgResize(void);

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


/**
 *  使用最小二乘法计算跑道方向
 *  输入变量:  BaseLine起始行 FinalLine终止行
 *  输出变量:  k, 斜率 b 常数项 (浮点型) 
 *  返回值:  最小二乘法拟合的残差和
 */
float
imgLeastsq(uint8 BaseLine, uint8 FinalLine, int8 *k, int8 *b);


#endif /* IMG_PROCESS_H_ */
