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
imgResize(const uint8 *srcImg)              //��ԭ��320X240���������320X24�����飨ÿ��40�ֽڣ���24�У�
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
 * ˵��: ��ȡͼ������
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
                    (img[row][col] != 0 && img[row][col+1] != 0))   //���ֺ���
            {
                rightBlack[row] = col;   //��¼�º��ߵ�����
                break;
            }
        }
        if (col == IMG_W -2)            // û�з��ֺ���
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
