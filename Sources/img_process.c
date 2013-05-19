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
static uint8 middle2[IMG_H] = {0};

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
    int qulv,area;
    static int ret;
    int A = 35;
    int B = 22;
    int C = 10;

    imgGetImg();

    if(IMG_FINISH == img_flag)  {      // ��ͼ��ɼ���� ��ʼ����ͼ��
        img_flag = IMG_PROCESS;
        imgResize();
        imgFindLine();
        imgGetMidLine();
        e2sum = imgLeastsq(C, A, &k, &b);
        qulv = imgArea(A,B,C,&area);

#if 1
        for (int row = 0; row < IMG_H; ++row) {
            for (int col = 0; col < IMG_W ; ++col)  {
                printf( "%d,",img[row][col]);
            }
            printf("\n");
        }
        printf("\n");

        printf("img\n");
#endif

#if 1
        if ((leftBlack[20] == -1 || leftBlack[20] == 0) && (rightBlack[20] != -1)){
            steerSetDuty(600);
        }else if((leftBlack[20]!= -1) &&(rightBlack[20] == -1 || rightBlack[20] ==0)){
            steerSetDuty(400);
        }else if(middle[20] < 18){
            steerSetDuty(400);
        }else if(middle[20] > 32){
            steerSetDuty(600);
        }else{
            steerSetDuty(500);
        }

#endif
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

    for(row=0; row < IMG_H; row++) {
        oldRow = ((row * CAMERA_H  + (IMG_H >> 1) ) / IMG_H);
        //ע�ͣ�row = (oldRow - 1/2)*(IH/CH)
        tmpRow = oldRow * CAMERA_W ;

        for(col=0;col<IMG_W;col++) {
            oldCol = ((col * CAMERA_W  + (IMG_W >> 1) ) / IMG_W);
            //ע�ͣ�col = (oldCol - 1/2)*(IH/CH)
            tmpIndex = tmpRow + oldCol;
            if( (srcImg[tmpIndex>>3] & (1<<(7- (tmpIndex & 0x07))) ) == 0  ) {
                img[row][col] = 0;
            }else{
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
    for (row = 1; row < IMG_H-1; ++row) {
        for (col = 1; col < IMG_W-1; ++col) {
            if (img[row][col-1]==1 && img[row][col]==0 && img[row][col+1]==1) {
                img[row][col] = 1;
            }
            if (img[row][col-1]==0 && img[row][col]==1 && img[row][col+1]==0) {
                img[row][col] = 0;
            }
        }
    }
}

void
imgFindLine2(void)
{
    int8 row, col;

    int8 leftStart, leftEnd, rightStart, rightEnd;  
    int8 getLeftBlack=0, getRightBlack=0;             //��־�Ƿ��ҵ�����

    memset(leftBlack, 0, sizeof(leftBlack));
    memset(rightBlack, 0, sizeof(rightBlack));
    memset(middle, 0, sizeof(middle));

    for (row = IMG_H -10; row > 0; --row){  
        getLeftBlack = 0;
        getRightBlack = 0;
        for (col = (IMG_W /2); col > 0; --col) {    //  ������ߺ���
            if (img[row][col] != 0){
                getLeftBlack = 1;
                leftBlack[row] = col;               //��¼�º��ߵ�����
                break;
            }
        }
        if (getLeftBlack != 1)  {                    //  û�з��ֺ���
            leftBlack[row] = -1;
            leftLostRow = row;
        }

        for (col = (IMG_W /2); col < (IMG_W -1); ++col) {  // �����ұߺ���
            if (img[row][col] != 0){                //���ֺ���
                rightBlack[row] = col;              //��¼�º��ߵ�����
                getRightBlack = 1;
                break;
            }
        }
        if (getRightBlack != 1) {               // û�з��ֺ���
            rightBlack[row] = -1;
            rightLostRow = row;
        }
    }
}

/*
 * �����ߺ���
 * Ӱ�쵽�ı��� leftBlack[] �� rightBlack[]
 */

void
imgFindLine(void)
{
    int8 row, col;

    int8 leftStart, leftEnd, rightStart, rightEnd;  
    int8 getLeftBlack=0, getRightBlack=0;             //��־�Ƿ��ҵ�����
    int8 leftLostCnt =0, rightLostCnt =0;


    memset(leftBlack, 0, sizeof(leftBlack));
    memset(rightBlack, 0, sizeof(rightBlack));
    memset(middle, 0, sizeof(middle));

    for (row = IMG_H -10; row > (IMG_H -16); --row){  //����ǰ����
        getLeftBlack = 0;
        getRightBlack = 0;
        for (col = (IMG_W /2); col > 0; --col) {    //  ������ߺ���
            if (img[row][col] != 0){
                getLeftBlack = 1;
                leftBlack[row] = col;               //��¼�º��ߵ�����
                break;
            }
        }
        if (getLeftBlack != 1)  {                    //  û�з��ֺ���
            leftBlack[row] = -1;
        }

        for (col = (IMG_W /2); col < (IMG_W -1); ++col) {  // �����ұߺ���
            if (img[row][col] != 0){                //���ֺ���
                rightBlack[row] = col;              //��¼�º��ߵ�����
                getRightBlack = 1;
                break;
            }
        }
        if (getRightBlack != 1) {               // û�з��ֺ���
            rightBlack[row] = -1;
        }
    }

    if(leftBlack[IMG_H-15] != -1){
        leftStart = leftBlack[IMG_H -15];
    }else{
        leftStart = 0;
    }
    if(leftBlack[IMG_H-15] != -1){
        leftEnd = leftBlack[IMG_H -15];
    }else{
        leftEnd = 0;
    }
    if(rightBlack[IMG_H-15] != -1){
        rightStart = rightBlack[IMG_H -15];
    }else{
        rightStart = IMG_W-1;
    }
    if(rightBlack[IMG_H-15] != -1){
        rightEnd = rightBlack[IMG_H -15];
    }else{
        rightEnd = IMG_W-1;
    }
    if (leftStart < 0) leftStart = 0;
    if (leftEnd > IMG_W -1) leftEnd = IMG_W-1;
    if (rightStart < 0) rightStart = 0;
    if (rightEnd > IMG_W -1) rightEnd = IMG_W-1; //�����������Խ��

    for (row = IMG_H -16; row > 0 ; --row){                 //����ߺ���
        getLeftBlack = 0;

        leftStart += 3;
        leftEnd -= 3;
        if (leftEnd < 0) leftEnd = 0;
        if (leftStart > IMG_W -1) leftStart = IMG_W-1;      //�����������Խ��

        for (col = leftStart; col > leftEnd ; --col) {      //������������
            if (img[row][col] != 0){                        //�ҵ�����
                leftBlack[row] = col;
                leftStart = col;
                leftEnd = col;
                getLeftBlack = 1;
                if(leftLostCnt == 1){                       //����
                    leftBlack[row+1] = (leftBlack[row+2]+leftBlack[row]) /2;
                }
                if(leftLostCnt == 2){                       //����
                    leftBlack[row+1] = leftBlack[row] - (leftBlack[row]-leftBlack[row+3])/3;
                    leftBlack[row+2] = leftBlack[row+3] + (leftBlack[row]-leftBlack[row+3])/3;
                }
                if(leftLostCnt == 3){
                    leftBlack[row+1] = leftBlack[row] - (leftBlack[row]-leftBlack[row+4])/4;
                    leftBlack[row+2] = leftBlack[row+4] + (leftBlack[row]-leftBlack[row+4])/2;
                    leftBlack[row+3] = leftBlack[row+4] + (leftBlack[row]-leftBlack[row+4])/4;
                }

                leftLostCnt = 0;
                break;      //�ҵ������˳�forѭ��
            }
        }
        if(getLeftBlack != 1){      //û���ҵ�����
            leftLostCnt ++;
            if(leftLostCnt > 4){
                leftStart -= 3;
                leftEnd += 3;
            }
        }
    }

    for (row = IMG_H -16 ; row >0; --row){                  //���ұߺ���
        getRightBlack = 0;

        rightStart -= 3;
        rightEnd += 3;
        if (rightStart < 0) rightStart = 0;
        if (rightEnd > IMG_W -1) rightEnd = IMG_W-1;    //�����������Խ��

        for (col = rightStart; col < rightEnd; ++col){          //������������
            if (img[row][col]!=0 ){                             //�ҵ�����
                rightBlack[row] = col;
                rightStart = col;
                rightEnd = col;
                getRightBlack = 1;
                if(rightLostCnt == 1){                       //����
                    rightBlack[row+1] = (rightBlack[row+2]+rightBlack[row]) /2;
                }
                if(rightLostCnt == 2){                       //����
                    rightBlack[row+1] = rightBlack[row] - (rightBlack[row]-rightBlack[row+3])/3;
                    rightBlack[row+2] = rightBlack[row+3] + (rightBlack[row]-rightBlack[row+3])/3;
                }
                if(rightLostCnt == 3){
                    rightBlack[row+1] = rightBlack[row] - (rightBlack[row]-rightBlack[row+4])/4;
                    rightBlack[row+2] = rightBlack[row+4] + (rightBlack[row]-rightBlack[row+4])/2;
                    rightBlack[row+3] = rightBlack[row+4] + (rightBlack[row]-rightBlack[row+4])/4;
                }
                rightLostCnt = 0;
                break;
            }
        }
        if (getRightBlack != 1){                            //û���ҵ�����
            rightLostCnt ++;
            if(leftLostCnt > 4){
                leftStart += 3;
                leftEnd -= 3;
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
    leftLostRow =0;
    rightLostRow =0;
    for (int row = IMG_H-6; row > 0; --row)  {
        middle[row] = (leftBlack[row] + rightBlack[row]) /2;

        if(leftBlack[row] ==255 && rightBlack[row] != 255){     //��ʧ����
            middle[row] = rightBlack[row] - jiaozheng[row];
            leftLostRow ++;
        }
        if(leftBlack[row] != 255 && rightBlack[row] == 255){     //��ʧ����
            middle[row] = leftBlack[row] + jiaozheng[row];
            rightLostRow ++;
        }
    }

    for (int row = IMG_H-2; row > 1; --row)  {      //ƽ������
        middle[row] = (middle[row-1] +middle[row+1])/2;
    }
}

/**
 *  ʹ����С���˷������ܵ�����
 *  �������:  BaseLine��ʼ�� < FinalLine��ֹ��
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

    for (i = BaseLine; i < FinalLine; ++i) {
        sumX += i;
        sumY += middle[i];
    }

    averageX = sumX / availableLines;
    averageY = sumY / availableLines;

    sumX = 0;
    sumY = 0;
    for (i = BaseLine; i < FinalLine; ++i) {
        sumX += (i-averageX)*(middle[i]-averageY);
        sumY += (i-averageX)*(i-averageX);
    }

    *k =  sumX / sumY;
    *b =  averageY - *k*averageX;   

    for (i = BaseLine; i < FinalLine; ++i) {
        error += (*k*i+*b - middle[i])*(*k*i+*b-middle[i]);
    }
    return error;
}

/*
 * �������ղ�ֵ����
 */
static int
lagrange(uint8 x0, uint8 x1, uint8 x2,uint8 y0,uint8 y1,uint8 y2,uint8 x)
{
    int tmp;
    tmp = (x-x1)*(x-x2)/((x0-x1)*(x0-x2))*y0 +
            (x-x0)*(x-x2)/((x1-x0)*(x1-x2))*y1+
            (x-x0)*(x-x1)/((x2-x0)*(x2-x1))*y2;
    return tmp;
}

/*
 * ʹ���������ղ�ֵ�������
 */
void
imgLagrange(int row1,int row2,int row3)
{
    for(int row=0;row< IMG_H; ++row){
        middle2[row] = lagrange(row1, row2, row3, middle[row1], middle[row2], middle[row3], row);
    }
}

/*
 * ţ�� �����д�Ŀ�ƽ������
 */
static float 
carmaSqrt(float x)      
{
    float xhalf = 0.5f*x;
    int i = *(int*)&x;      // get bits for floating VALUE
    i = 0x5f3759df- (i>>1); // gives initial guess y0
    x = *(float*)&i;        // convert bits BACK to float
    x = x*(1.5f-xhalf*x*x); // Newton step, repeating increases accuracy
    return (float)1/x;
}


/**
 * ʹ��������������߷��������
 * ����ֵ�� ����
 */
int
imgArea(int row1,int row2, int row3,int *area)
{
    int k;        //����
    int32 AB=0, BC=0, AC=0;
    *area = ((row2-row1)*(middle[row3]-middle[row1])-(row3-row1)*(middle[row2]-middle[row1]))/2;

    AB = carmaSqrt((row1-row2)*(row1-row2)+(middle[row1]-middle[row2])*(middle[row1]-middle[row2]));
    BC = carmaSqrt((row2-row3)*(row2-row3)+(middle[row2]-middle[row3])*(middle[row2]-middle[row3]));
    AC = carmaSqrt((row1-row3)*(row1-row3)+(middle[row1]-middle[row3])*(middle[row1]-middle[row3]));
    k = 4*(*area)/(AB*BC*AC);

    return k;
}

/*
 * �����߼��
 */
int
imgStartLine(void)
{

}