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

void
imgResize(const uint8 *srcImg)              //将原来320X240的数组变成320X24的数组（每行40字节，共24行）
{
    for (int row = 0; row < (IMG_H); ++row)
    {
        for (int col = 0; col < (IMG_W); ++col)
        {
            img[row][col] = srcImg[row*10*IMG_W + col];
        }
    }

    for (int row = 0; row < (IMG_H); ++row)
    {
        for (int col = 0; col < (IMG_W); ++col)
        {
            printf("%d", img[row][col] ? 1 : 0);
        }
        printf("\n");
    }
}

/*
 * 说明: 提取图像中线
 */

void
imgGetMidLine(void)
{
    int8 blackLeft[IMG_H] = {0};
    int8 center[IMG_H] = {0};
    int8 blackRight[IMG_H]={0};

    int8 row, col;

    //计算所有行左右边缘
    for (row = (IMG_H -1); row > 0; --row)              //先计算左边界
    {
        for (col = (IMG_W/2-1); col > 2; --col)                 //从中间列开始测量
        {
            if (((img[row][col]==0) && img[row][col-1] !=0)
                    || ((img[row][col-2]!=0)&&(img[row][col-1]!=0)&&(img[row][col]!=0)))
            {
                blackLeft[row] = col;
                break;
            }
        }
        if (col ==0)
        {
            blackLeft[row] = 0;
        }
    }

    for (row = (IMG_H-1) ; row > 0; --row )                  //再计算右边界
    {
        for (col = (IMG_W /2-1); col < IMG_W-2; ++col )              //从中间列开始测量
        {
            if (((img[row][col]!=0) && (img[row][col+1]==0))
                    || ((img[row][col]!=0)&&(img[row][col+1]!=0)&&(img[row][col+2]!=0)))
            {
                blackRight[row] = col;
                break;
            }
        }
        if(col == IMG_W)
        {
            blackRight[row] = IMG_W -1;
        }
    }

}
