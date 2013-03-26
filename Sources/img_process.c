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
imgResize(const uint8 *srcImg)              //将原来320X240的数组变成320X24的数组（每行40字节，共24行）
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

    int8 leftStart, leftEnd, rightStart, rightEnd;
    int8 tmp1, tmp2, tmp3, tmp4, tmp5;
    uint8 leftFailNum, rightFailNum;

    //计算前两行左右边缘
    for (row = (IMG_H -1); row > (IMG_H -3); --row)              //先计算左边界
    {
        for (col = (IMG_W/2-1); col > 2; --col)                 //从中间列开始测量
        {
            if (((img[row][col]==0) && img[row][col-1] !=0)
                    || ((img[row][col-2]!=0)&&(img[row][col-1]!=0)&&(img[row][col]!=0)))
            {
                leftBlack[row] = col;
                break;
            }
        }
        if (col == 2)
        {
            leftBlack[row] = 0;         //丢失左边线
        }
    }

    DEBUG_OUT("blackLeft : %d,%d\n", leftBlack[IMG_H-1], leftBlack[IMG_H -2]);

    for (row = (IMG_H-1) ; row > (IMG_H -3); --row )                  //再计算右边界
    {
        for (col = (IMG_W /2-1); col < IMG_W-2; ++col )              //从中间列开始测量
        {
            if (((img[row][col]!=0) && (img[row][col+1]==0))
                    || ((img[row][col]!=0)&&(img[row][col+1]!=0)&&(img[row][col+2]!=0)))
            {
                rightBlack[row] = col;
                break;
            }
        }
        if(col == IMG_W-2)
        {
            rightBlack[row] = IMG_W -1;  //丢失右边线
        }
    }
    DEBUG_OUT("blackRight : %d,%d\n", rightBlack[IMG_H-1], rightBlack[IMG_H -2]);
    if (leftBlack[IMG_H-1] == 0 && rightBlack[IMG_H-1] == IMG_W -1) {
        DEBUG_OUT("lost lines\n",0);   //两边的线都丢失了，跑出跑道
        return ;
    }

    leftStart = leftBlack[IMG_H-2]-3;
    leftEnd = leftBlack[IMG_H-2]+3;
    rightStart = rightBlack[IMG_H-2]-3;
    rightEnd = rightBlack[IMG_H-2]+3;
    if (leftStart <=0 ) leftStart = 0;
    if (leftEnd >= IMG_W-1) leftEnd = IMG_W -1;
    if (rightStart <= 0) rightStart = 0;
    if (rightEnd >= IMG_W -1) rightEnd = IMG_W -1;

    for (row = IMG_H -3; row > 2; --row) 
    {
        for (col = leftStart; col < leftEnd; ++col) 
        {
            if (img[row][col] != 0)         //找到黑线
            {
                leftBlack[row] = col;
                leftStart = col-3;
                leftEnd = col +3;
                if (leftStart <= 0) leftStart =0;
                if (leftEnd >= IMG_W -1) leftEnd = IMG_W-1;
                break;
            }
        }
        if (col == leftEnd)     // 没有找到黑线,扩大搜索范围查找
        {
            leftStart -= 3;
            leftEnd += 3;
            if (leftStart <= 0) leftStart =0;
            if (leftEnd >= IMG_W -1) leftEnd = IMG_W-1;
            for (col = leftStart; col < leftEnd; ++col) 
            {
                if (img[row-1][col] != 0) 
                {
                    leftBlack[row-1] = col;
                    leftBlack[row] = ((leftBlack[row+1] + col) /2);
                    leftStart  = col -3;
                    leftEnd = col + 3;
                    if (leftStart <= 0) leftStart =0;
                    if (leftEnd >= IMG_W -1) leftEnd = IMG_W-1;
                    row --;
                    break;
                }
            }
            if(row == leftEnd)    // 没有找到黑线,扩大搜索范围查找
            {
                leftStart -= 3;
                leftEnd += 3;
                if (leftStart <= 0) leftStart =0;
                if (leftEnd >= IMG_W -1) leftEnd = IMG_W-1;
                for(col = leftStart; col < leftEnd; col++)
                {
                    if (img[row-2][col] != 0) {
                        leftBlack[row-2] = col;
                        leftBlack[row] = 2* leftBlack[row+1] - leftBlack[row+2];
                        leftBlack[row-1] = (leftBlack[row]+col) /2;
                        leftStart -= 3;
                        leftEnd += 3;
                        if (leftStart <= 0) leftStart =0;
                        if (leftEnd >= IMG_W -1) leftEnd = IMG_W-1;
                        row -= 2;
                        break;
                    }
                }
                if (col == leftEnd)     //没有找到黑线
                {   
                    leftStart = 0;
                    leftEnd = 6;
                }
            }
        }
    }//左边界查找完毕


    for (row = IMG_H -3; row > 2; --row) 
    {
        for (col = rightStart; col < rightEnd; ++col) 
        {
            if (img[row][col] != 0)         //找到黑线
            {
                rightBlack[row] = col;
                rightStart = col-3;
                rightEnd = col +3;
                if (rightStart <= 0) rightStart =0;
                if (rightEnd >= IMG_W -1) rightEnd = IMG_W-1;
                break;
            }
        }
        if (col == rightEnd)     // 没有找到黑线,扩大搜索范围查找
        {
            rightStart -= 3;
            rightEnd += 3;
            if (rightStart <= 0) rightStart =0;
            if (rightEnd >= IMG_W -1) rightEnd = IMG_W-1;
            for (col = rightStart; col < rightEnd; ++col) 
            {
                if (img[row-1][col] != 0) 
                {
                    rightBlack[row-1] = col;
                    rightBlack[row] = ((rightBlack[row+1] + col) /2);
                    rightStart  = col -3;
                    rightEnd = col + 3;
                    if (rightStart <= 0) rightStart =0;
                    if (rightEnd >= IMG_W -1) rightEnd = IMG_W-1;
                    row --;
                    break;
                }
            }
            if(row == rightEnd)    // 没有找到黑线,扩大搜索范围查找
            {
                rightStart -= 3;
                rightEnd += 3;
                if (rightStart <= 0) rightStart =0;
                if (rightEnd >= IMG_W -1) rightEnd = IMG_W-1;
                for(col = rightStart; col < rightEnd; col++)
                {
                    if (img[row-2][col] != 0) {
                        rightBlack[row-2] = col;
                        rightBlack[row] = 2* rightBlack[row+1] - rightBlack[row+2];
                        rightBlack[row-1] = (rightBlack[row]+col) /2;
                        rightStart -= 3;
                        rightEnd += 3;
                        if (rightStart <= 0) rightStart =0;
                        if (rightEnd >= IMG_W -1) rightEnd = IMG_W-1;
                        row -= 2;
                        break;
                    }
                }
                if (col == rightEnd)     //没有找到黑线
                {   
                    rightStart = 0;
                    rightEnd = 6;
                }
            }
        }
    }//右边界查找完毕

    //判断左边界有效行
    for (row = IMG_H-1; row > 1; --row) 
    {
        if (leftBlack[row] != 0 && leftBlack[row-1] != 0 && leftBlack[row-2] != 0) 
        {
            tmp5 = row;
            break;
        }
    }
    if (tmp5 <= 10 || row == 1) 
    {
        leftFailNum = IMG_H -1;
    }else{
        for (row = tmp5; row > 2; --row) 
        {
            if (ABS(leftBlack[row] - leftBlack[row-1]) > 5) 
            {
                tmp4 = row;
                break;
            }
        }
        if (2 == row) {
            tmp4 = 2;
        }
        for (row = tmp5; row >= tmp4 ; --row) 
        {
            if (leftBlack[row] == 0) {
                leftFailNum = row;
                break;
            }
        }
        if (row == tmp4 -1) {
            leftFailNum = tmp4;
        }
    }


    //判断右边界有效行
    for (row = IMG_H-1; row > 1; --row) 
    {
        if (rightBlack[row] != 0 && rightBlack[row-1] != 0 && rightBlack[row-2] != 0) 
        {
            tmp5 = row;
            break;
        }
    }
    if (tmp5 <= 10 || row == 1) 
    {
        rightFailNum = IMG_H -1;
    }else{
        for (row = tmp5; row > 2; --row) 
        {
            if (ABS(rightBlack[row] - rightBlack[row-1]) > 5) 
            {
                tmp4 = row;
                break;
            }
        }
        if (2 == row) {
            tmp4 = 2;
        }
        for (row = tmp5; row >= tmp4 ; --row) 
        {
            if (rightBlack[row] == IMG_W -1) {
                rightFailNum = row;
                break;
            }
        }
        if (row == tmp4 -1) {
            rightFailNum = tmp4;
        }
    }
    
    if (leftFailNum == IMG_H -1) {
        tmpValueRowNum = rightFailNum;
        count = leftFailNum;
        for (row = IMG_H -1; row > rightFailNum; --row) {
            tmp1 = (uint8) ((IMG_H -1-row)/2);
            tmp2 = 52 - tmp1;
            center[row] = rightBlack[row] -tmp2;
        }
    } else if(leftFailNum < rightFailNum){
        tmpValueRowNum = leftFailNum;
        count = rightFailNum;
        for (row = IMG_H -1; row > rightFailNum; --row ) {
            center[row] = (uint8)((leftBlack[row]+rightBlack[row])/2);
        }
        tmp1 = center[rightFailNum +1] -leftBlack[rightFailNum+1];
        for (row = rightFailNum; row > leftFailNum; --row) {
            center[row] = leftBlack[row] + tmp1;
        }
    }
    else if(leftFailNum >= rightFailNum){
        tmpValueRowNum = rightFailNum;
        for (row = IMG_H -1; row > leftFailNum; --row ) {
            center[row] = (uint8)((leftBlack[row]+rightBlack[row])/2);
        }
        tmp3 = rightBlack[leftFailNum+1] -center[leftFailNum+1];
        for (row = leftFailNum; row > rightFailNum; --row) {
            center[row] = rightBlack[row] -tmp3;
        }
    }
    
    for (row = 0; row < IMG_H; ++row) {
        DEBUG_OUT("%d,",center[row]);
    }
}
