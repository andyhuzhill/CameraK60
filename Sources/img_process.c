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
static uint8 *srcImg = (uint8 *)(((uint8 *)&nrf_buff)+COM_LEN);                        //��������ͷ�ɼ�����
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

int
imgProcess(void)
{
	int8 b;
	static int32 ret;
	int error=10;

#ifdef SENDIMG
	int8 status = 0 ;
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

		imgResize();
		imgFilter();
		imgFindLine();
		imgGetMidLine();
		imgStartLine();

		b = MAX(lostRow,3);
		if (b >= 50) b = 3;
		for(i= b ;i<50;i++){
			sum += middle[i];
		}
		average = sum/(50-b);

		// ɽկ���ƴ��㷨
		error = average - IMG_MID;
		if(ABS(error) <= 2){
			maxspeed = 9;
		}else{
			maxspeed = 7;
		}
		pidSteer.kp = error*error/2 + 120;
		ret = steerUpdate(error);

		ret += FTM_PRECISON/2;
		steerSetDuty(ret);

		ret = maxspeed - error*error*(maxspeed-minspeed)/(1600);

		GPIOD_PTOR |= (1 << 9);

		img_flag = IMG_READY;

#ifdef SENDIMG
		NRF_MSG_send(COM_IMG, nrf_buff);

		do{
			status = NRF_MSG_send_state();
		}while(status == TX_ISR_SEND);

#endif

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


/*
 * �����ߺ���
 * Ӱ�쵽�ı��� leftBlack[] �� rightBlack[]
 */
__relocate_code__
void
imgFindLine(void)
{
	int8_t row, col;

	int8_t leftStart, leftEnd, rightStart, rightEnd;
	int8_t getLeftBlack=0, getRightBlack=0;  //��־�Ƿ��ҵ�����
	int8_t leftLostCnt =0, rightLostCnt=0;

	memset((void *)leftBlack, -1, sizeof(leftBlack));
	memset((void *)rightBlack, IMG_W, sizeof(rightBlack));

	row = IMG_H -5;
	getRightBlack = getLeftBlack = 0;
	do{
		for (col = (IMG_W /2); col >= 0; --col) {  // ������ߺ���
			if (img[row][col] != 0){
				leftBlack[row] = col;               //��¼�º��ߵ�����
				getLeftBlack ++;
				break;
			}
		}

		for (col = (IMG_W /2)+1; col <= (IMG_W -1); ++col) {  // �����ұߺ���
			if (img[row][col] != 0){     //���ֺ���
				rightBlack[row] = col;   //��¼�º��ߵ�����
				getRightBlack ++;
				break;
			}
		}
		row --;
	}while(getLeftBlack != 5 && getRightBlack != 5);

	if(ABS(leftBlack[row+1] - rightBlack[row+1]) <= 3){
		if((leftBlack[row+1] - leftBlack[row+4]) > 0){    //������
			rightBlack[row+1] = rightBlack[row+2] = rightBlack[row+3] = rightBlack[row+4] = IMG_W;
		}
		if((leftBlack[row+1] - leftBlack[row+4]) < 0){    //������
			leftBlack[row+1] = leftBlack[row+2] = leftBlack[row+3] = leftBlack[row+4] = -1;
		}
	}

	startRow = row;

	leftStart = leftEnd = leftBlack[row+1];
	rightStart = rightEnd = rightBlack[row+1];

	do{  //����ߺ���
		getLeftBlack = 0;
		leftStart += 3;
		leftEnd -= 3;
		if (leftEnd < 0) leftEnd = 0;
		if (leftStart > IMG_W -1) leftStart = IMG_W-1;//�����������Խ��

		for (col = leftStart; col >= leftEnd ; --col) { //������������
			if (img[row][col] != 0 ){//�ҵ�����
				leftBlack[row] = col;
				leftStart = leftEnd = col;
				getLeftBlack = 1;
				if(leftLostCnt == 1){  //����
					leftBlack[row+1] = (leftBlack[row+2] + leftBlack[row]) /2;
				}
				if(leftLostCnt == 2){ //����
					leftBlack[row+1] = leftBlack[row]   - (leftBlack[row]-leftBlack[row+3])/3;
					leftBlack[row+2] = leftBlack[row+3] + (leftBlack[row]-leftBlack[row+3])/3;
				}
				if(leftLostCnt == 3 && ABS(col-rightBlack[row+1])>=3){
					leftBlack[row+1] = leftBlack[row]   - (leftBlack[row]-leftBlack[row+4])/4;
					leftBlack[row+2] = leftBlack[row+4] + (leftBlack[row]-leftBlack[row+4])/2;
					leftBlack[row+3] = leftBlack[row+4] + (leftBlack[row]-leftBlack[row+4])/4;
				}
				if(leftLostCnt == 4 && ABS(col-rightBlack[row+1])>=3){
					leftBlack[row+1] = leftBlack[row] - (leftBlack[row]-leftBlack[row+5])/5;
					leftBlack[row+2] = leftBlack[row] - (leftBlack[row]-leftBlack[row+5])*2/5;
					leftBlack[row+3] = leftBlack[row+5] + (leftBlack[row]-leftBlack[row+5])*3/5;
					leftBlack[row+4] = leftBlack[row+5] + (leftBlack[row]-leftBlack[row+5])*2/5;
					leftBlack[row+5] = leftBlack[row+5] + (leftBlack[row]-leftBlack[row+5])/5;
				}
				leftLostCnt = 0;
				break;  //�ҵ������˳�forѭ��
			}
		}
		if(getLeftBlack != 1){ //û���ҵ�����
			leftLostCnt ++;
			leftBlack[row] = -1;
			if(leftLostCnt > 4){
				leftStart -= 3;
				leftEnd += 3;
			}
		}

		getRightBlack = 0;    //���ұߺ���
		rightStart -= 3;
		rightEnd += 3;
		if (rightStart < 0) rightStart = 0;
		if (rightEnd > IMG_W -1) rightEnd = IMG_W-1; //�����������Խ��

		for (col = rightStart; col <= rightEnd; ++col){  //������������
			if (img[row][col]!=0  ){ //�ҵ�����
				rightBlack[row] = col;
				rightStart = rightEnd = col;
				getRightBlack = 1;
				if(rightLostCnt == 1){ //����
					rightBlack[row+1] = (rightBlack[row+2]+rightBlack[row]) /2;
				}
				if(rightLostCnt == 2){  //����
					rightBlack[row+1] = rightBlack[row] - (rightBlack[row]-rightBlack[row+3])/3;
					rightBlack[row+2] = rightBlack[row+3] + (rightBlack[row]-rightBlack[row+3])/3;
				}
				if(rightLostCnt == 3  && ABS(col-leftBlack[row+1])>=3){
					rightBlack[row+1] = rightBlack[row] - (rightBlack[row]-rightBlack[row+4])/4;
					rightBlack[row+2] = rightBlack[row+4] + (rightBlack[row]-rightBlack[row+4])/2;
					rightBlack[row+3] = rightBlack[row+4] + (rightBlack[row]-rightBlack[row+4])/4;
				}
				if(rightLostCnt == 4 && ABS(col-leftBlack[row+1])>=3){
					rightBlack[row+1] = rightBlack[row] - (rightBlack[row]-rightBlack[row+5])/5;
					rightBlack[row+2] = rightBlack[row] - (rightBlack[row]-rightBlack[row+5])*2/5;
					rightBlack[row+3] = rightBlack[row+5] + (rightBlack[row]-rightBlack[row+5])*3/5;
					rightBlack[row+4] = rightBlack[row+5] + (rightBlack[row]-rightBlack[row+5])*2/5;
					rightBlack[row+5] = rightBlack[row+5] + (rightBlack[row]-rightBlack[row+5])/5;
				}
				break;      //����forѭ��
			}
		}
		if (getRightBlack != 1){ //û���ҵ�����
			rightBlack[row] = IMG_W;
			rightLostCnt ++;
			if(rightLostCnt > 4){
				rightStart += 3;
				rightEnd -= 3;
			}
		}
		row --;
	}while(row > 0);
}


/**
 * ˵��: ��ȡͼ������
 * Ӱ�쵽�ı���:  middle[];
 */
__relocate_code__
void
imgGetMidLine(void)
{
	int leftCnt=0, rightCnt=0;
	lostRow = 3;
	int slop1 = 0, slop2 = 0;

	memset((void *)middle, IMG_W/2 , sizeof(middle));

	for (int row = IMG_H-8; row > 0; --row) {
		if(leftBlack[row] != -1 && rightBlack[row] != IMG_W && (leftBlack[row] < rightBlack[row])){
			middle[row] = (leftBlack[row] + rightBlack[row])/2;
			leftCnt = rightCnt = 0;
			continue;
		}else if(leftBlack[row] == -1 && rightBlack[row] != IMG_W){     //��ʧ����
			if(row > 50){
				middle[row] = rightBlack[row] / 2;
			}else{
				middle[row] = middle[row+1] + (rightBlack[row+1] - rightBlack[row+2]);
			}
		}else if(leftBlack[row] != -1 && rightBlack[row] == IMG_W){     //��ʧ����
			if(row > 50){
				middle[row] = (leftBlack[row]+IMG_W) /2;
			}else{
				middle[row] = middle[row+1] + (leftBlack[row+1] - leftBlack[row+2]);
			}
		}
	}

	for(int row = IMG_H-8; row > 1; --row){
		middle[row]= (middle[row+1]+middle[row-1])/2;

		if(middle[row]<3 || middle[row] > (IMG_W-3) || (ABS(middle[row]-middle[row+1])>=10)){
			if(lostRow == 3){
				lostRow = row;
			}
		}
	}
}


/**
 *  ʹ����С���˷������ܵ�����
 *  �������:  BaseLine��ʼ�� < FinalLine��ֹ��
 *  �������:  k, б�� b ������
 */
__relocate_code__
void
imgLeastsq(int8 BaseLine, int8 FinalLine, float *k, int8 *b)
{
	int32 sumX=0, sumY=0;
	int32 averageX=0, averageY=0;
	int8 i;
	int8 availableLines = FinalLine - BaseLine;

	if(availableLines == 0){
		*k = 0;
		*b = 0;
		return ;
	}

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

	if(sumY == 0){
		*k = 0.0;
	}else{
		*k = (float) sumX / sumY;
	}
	*b = (int8) averageY -*k*averageX;
}

/*
 * �����߼��
 */
__relocate_code__
void
imgStartLine(void)
{
	int8 row, col;
	int8 site[6]={0};
	int8 count = 0;
	int8 a,b,c,d,e;
	
	for(row = IMG_H; row < (IMG_H-6); --row){
		for(col=0;col<(IMG_W-1); ++col){
			if(img[row][col]!= img[row][col+1]){
				site[count++] = col;		//��¼�����λ��
				if(count >=6){
					break;
				}
			}
		}
		
		a = site[1] - site[0];
		b = site[2] - site[1];
		c = site[3] - site[2];
		d = site[4] - site[3];
		e = site[5] - site[4];
		
		if((ABS(b-a) <= 3 && ABS(c-b) <=3) ||
				(ABS(c-b)<=3 && ABS(d-c)<=3)){
			if(0 == getStartLine){
				getStartLine = 1;
			}else{
				stopcar();
			}
		}
	}
}