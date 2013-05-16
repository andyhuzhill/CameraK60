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
#include "string.h"

//// 本模块公共变量声明
static int8 leftBlack[IMG_H] = {0};
static int8 rightBlack[IMG_H]={0};
static int8 middle[IMG_H] = {0};                         //记录中线位置
static uint8 srcImg[CAMERA_SIZE];                        //保存摄像头采集数据
static vuint8 img[IMG_H][IMG_W];                         //将摄像头采集数据另存入此数组
static int8 leftLostRow=0, rightLostRow =0;              //左右边线丢失的行数

static const int8 jiaozheng[IMG_H] = {6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 12, 
        12, 13, 13, 14, 14, 15, 15, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21,
        22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 28};


////// 外部公共变量声明
extern volatile IMG_STATE img_flag;

void
imgInit(void)
{
    ov7725_init(srcImg);
    PORTA_ISFR = ~0;                        //清中断
    enable_irq(PORTA_IRQn);                 //允许PTA的中断
    img_flag = IMG_READY;
}

/**
 *  获取图像
 */
void
imgGetImg(void)
{
    if( (IMG_READY == img_flag) ){
        img_flag = IMG_START;                   //开始采集图像
        PORTA_ISFR=~0;                          //写1清中断标志位(必须的，不然回导致一开中断就马上触发中断)
    }
}

int
imgProcess(void)
{
    int8 k, b, e2sum;
    static int ret;
    uint32 filesize;

    //    FIL file;
    //    FATFS fs;

    imgGetImg();

    if(IMG_FINISH == img_flag)      // 当图像采集完毕 开始处理图像
    {
        img_flag = IMG_PROCESS;
        imgResize();
        imgGetMidLine();
        e2sum = imgLeastsq(8, 18, &k, &b);

        //        f_mount(0, &fs);
        //
        //        f_open( "0:/img.txt", FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
        //
        //        filesize = f_size(&file);
        //
        //        f_lseek( filesize);
        //
        //        printf( "srcImg is:\n");
        //
        //        for(int i=0; i< CAMERA_SIZE; ++i)
        //        {
        //            printf( "%d,",srcImg[i]);
        //        }
        //        printf( "\n\n");

        //        printf( "img is:\n");

#if 1
        for (int row = 0; row < IMG_H; ++row) 
        {
            for (int col = 0; col < IMG_W ; ++col) 
            {
                printf( "%d,",img[row][col]);
            }
            printf("\n");
        }

        printf("\n");

        printf("leftBlack is:\n");
        for(int i=0; i< IMG_H; ++i)
        {
            printf("%3d,",leftBlack[i]);
        }
        printf("\n");

        printf( "rightBlack is:\n");
        for(int i=0; i< IMG_H; ++i)
        {
            printf( "%3d,",rightBlack[i]);
        }
        printf("\n");
        printf( "middle is:\n");
        for(int i=0; i< IMG_H; ++i)
        {
            printf( "%3d,",middle[i]);
        }

        printf("\n\n");
#endif

        if(middle[IMG_H/2] > IMG_W/2)        //左偏
        {
            steerSetDuty(45);
        }else if (middle[IMG_H/2] < IMG_W /2){  //右偏
            steerSetDuty(55);
        }else {
            steerSetDuty(50);
        }
        img_flag = IMG_READY;
        return ret;
    }
    return ret;
}


/**
 * 影响到的变量 img[]
 */
void
imgResize(void)              
{
    uint32 tmpIndex, tmpRow;
    uint16 col,row;    
    uint16 oldCol,oldRow; 

    for(row=0; row < IMG_H; row++)
    {
        oldRow = ((row * CAMERA_H  + (IMG_H >> 1) ) / IMG_H);
        //注释：row = (oldRow - 1/2)*(IH/CH)
        tmpRow = oldRow * CAMERA_W ;

        for(col=0;col<IMG_W;col++)
        {
            oldCol = ((col * CAMERA_W  + (IMG_W >> 1) ) / IMG_W);
            //注释：col = (oldCol - 1/2)*(IH/CH)
            tmpIndex = tmpRow + oldCol;
            if( (srcImg[tmpIndex>>3] & (1<<(7- (tmpIndex & 0x07))) ) == 0  ) 
            {
                img[row][col] = 0;
            }
            else
            {
                img[row][col] = 1;
            }
        }
    }
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
    int8 row, col;

    int8 leftStart, leftEnd, rightStart, rightEnd;  
    int8 getLeftBlack=0, getRightBlack=0;             //标志是否找到黑线
    
    memset(leftBlack, 0, sizeof(leftBlack));          //初始化三个数组的值
    memset(rightBlack, 0, sizeof(rightBlack));
    memset(middle, 0, sizeof(middle));  

    for (row = IMG_H -6; row > (IMG_H -11); --row)       //搜索前五行
    {
        for (col = (IMG_W /2); col > 1; --col)          //  先找左边黑线
        {
            if (img[row][col] != 0 && img[row][col-1] !=0)
            {
                leftBlack[row] = col;       //记录下黑线的列数
                break;
            }
        }
        if (col == 1)                       //  没有发现黑线
        {   
            if(img[row][col]==0 && img[row][col-1]==0)
            {
                leftBlack[row] = 0;
                leftLostRow = row;
            }
        }

        for (col = (IMG_W /2); col < (IMG_W -1); ++col) // 再找右边黑线
        {
            if (img[row][col] != 0 && img[row][col+1] != 0)   //发现黑线
            {
                rightBlack[row] = col;   //记录下黑线的列数
                break;
            }
        }
        if (col == IMG_W -1)            
        {
            if(img[row][col]==0 && img[row][col+1] ==0) // 没有发现黑线
            {
                rightBlack[row] = IMG_W; 
                rightLostRow = row;    
            }
        }
    }

    leftStart = leftBlack[IMG_H -10];
    leftEnd = leftBlack[IMG_H -10];
    rightStart = rightBlack[IMG_H-10];
    rightEnd = rightBlack[IMG_H-10];          
    if (leftStart < 0) leftStart = 0;
    if (leftEnd > IMG_W -1) leftEnd = IMG_W-1;
    if (rightStart < 0) rightStart = 0;
    if (rightEnd > IMG_W -1) rightEnd = IMG_W-1; //避免数组访问越界

    for (row = IMG_H -11; row > 0 ; --row) 
    {
        getLeftBlack = 0;
        do {                                    // 查找左边黑线
            leftStart -= 3;
            leftEnd += 3;
            if (leftStart < 0) leftStart = 0;
            if (leftEnd > IMG_W -1) leftEnd = IMG_W-1;      //避免数组访问越界

            for (col = leftStart; col < leftEnd ; ++col) 
            {
                if (img[row][col] != 0 && img[row][col-1]!=0) 
                {   
                    leftBlack[row] = col;
                    getLeftBlack = 1;
                    break;
                }
            }
            if(leftStart ==0)          
                leftLostRow = row;
        } while ((leftStart != 0 || leftEnd != (IMG_W -1))&&(getLeftBlack !=1));

        getRightBlack = 0;
        do{                                    // 查找右边黑线
            rightStart -= 3;
            rightEnd += 3;
            if (rightStart < 0) rightStart = 0;
            if (rightEnd > IMG_W -1) rightEnd = IMG_W-1;    //避免数组访问越界

            for (col = rightStart; col < rightEnd; ++col) 
            {
                if (img[row][col]!=0 && img[row][col+1]!=0) 
                {
                    rightBlack[row] = col;
                    getRightBlack = 1;
                    break;
                }
            }
            if (rightEnd == IMG_W-1) 
                rightLostRow = row; 
        }while(((rightStart!=0) || (rightEnd != IMG_W-1))&&(getRightBlack !=1));
    }


    for (row = IMG_H-6; row > MIN(leftLostRow, rightLostRow); --row)  
    {
        middle[row] = (leftBlack[row] + rightBlack[row]) /2;
    }
}

/**
 *  使用最小二乘法计算跑道方向
 *  输入变量:  BaseLine起始行 FinalLine终止行
 *  输出变量:  k, 斜率 b 常数项 
 *  返回值:  最小二乘法拟合的残差和
 */
int8
imgLeastsq(uint8 BaseLine, uint8 FinalLine, int8 *k, int8 *b)
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

    *k =  sumX / sumY;
    *b =  averageY - *k*averageX;   

    for (i = BaseLine; i < FinalLine; ++i) 
    {
        error += (*k*i+*b - middle[i])*(*k*i+*b-middle[i]);
    }
    return error;
}


