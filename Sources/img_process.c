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

static uint8 nrf_buff[CAMERA_SIZE + MAX_ONCE_TX_NUM];
static uint8 *srcImg = (uint8 *)(((uint8 *)&nrf_buff)+COM_LEN); //��������ͷ�ɼ�����
static vuint8 img[IMG_H][IMG_W];                         //������ͷ�ɼ���������������
static int8 leftLostRow=0, rightLostRow =0;              //���ұ��߶�ʧ������

static int8 lostRow;
static int8 startRow;

static int8 maxspeed = 4;
static int8 minspeed = 4;
static int8 getStartLine = 0;

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

extern PidObject pidSteer;

extern vint32 imgspeed;
extern speedChoice choice;
extern int32 imgcount;

int
imgProcess(void)
{
	int8 b;
	static int32 ret;
	int error=10;

#ifdef SENDIMG
	int8 status = 0;
#endif

	int sum = 0;
	int8 average;
	int8 i;

#ifdef SDCARD
	FATFS fs;
	FIL file;
	int res;
	uint32 filesize;

	f_mount(0,&fs);
#endif

	imgGetImg();

	if(IMG_FINISH == img_flag)  {      // ��ͼ��ɼ���� ��ʼ����ͼ��
		img_flag = IMG_PROCESS;

		imgspeed = 0;
		imgcount ++;

		imgResize();
		imgFilter();
		imgGetMidLine();

		b = MAX(lostRow,3); 
		if (b >= 50) b = 3;
		for(i= b ;i<50;i++){
			sum += middle[i];
		}
		average = sum/(50-b);

#ifdef SENDIMG
		NRF_MSG_send(COM_IMG, nrf_buff);

		do{
			status = NRF_MSG_send_state();
		}while(status == TX_ISR_SEND);

#endif

		if((imgcount >= 500) && (ABS(average-IMG_MID)<=3)){
//			imgStartLine();
		}

		// ɽկ���ƴ��㷨
		error = average - IMG_MID;

		switch(choice){
		case LOWEST:
			maxspeed = 9;
			minspeed = 3;
			break;
		case MID:
			if(ABS(error) <= 3){
				maxspeed = 8;
			}else{
				maxspeed = 7;
			}
			break;
		case FASTER:
			if(ABS(error) <= 3){
				maxspeed = 10;
			}else{
				maxspeed = 7;
			}
			break;
		case FASTEST:
			maxspeed = 5;
			minspeed = 2;
			break;
		default:
			break;
		}

		pidSteer.kp = error*error/30 + 20;
		ret = steerUpdate(error);

		ret += FTM_PRECISON/2;
		steerSetDuty(ret);

		ret = maxspeed - (maxspeed-minspeed)*(error)*(error)/1600;

		GPIOD_PTOR |= (1 << 9);

		img_flag = IMG_READY;

#ifdef SDCARD
		res = f_open(&file, "0:/img.img", FA_OPEN_ALWAYS | FA_WRITE | FA_READ);

		filesize = f_size(&file);

		f_lseek(&file, filesize);

		f_printf(&file, "img\n");

		for(int row = 0; row < IMG_H; ++row){
			for(int col = 0; col < IMG_W; ++col){
				f_printf(&file, "%d,", img[row][col]);
			}
			f_printf(&file, "\n");
		}
		f_printf(&file, "\n");

		f_close(&file);
#endif 

#ifdef SERIALIMG
		printf("img\n");
		for (int row = 0; row < IMG_H; ++row) {
			for (int col = 0; col < IMG_W ; ++col)  {
				printf( "%d,",img[row][col]);
			}
			printf("\n");
		}
		printf("\n");

#endif
		return ret;
	}
	return ret;
}


/**
 * Ӱ�쵽�ı��� img[]
 */
__relocate_code__
void
imgResize(void)              
{
	int row, col, i;
	for(row=0; row<IMG_H; ++row){
		for(col=0; col<IMG_W/8; ++col){
			for(i=7;i>=0;--i){
				if(srcImg[row*IMG_W/8 + col] & (1 << i)){
					img[row][col*8+(7-i)] = 1;
				}else{
					img[row][col*8+(7-i)] = 0;
				}
			}
		}
	}
}

/**
 * �˲� ������������ȥ��
 * Ӱ�쵽�ı���: img[];
 */
__relocate_code__
void
imgFilter(void)
{
	int8 row, col;
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

/**
 * ˵��: ��ȡͼ������
 * Ӱ�쵽�ı���:  middle[];
 */
__relocate_code__
void
imgGetMidLine(void)
{
    int8_t getLeft=0, getRight=0;
    int8_t row, col;
    int8_t start, end;

    memset(leftBlack, -1, sizeof(leftBlack));
    memset(rightBlack, IMG_W, sizeof(rightBlack));
    memset(middle, (IMG_W/2), sizeof(middle));

    lostRow = 3;

    for(row=IMG_H-1; row > (IMG_H-8); --row){
        getLeft = getRight = 0;
        for(col=0;col<(IMG_W-1); ++col){
            if((img[row][col] != 0) && (img[row][col+1] == 0)){
                getLeft = 1;
                leftBlack[row] = col;
                break;
            }
        }

        for(col=IMG_W-1; col >= 1; --col){
            if((img[row][col]!=0) && (img[row][col-1] == 0)){
                getRight = 1;
                rightBlack[row] = col;
                break;
            }
        }

        if(getLeft && getRight && (leftBlack[row] < rightBlack[row]) &&
                (ABS(rightBlack[row]-leftBlack[row]) > 10)){            //�ҵ����ߺ���
            middle[row] = (leftBlack[row]+rightBlack[row])/2;
        }else if(getLeft && getRight &&
                 ((leftBlack[row] > rightBlack[row]) && leftBlack[row] > IMG_W/2)
                 ){
            middle[row] = rightBlack[row] / 2;
        }else if (getLeft && getRight &&
                  ((leftBlack[row] > rightBlack[row]) && rightBlack[row] < IMG_W/2)
                  ){
            middle[row] = (leftBlack[row] + IMG_W-1)/2;
        }else if(getLeft && !getRight){     //��ʧ�ұߺ���
            middle[row] = (leftBlack[row]+IMG_W)/2;
        }else if(!getLeft && getRight){     //��ʧ��ߺ���
            middle[row] = rightBlack[row]/2;
        }else if(!getLeft && !getRight){    //���߶���
            // ??
        }

        if((middle[row] <3) || (middle[row] > (IMG_W-2))){
            lostRow = row;
            return;
        }
    }

    for(row = (IMG_H-8); row > 0; --row){
        getLeft = getRight = 0;
        start   = middle[row+1];
        end     = 1;
        for(col=start; col >= end; --col){   //��������������
            if((img[row][col] == 0) && (img[row][col-1] != 0)){
                getLeft = 1;
                leftBlack[row] = col-1;
                break;
            }
        }

        end = IMG_W-1;

        for(col=start; col < end; ++col){   //��������������
            if((img[row][col] == 0) && (img[row][col+1] != 0)){
                getRight = 1;
                rightBlack[row] = col+1;
                break;
            }
        }

        if(getLeft && getRight && (leftBlack[row] < rightBlack[row]) && (ABS(rightBlack[row]-leftBlack[row]) > 10)){            //�ҵ����ߺ���
            middle[row] = (leftBlack[row]+rightBlack[row])/2;
        }else if(getLeft && !getRight){     //��ʧ�ұߺ���
            middle[row] = middle[row+1] + leftBlack[row+1] - leftBlack[row+2];
        }else if(!getLeft && getRight){     //��ʧ��ߺ���
            middle[row] = middle[row+1] + rightBlack[row+1] - rightBlack[row+2];
        }else if(!getLeft && !getRight){    //���߶���
            // ??
            middle[row] = middle[row+1];
        }

        if((middle[row] <3) || (middle[row] > (IMG_W-2))){
            lostRow = row;
            return;
        }
    }

    for(row=IMG_H-2;row>=lostRow; ++row){
        middle[row] = (middle[row-1]+middle[row]+middle[row+1])/3;
    }
}

//
///**
// *  ʹ����С���˷������ܵ�����
// *  �������:  BaseLine��ʼ�� < FinalLine��ֹ��
// *  �������:  k, б�� b ������
// */
//__relocate_code__
//void
//imgLeastsq(int8 BaseLine, int8 FinalLine, float *k, int8 *b)
//{
//	int32 sumX=0, sumY=0;
//	int32 averageX=0, averageY=0;
//	int8 i;
//	int8 availableLines = FinalLine - BaseLine;
//
//	if(availableLines == 0){
//		*k = 0;
//		*b = 0;
//		return ;
//	}
//
//	for (i = BaseLine; i < FinalLine; ++i) {
//		sumX += i;
//		sumY += middle[i];
//	}
//
//	averageX = sumX / availableLines;
//	averageY = sumY / availableLines;
//
//	sumX = 0;
//	sumY = 0;
//	for (i = BaseLine; i < FinalLine; ++i) {
//		sumX += (i-averageX)*(middle[i]-averageY);
//		sumY += (i-averageX)*(i-averageX);
//	}
//
//	if(sumY == 0){
//		*k = 0.0;
//	}else{
//		*k = (float) sumX / sumY;
//	}
//	*b = (int8) averageY -*k*averageX;
//}

/*
 * �����߼��
 */
__relocate_code__
void
imgStartLine(void)
{
	int8 row, col;
	int8 count = 0;
	int8 tiaobian[8] = {0};

	for(row = IMG_H-1; row > (2); --row){
		count = 0;
		for(col=0;col<(IMG_W-1); ++col){
			if(img[row][col]!= img[row][col+1]){
				tiaobian[count++] = row;
				if(count >= 5){
					if((ABS((tiaobian[2]-tiaobian[1])-(tiaobian[4]-tiaobian[3])) <= 2) //&&
//							((tiaobian[2]-tiaobian[1]) >= 15) &&
//							(tiaobian[3]-tiaobian[2] >= 15)
							)
					{
						stopcar();
						return ;
					}
				}
			}
		}
	}
}