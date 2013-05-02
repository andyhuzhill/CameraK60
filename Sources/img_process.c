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

//// 本模块内部公共变量声明

static int8 middle[IMG_H] = {0};                         //记录中线位置
static uint8 srcImg[CAMERA_SIZE];                        //保存摄像头采集数据
static vuint8 img[IMG_H][IMG_W];                         //将摄像头采集数据另存入此数组
static int8 leftLostRow=0, rightLostRow =0;              //左右边线丢失的行数

////// 外部公共变量声明
extern volatile IMG_STATE img_flag;

void
imgInit(void)
{
    ov7725_init(srcImg);
    PORTA_ISFR = ~0;                        //清中断
}

/**
 *  获取图像
 */
void
imgGetImg(void)
{
    if((IMG_READY == img_flag) || (IMG_FAIL == img_flag)){
        img_flag = IMG_START;                   //开始采集图像
        PORTA_ISFR=~0;                          //写1清中断标志位(必须的，不然回导致一开中断就马上触发中断)
    }
}

int
imgProcess(void)
{    
    float k, b, e2sum;
    static int ret;
    
    imgGetImg();

    if(IMG_FINISH == img_flag)      // 当图像采集完毕 开始处理图像
    {
        img_flag = IMG_PROCESS;
        imgResize();
        imgFilter();
        imgGetMidLine();
        e2sum = imgLeastsq(3, 15, &k, &b);
        
        DEBUG_OUT("k = %d, b = %d, e2sum=%d\n",(int32)k, (int32)b, (int32)e2sum);
        
        if ((ABS(k)<5) && (ABS(b)< 30)) {                               //直道
            if(middle[IMG_H/2] > IMG_W/2)                               //直道左偏
            {
                steerSetDuty(45);
            }else if (middle[IMG_H/2] < IMG_W /2){                      //直道右偏
                steerSetDuty(55);
            }else {
                steerSetDuty(50);
            }
        }else if (((ABS(k) > 5) && (ABS(k) <10))&&(ABS(b)<30)){         //入弯
            ret = 50 + k;
            steerSetDuty(ret);
        }else if ((ABS(k)>10) && (ABS(b)>20 && (ABS(b)<40))){            //弯道
            ret = 50 + 2*k;
            steerSetDuty(ret);
        }else if (((ABS(k)<10) && (ABS(b) <30)) && (e2sum > 200)){
            ret = 50 + 0.5*k ;
            steerSetDuty(ret);
        }
        img_flag = IMG_READY;
        return ret;
    }
    return ret;
}


/**
 * 将原来320X240的数组存入320X24的数组（每行40字节，共24行）
 * 影响到的变量 img[]
 */
void
imgResize(void)              
{
    for (int row = 0; row < (IMG_H); ++row)
    {
        for (int col = 0; col < (IMG_W); ++col)
        {
            img[row][col] = (srcImg[row*10*IMG_W + col] ? 1 : 0) ;
        }
    }

#if 0
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

/**
 * 滤波 将孤立的噪声去掉
 * 影响到的变量: img[];
 */
void
imgFilter(void)
{
    uint8 row, col;
    for (row = 1; row < IMG_H-1; ++row) 
    {
        for (col = 1; col < IMG_W-1; ++col) 
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


/**
 * 说明: 提取图像中线
 * 影响到的变量:  middle[];
 */
void
imgGetMidLine(void)
{
    int8 leftBlack[IMG_H] = {0};
    int8 rightBlack[IMG_H]={0};
    
    int8 row, col;

    int8 leftStart, leftEnd, rightStart, rightEnd;  
    int8 getLeftBlack=0, getRightBlack=0;             //标志是否找到黑线


    for (row = IMG_H -1; row > (IMG_H -6); --row)       //搜索前五行
    {
        for (col = (IMG_W /2); col > 1; --col)          //  先找左边黑线
        {
            if (img[row][col] != 0 && img[row][col-1] !=0)
            {
                leftBlack[row] = col;       //记录下黑线的列数
                DEBUG_OUT("leftBlack[%d] is : %d\n", row, leftBlack[row]);
                break;
            }
        }
        if (col == 2)                       //  没有发现黑线
        {   
            leftBlack[row] = 0; 
            leftLostRow = row;
        }

        for (col = (IMG_W /2); col < (IMG_W -1); ++col) // 再找右边黑线
        {
            if (img[row][col] != 0 && img[row][col+1] != 0)   //发现黑线
            {
                rightBlack[row] = col;   //记录下黑线的列数
                DEBUG_OUT("rightBlack[%d] is : %d\n", row, rightBlack[row]);
                break;
            }
        }
        if (col == IMG_W -2)            // 没有发现黑线
        {
            rightBlack[row] = IMG_W -1; 
            rightLostRow = row;
        }
    }

    leftStart = leftBlack[IMG_H -5];
    leftEnd = leftBlack[IMG_H -5];
    rightStart = rightBlack[IMG_H-5];
    rightEnd = rightBlack[IMG_H-5];          
    if (leftStart < 0) leftStart = 0;
    if (leftEnd > IMG_W -1) leftEnd = IMG_W-1;
    if (rightStart < 0) rightStart = 0;
    if (rightEnd > IMG_W -1) rightEnd = IMG_W-1; //避免数组访问越界

    for (row = IMG_H -6; row > 0 ; --row) 
    {
        getLeftBlack = 0;
        do {
            leftStart -= 3;
            leftEnd += 3;
            if (leftStart < 0) leftStart = 0;
            if (leftEnd > IMG_W -1) leftEnd = IMG_W-1;      //避免数组访问越界

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
            if(leftStart ==0)          
                leftLostRow = row;
        } while ((leftStart != 0 || leftEnd != (IMG_W -1))&&(getLeftBlack !=1));

        getRightBlack = 0;
        do{
            rightStart -= 3;
            rightEnd += 3;
            if (rightStart < 0) rightStart = 0;
            if (rightEnd > IMG_W -1) rightEnd = IMG_W-1;    //避免数组访问越界

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
            if (rightEnd == IMG_W-1) 
                rightLostRow = row; 
        }while(((rightStart!=0) || (rightEnd != IMG_W-1))&&(getRightBlack !=1));
    }

    DEBUG_OUT("leftLostRow:%2d, rightLostRow:%2d\n",leftLostRow, rightLostRow);
    for (row = IMG_H-1; row > MIN(leftLostRow, rightLostRow); --row)  
    {
        middle[row] = (leftBlack[row] + rightBlack[row]) /2;
        DEBUG_OUT("middle[%d] is :%d", row, middle[row]);
    }
}

/**
 *  使用最小二乘法计算跑道方向
 *  输入变量:  BaseLine起始行 FinalLine终止行
 *  输出变量:  k, 斜率 b 常数项 (浮点型) 
 *  返回值:  最小二乘法拟合的残差和
 */
float
imgLeastsq(uint8 BaseLine, uint8 FinalLine, float *k, float *b)
{
    int32 sumX=0, sumY=0;   
    int32 averageX=0, averageY=0;     
    uint8 i;
    uint8 availableLines = FinalLine - BaseLine;
    float error=0;
    
    for (i = BaseLine; i < FinalLine; ++i) 
    {
      sumX += i;
      sumY += middle[i];
    }
    
    averageX = sumX / availableLines;
    averageY = sumY / availableLines;
    
    sumX = 0;
    sumY = 0;
    for (i = BaseLine; i < FinalLine; ++i) 
    {
        sumX += (i-averageX)*(middle[i]-averageY);
        sumY += (i-averageX)*(i-averageX);
    }
    
    *k = (float) sumX / sumY;
    *b = (float) averageY - *k*averageX;   
    
    for (i = BaseLine; i < FinalLine; ++i) 
    {
        error += (*k*i+*b - middle[i])*(*k*i+*b-middle[i]);
    }
    return error;
}


