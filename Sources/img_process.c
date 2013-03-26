/**
 * =======================================================================
 * img_process.c
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

#include "img_process.h"

static uint8 img[IMG_H][IMG_W];


int tmpValueRowNum ;
int count;

void
imgResize(const uint8 *srcImg)              //将原来320X240的数组存入320X24的数组（每行40字节，共24行）
{
    for (int row = 0; row < (IMG_H); ++row)
    {
        for (int col = 0; col < (IMG_W); ++col)
        {
            img[row][col] = srcImg[row*10*IMG_W + col];
        }
    }

#if 1
    for (int row = 0; row < (IMG_H); ++row)
    {
        for (int col = 0; col < (IMG_W); ++col)
        {
            printf("%d", img[row][col] ? 1 : 0);
        }
        printf("\n");
    }
#endif
}

/*
 * 说明: 提取图像中线
 */

void
imgGetMidLine(void)
{
    int8 leftBlack[IMG_H] = {0};
    int8 rightBlack[IMG_H]={0};
    int8 center[IMG_H] = {0};

    int8 row, col;

    for (row = IMG_H -1; row > (IMG_H -2); --row) 
    {
        for (col = (IMG_W /2); col < (IMG_W -2); ++col) 
        {
            if (img[row][col] != 0 && img[row][col-1] ==0 ||
                    (img[row][col] != 0 && img[row][col+1] != 0))   //发现黑线
            {
                rightBlack[row] = col;   //记录下黑线的列数
                break;
            }
        }
        if (col == IMG_W -2)            // 没有发现黑线
        {
            rightBlack[row] = IMG_W -1; 
        }
        
        for (col = (IMG_W /2); col > 2; --col) 
        {
            if (img[row][col] == 0 && img[row][col-1] !=0 ||
                    (img[row][col] != 0 && img[row][col-1] !=0)) 
            {
                
            }
        }
    }
}
