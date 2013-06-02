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
 * 找两边黑线
 * 影响到的变量 leftBlack[] 和 rightBlack[]
 */

void
imgFindLine(void);

/*
 * 说明: 提取图像中线
 */
void
imgGetMidLine(void);


/**
 *  使用最小二乘法计算跑道方向
 *  输入变量:  BaseLine起始行 < FinalLine终止行
 *  输出变量:  k, 斜率 b 常数项  
 */
void 
imgLeastsq(int8 BaseLine, int8 FinalLine, float *k, int8 *b);

///*
// * 使用拉格朗日插值拟合曲线
// */
//void
//imgLagrange(int row1, int row2,int row3);
//
///**
// * 使用面积法计算中线方向和曲率
// * 返回值： 曲率
// */
//int
//imgArea(int row1, int row2, int row3,int *area);


/*
 * 起跑线检测
 */
int
imgStartLine(void);

#endif /* IMG_PROCESS_H_ */
