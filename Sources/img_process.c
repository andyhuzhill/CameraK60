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
}

/**
 *  ��ȡͼ��
 */
void
imgGetImg(void)
{
    if((IMG_READY == img_flag) || (IMG_FAIL == img_flag)){
        img_flag = IMG_START;                   //��ʼ�ɼ�ͼ��
        PORTA_ISFR=~0;                          //д1���жϱ�־λ(����ģ���Ȼ�ص���һ���жϾ����ϴ����ж�)
    }
}

int
imgProcess(void)
{
    int8 k, b, e2sum;
    static int ret;
    uint8 status = 0;

    int8 buff[3+IMG_H]={0};

    int res;
    int size;

    FATFS fatfs;
    FIL  file;

    imgGetImg();



    if(IMG_FINISH == img_flag)      // ��ͼ��ɼ���� ��ʼ����ͼ��
    {
        img_flag = IMG_PROCESS;
        imgResize();
        imgFilter();
        imgGetMidLine();
        e2sum = imgLeastsq(8, 18, &k, &b);

        f_mount(0, &fatfs);

        res = f_open(&file, "0:/img2.txt", FA_OPEN_ALWAYS | FA_WRITE);

        if (res == FR_DISK_ERR) 
        {
            printf("no sd card inserted\n");
            GPIOD_PTOR |= (1 << 10);
            return ;
        }else{
//            return ;
        }
     
        size = f_size(&file);

        f_lseek(&file, size);

        for (int row = 0; row < IMG_H; ++row) 
        {
            for (int col = 0; col < IMG_W ; ++col) 
            {
                f_printf(&file, "%d,",img[row][col]);
            }
            f_printf(&file, "\n");
        }
        
        f_printf(&file, "k= %d, b= %d, e2sum = %d\n",k,b,e2sum);
        
        f_printf(&file, "\n\n\n");
        
        GPIOD_PTOR |= (1 << 15);

        f_close(&file);


//        printf("k = %d, b = %d, e2sum=%d\n",k, b, e2sum);
//
//        buff[0] = e2sum;
//        buff[1] = b;
//        buff[2] = k;

//        memcpy(&buff[3],middle, IMG_H);
//
//        for (int i = 0; i < IMG_H; ++i) 
//        {
//            printf("%6d,%5d\n",i,middle[i]);
//        }

        //        NRF_ISR_Tx_Dat((uint8*)buff, sizeof(buff));
        //
        //        do {
        //            status = NRF_ISR_Tx_State();
        //        } while (status == TX_ISR_SEND);


        if ((ABS(k)<2)) {                               //ֱ��
            if(middle[IMG_H/2] > IMG_W/2)        //��ƫ
            {
                steerSetDuty(45);
            }else if (middle[IMG_H/2] < IMG_W /2){  //��ƫ
                steerSetDuty(55);
            }else {
                steerSetDuty(50);
            }
        }else if (((ABS(k) > 5) && (ABS(k) <10))&&(ABS(b)<30)){         //����
            ret = 50 + k;
        }else if ((ABS(k)>10) && (ABS(b)>20 && (ABS(b)<40))){            //���
            ret = 50 + 2*k;
        }else if (((ABS(k)<10) && (ABS(b) <30)) && (e2sum > 200)){
            ret = 50 + 0.5*k ;
        }
        img_flag = IMG_READY;
        return ret;
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
        if (col == 2)                       //  û�з��ֺ���
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
        if (col == IMG_W -2)            // û�з��ֺ���
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


