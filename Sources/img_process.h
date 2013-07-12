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
__relocate_code__
void
imgResize(void);

/*
 * 滤波 将孤立的噪声去掉
 */
__relocate_code__
void
imgFilter(void);


/*
 * 找两边黑线
 * 影响到的变量 leftBlack[] 和 rightBlack[]
 */
__relocate_code__
void
imgFindLine(void);

/*
 * 说明: 提取图像中线
 */
__relocate_code__
void
imgGetMidLine(void);


/*
 * 求中线平均值
 */
__relocate_code__
int
imgAverage(int8_t start, int8_t end);

/**
 *  使用最小二乘法计算跑道方向
 *  输入变量:  BaseLine起始行 < FinalLine终止行
 *  输出变量:  k, 斜率 b 常数项  
 */
//__relocate_code__
//void 
//imgLeastsq(int8 BaseLine, int8 FinalLine, float *k, int8 *b);


/*
 * 起跑线检测
 */
__relocate_code__
void
imgStartLine(void);

#endif /* IMG_PROCESS_H_ */
