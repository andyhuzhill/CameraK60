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
    if(IMG_READY == img_flag){
        img_flag = IMG_START;                   //��ʼ�ɼ�ͼ��
        PORTA_ISFR=~0;                          //д1���жϱ�־λ(����ģ���Ȼ�ص���һ���жϾ����ϴ����ж�)
    }
}

int
imgProcess(void)
{
    int8 k, b, e2sum;
    static int ret;

    int res;
    int size;

    FATFS fatfs;
    FIL  file;
    imgGetImg();

    if(IMG_FINISH == img_flag)      // ��ͼ��ɼ���� ��ʼ����ͼ��
    {
        img_flag = IMG_PROCESS;
        imgResize();
        imgGetMidLine();
        e2sum = imgLeastsq(8, 18, &k, &b);

        f_mount(0, &fatfs);

        res = f_open(&file, "0:/img.txt", FA_OPEN_ALWAYS | FA_WRITE);

        size = f_size(&file);

        f_lseek(&file, size);

        f_printf(&file, "srcImg is :\n");
        for(int i=0;i<CAMERA_SIZE;++i)
        {
            f_printf(&file, "%d,", srcImg[i]);
        }
        f_printf(&file, "\n\n");

        for (int row = 0; row < IMG_H; ++row) 
        {
            for (int col = 0; col < IMG_W ; ++col) 
            {
                f_printf(&file, "%d,",img[row][col]);
            }
            f_printf(&file, "\n");
        }

        f_printf(&file, "k= %d, b= %d, e2sum = %d\n",k,b,e2sum);
        f_close(&file);

        if ((ABS(k)<2)) {                               //ֱ��
            if(middle[IMG_H/2] > IMG_W/2)        //��ƫ
            {
                steerSetDuty(40);
            }else if (middle[IMG_H/2] < IMG_W /2){  //��ƫ
                steerSetDuty(60);
            }else {
                steerSetDuty(50);
            }
        }else if (((ABS(k) > 5) && (ABS(k) <10))&&(ABS(b)<30)){         //����
            ret = 50 + k;
            steerSetDuty(ret);
        }else if ((ABS(k)>10) && (ABS(b)>20 && (ABS(b)<40))){            //���
            ret = 50 + 2*k;
            steerSetDuty(ret);
        }else if (((ABS(k)<10) && (ABS(b) <30)) && (e2sum > 200)){
            ret = 50 + 0.5*k ;
            steerSetDuty(ret);
        }
        img_flag = IMG_READY;
    }
    return ret;
}


/**
 * ��ԭ��320X240���������320X24�����飨ÿ��40�ֽڣ���24�У�
 * Ӱ�쵽�ı��� img[]
 */
void
imgResize(void)              
{
    uint32 temp, tempY;
    uint16 col,row;    
    uint16 X,Y; 

    for(row=0; row < IMG_H; row++)
    {
        Y = ( (  row * CAMERA_H  + (IMG_H >> 1)) / IMG_H) ;
        tempY = Y * CAMERA_W ;

        for(col=0;col<IMG_W;col++)
        {
            X = (( col * CAMERA_W  + (IMG_W >> 1)) / IMG_W) ;
            temp = tempY + X;
            if( (srcImg[temp>>3] & (1<<(7- (temp & 0x07))) ) == 0){
                img[row][col] = 0;
            }
            else
            {
                img[row][col] = 1;
            }
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
    int8 getLeftBlack=0, getRightBlack=0;               //��־�Ƿ��ҵ�����


    for (row = IMG_H -1; row > (IMG_H -6); --row)       //����ǰ����
    {
        for (col = (IMG_W /2); col > 1; --col)          //  ������ߺ���
        {
            if (img[row][col] != 0 && img[row][col-1] !=0)
            {
                leftBlack[row] = col;       //��¼�º��ߵ�����
                DEBUG_OUT("leftBlack[%d] is : %d\n", row, leftBlack[row]);
                break;
            }
        }
        if (col == 1)                       //  û�з��ֺ���
        {   
            leftBlack[row] = 0; 
            leftLostRow = row;
        }

        for (col = (IMG_W /2); col < (IMG_W -1); ++col) // �����ұߺ���
        {
            if (img[row][col] != 0 && img[row][col+1] != 0)   //���ֺ���
            {
                rightBlack[row] = col;   //��¼�º��ߵ�����
                DEBUG_OUT("rightBlack[%d] is : %d\n", row, rightBlack[row]);
                break;
            }
        }
        if (col == IMG_W -1)            // û�з��ֺ���
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
            if(leftStart ==0)          
                leftLostRow = row;
        } while((leftStart != 0 || leftEnd != (IMG_W -1)) && (getLeftBlack !=1));

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
            if (rightEnd == IMG_W-1) 
                rightLostRow = row; 
        } while(((rightStart!=0) || (rightEnd != IMG_W-1)) && (getRightBlack !=1));
    }

    DEBUG_OUT("leftLostRow:%2d, rightLostRow:%2d\n",leftLostRow, rightLostRow);
    for (row = IMG_H-1; row > MIN(leftLostRow, rightLostRow); --row)  
    {
        middle[row] = (leftBlack[row] + rightBlack[row]) /2;
        DEBUG_OUT("middle[%d] is :%d", row, middle[row]);
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


