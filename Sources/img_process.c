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

//// ��ģ�鹫����������
static int8 leftBlack[IMG_H] = {0};
static int8 rightBlack[IMG_H]={0};
static int8 middle[IMG_H] = {0};                         //��¼����λ��
static uint8 srcImg[CAMERA_SIZE];                        //��������ͷ�ɼ�����
static vuint8 img[IMG_H][IMG_W];                         //������ͷ�ɼ���������������
static int8 leftLostRow=0, rightLostRow =0;              //���ұ��߶�ʧ������

static const int8 jiaozheng[IMG_H] = {6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 12, 
        12, 13, 13, 14, 14, 15, 15, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21,
        22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 28};


////// �ⲿ������������
extern volatile IMG_STATE img_flag;

void
imgInit(void)
{
    ov7725_init(srcImg);
    PORTA_ISFR = ~0;                        //���ж�
    enable_irq(PORTA_IRQn);                 //����PTA���ж�
    img_flag = IMG_READY;
}

/**
 *  ��ȡͼ��
 */
void
imgGetImg(void)
{
    if( (IMG_READY == img_flag) ){
        img_flag = IMG_START;                   //��ʼ�ɼ�ͼ��
        PORTA_ISFR=~0;                          //д1���жϱ�־λ(����ģ���Ȼ�ص���һ���жϾ����ϴ����ж�)
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

    if(IMG_FINISH == img_flag)      // ��ͼ��ɼ���� ��ʼ����ͼ��
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

        if(middle[IMG_H/2] > IMG_W/2)        //��ƫ
        {
            steerSetDuty(45);
        }else if (middle[IMG_H/2] < IMG_W /2){  //��ƫ
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
 * Ӱ�쵽�ı��� img[]
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
        //ע�ͣ�row = (oldRow - 1/2)*(IH/CH)
        tmpRow = oldRow * CAMERA_W ;

        for(col=0;col<IMG_W;col++)
        {
            oldCol = ((col * CAMERA_W  + (IMG_W >> 1) ) / IMG_W);
            //ע�ͣ�col = (oldCol - 1/2)*(IH/CH)
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
 * �˲� ������������ȥ��
 * Ӱ�쵽�ı���: img[];
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
 * ˵��: ��ȡͼ������
 * Ӱ�쵽�ı���:  middle[];
 */
void
imgGetMidLine(void)
{
    int8 row, col;

    int8 leftStart, leftEnd, rightStart, rightEnd;  
    int8 getLeftBlack=0, getRightBlack=0;             //��־�Ƿ��ҵ�����
    
    memset(leftBlack, 0, sizeof(leftBlack));          //��ʼ�����������ֵ
    memset(rightBlack, 0, sizeof(rightBlack));
    memset(middle, 0, sizeof(middle));  

    for (row = IMG_H -6; row > (IMG_H -11); --row)       //����ǰ����
    {
        for (col = (IMG_W /2); col > 1; --col)          //  ������ߺ���
        {
            if (img[row][col] != 0 && img[row][col-1] !=0)
            {
                leftBlack[row] = col;       //��¼�º��ߵ�����
                break;
            }
        }
        if (col == 1)                       //  û�з��ֺ���
        {   
            if(img[row][col]==0 && img[row][col-1]==0)
            {
                leftBlack[row] = 0;
                leftLostRow = row;
            }
        }

        for (col = (IMG_W /2); col < (IMG_W -1); ++col) // �����ұߺ���
        {
            if (img[row][col] != 0 && img[row][col+1] != 0)   //���ֺ���
            {
                rightBlack[row] = col;   //��¼�º��ߵ�����
                break;
            }
        }
        if (col == IMG_W -1)            
        {
            if(img[row][col]==0 && img[row][col+1] ==0) // û�з��ֺ���
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
    if (rightEnd > IMG_W -1) rightEnd = IMG_W-1; //�����������Խ��

    for (row = IMG_H -11; row > 0 ; --row) 
    {
        getLeftBlack = 0;
        do {                                    // ������ߺ���
            leftStart -= 3;
            leftEnd += 3;
            if (leftStart < 0) leftStart = 0;
            if (leftEnd > IMG_W -1) leftEnd = IMG_W-1;      //�����������Խ��

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
        do{                                    // �����ұߺ���
            rightStart -= 3;
            rightEnd += 3;
            if (rightStart < 0) rightStart = 0;
            if (rightEnd > IMG_W -1) rightEnd = IMG_W-1;    //�����������Խ��

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
 *  ʹ����С���˷������ܵ�����
 *  �������:  BaseLine��ʼ�� FinalLine��ֹ��
 *  �������:  k, б�� b ������ 
 *  ����ֵ:  ��С���˷���ϵĲв��
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


