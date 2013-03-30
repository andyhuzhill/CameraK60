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

static int8 center[IMG_H] = {0};

extern uint8 *img_bin_buff;

/*
 * ��ԭ��320X240���������320X24�����飨ÿ��40�ֽڣ���24�У�
 */
void
imgResize(const uint8 *srcImg)              
{
    for (int row = 0; row < (IMG_H); ++row)
    {
        for (int col = 0; col < (IMG_W); ++col)
        {
            img[row][col] = (srcImg[row*10*IMG_W + col] ? 1 : 0) ;
        }
    }

#if 1
    for (int row = 0; row < (IMG_H); ++row)
    {
        printf("Row %2d:",row);
        for (int col = 0; col < (IMG_W); ++col)
        {
            printf("%d",img[row][col]);
        }
        printf("\n");
    }
#endif
}

/*
 * �˲� ������������ȥ��
 */
void
imgFilter(void)
{
    uint8 row, col;
    for (row = 1; row < IMG_H-2; ++row) 
    {
        for (col = 1; col < IMG_W-2; ++col) 
        {
            if (img[row][col-1]==1 && img[row][col]==0 && img[row][col+1]==1) 
            {
                img[row][col] = 1;
            }
            if (img[row][col-1]==0 && img[row][col]==1 && img[row][col+1]==0) 
            {
                img[row][col] = 0;
            }
        }
    }
}


/*
 * ˵��: ��ȡͼ������
 */

void
imgGetMidLine(void)
{
    int8 leftBlack[IMG_H] = {0};
    int8 rightBlack[IMG_H]={0};

    int8 row, col;

    int8 leftStart, leftEnd, rightStart, rightEnd;  
    int8 getLeftBlack=0, getRightBlack=0;             //��־�Ƿ��ҵ�����
    int8 leftLostRow=0, rightLostRow =0;              //���ұ��߶�ʧ������

    for (row = IMG_H -1; row > (IMG_H -6); --row)       //����ǰ����
    {
        for (col = (IMG_W /2); col > 2; --col)          //  ������ߺ���
        {
            if (img[row][col] != 0 && img[row][col-1] !=0)
            {
                leftBlack[row] = col;       //��¼�º��ߵ�����
                DEBUG_OUT("leftBlack[%d] is : %d\n", row, leftBlack[row]);
                break;
            }
        }
        if (col == 2)                       //  û�з��ֺ���
        {   
            leftBlack[row] = 0; 
        }

        for (col = (IMG_W /2); col < (IMG_W -2); ++col) // �����ұߺ���
        {
            if (img[row][col] != 0 && img[row][col+1] != 0)   //���ֺ���
            {
                rightBlack[row] = col;   //��¼�º��ߵ�����
                DEBUG_OUT("rightBlack[%d] is : %d\n", row, rightBlack[row]);
                break;
            }
        }
        if (col == IMG_W -2)            // û�з��ֺ���
        {
            rightBlack[row] = IMG_W -1; 
        }
    }

    leftStart = leftBlack[IMG_H -5];
    leftEnd = leftBlack[IMG_H -5];
    rightStart = rightBlack[IMG_H-5];
    rightEnd = rightBlack[IMG_H-5];          
    if (leftStart < 0) leftStart = 0;
    if (leftEnd > IMG_W -1) leftEnd = IMG_W-1;
    if (rightStart < 0) rightStart = 0;
    if (rightEnd > IMG_W -1) rightEnd = IMG_W-1; //�����������Խ��

    for (row = IMG_H -6; row > 0 ; --row) 
    {
        getLeftBlack = 0;
        do {
            leftStart -= 3;
            leftEnd += 3;
            if (leftStart < 0) leftStart = 0;
            if (leftEnd > IMG_W -1) leftEnd = IMG_W-1;      //�����������Խ��

            for (col = leftStart; col < leftEnd ; ++col) 
            {
                if (img[row][col] != 0 && img[row][col-1]!=0) 
                {   
                    leftBlack[row] = col;
                    DEBUG_OUT("leftBlack[%d] is : %d\n", row, leftBlack[row]);
                    getLeftBlack = 1;
                    break;
                }
            }
            if(leftStart ==0 && leftEnd== IMG_W-1)          
                leftLostRow = row;
        } while ((leftStart != 0 || leftEnd != (IMG_W -1))&&(getLeftBlack !=1));

        getRightBlack = 0;
        do{
            rightStart -= 3;
            rightEnd += 3;
            if (rightStart < 0) rightStart = 0;
            if (rightEnd > IMG_W -1) rightEnd = IMG_W-1;    //�����������Խ��

            for (col = rightStart; col < rightEnd; ++col) 
            {
                if (img[row][col]!=0 && img[row][col+1]!=0) 
                {
                    rightBlack[row] = col;
                    DEBUG_OUT("rightBlack[%d] is : %d\n", row, rightBlack[row]);
                    getRightBlack = 1;
                    break;
                }
            }
            if (rightStart == 0 && rightEnd == IMG_W-1) 
                rightLostRow = row; 
        }while(((rightStart!=0) || (rightEnd != IMG_W-1))&&(getRightBlack !=1));
    }

    DEBUG_OUT("leftLostRow:%2d, rightLostRow:%2d\n",leftLostRow, rightLostRow);
    for (row = IMG_H-1; row > MIN(leftLostRow, rightLostRow); --row)  
    {
        center[row] = (leftBlack[row] + rightBlack[row]) /2;
        DEBUG_OUT("center[%d] is :%d", row, center[row]);
    }
}


int 
imgProcess(void)
{
    int err[IMG_H]={0};
    int slop, slop1, slop2, slop_add;
    imgResize(img_bin_buff);
    imgGetMidLine();
    
    for (int cnt = 0; cnt < IMG_H-1; ++cnt) 
    {
        err[cnt] = center[cnt] - IMG_W/2;
    }
    slop = err[IMG_W-1] -err[0];
    slop1 = center[IMG_H] - center[IMG_H/2];
    slop2 = center[IMG_H/2] - center[0];
    slop_add = slop1 + slop2;
    
    if (slop1 * slop2 < 0)  //���
    {
        if (-5 <= slop_add && slop_add <= 5) 
        {
//            control_steer = err[0];
//            control_speed = 0;
        }
    }
    else
    {
        if (-3 <= slop_add && slop_add <= 3)    //ֱ�� 
        {
//            control_steer = err[7];
        }
    }
}
