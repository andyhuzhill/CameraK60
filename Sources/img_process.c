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

uint8 nrf_buff[CAMERA_SIZE + MAX_ONCE_TX_NUM];
uint8 *srcImg = (uint8 *)(((uint8 *)&nrf_buff)+COM_LEN);                        //��������ͷ�ɼ�����
static vuint8 img[IMG_H][IMG_W];                         //������ͷ�ɼ���������������
static int8 leftLostRow=0, rightLostRow =0;              //���ұ��߶�ʧ������

static int8 lostRow;
static int8 startRow;

static int8 maxspeed = 8;
static int8 minspeed = 4;

////// �ⲿ������������
extern volatile IMG_STATE img_flag;

void
outimg(uint8*imgpointer)
{
	srcImg = imgpointer;	
}


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
	static int ret;
	int error=10;

	Site_t steersign = {0, 65};
	Site_t steersite = {5, 65};

	Site_t avrsite = {5,80};

	Site_t speedsign = {0, 95};
	Site_t speedsite = {5,95};

	Site_t pointsite = {0,0};

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

	//	imgGetImg();

	//	if(IMG_FINISH == img_flag)  {      // ��ͼ��ɼ���� ��ʼ����ͼ��
	//		img_flag = IMG_PROCESS;

	imgspeed = 0;

	imgResize();
	imgFilter();
	imgFindLine();
	imgGetMidLine();

	b = MAX(lostRow+5,3);
	if (b >= 50) b = 3;
	for(i= b ;i<50;i++){
		sum += middle[i];
		pointsite.x = middle[i];
		pointsite.y = i;
		LCD_Point(pointsite, RED);
	}

	for(i = 0; i< 60; i++){
		pointsite.y = i;
		if(i%10 == 0){
			for(int j=0;j<80;j++){
				pointsite.x = j;
				
				LCD_Point(pointsite, BLUE);
			}
		}
		pointsite.x = leftBlack[i];
		LCD_Point(pointsite, YELLOW);
		pointsite.x = rightBlack[i];
		LCD_Point(pointsite, GREEN);
	}

	average = sum / (50-b);

	// ɽկ���ƴ��㷨
	error = average - IMG_MID;
	if(ABS(error) <= 2){
		maxspeed = 7;
	}else{
		maxspeed = 6;
	}

	if(ABS(error) <= 3){
		pidSteer.kp = error*error/5+10;
		pidSteer.kd = 300;
	}else{
		pidSteer.kp = error*error/5+30;
		pidSteer.kd = 700;
	}
	ret = steerUpdate(error);

	ret += FTM_PRECISON/2;
	if(ret >= 0){
		LCD_Char(steersign, ' ', WHITE, WHITE);
	}else{
		ret = - ret;
		LCD_Char(steersign, '-', RED, WHITE);
	}
	LCD_Num_C(steersite, ret, RED, WHITE);


	ret = maxspeed - error*error*(maxspeed-minspeed)/(1600);

	if(ret >= 0){
		LCD_Char(speedsign, ' ', WHITE, WHITE);
	}else{
		ret = -ret;
		LCD_Char(speedsign, '-', BLACK, WHITE);
	}

	LCD_Num_C(speedsite, lostRow,  BLACK, WHITE);
	LCD_Num_C(avrsite, average, RED, WHITE);	


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

	row = IMG_H -3;
	getRightBlack = getLeftBlack = 0;
	do{
		for (col = IMG_W/2 ; col >= 0; --col) {  // ������ߺ���
			if ((img[row][col] != 0) && (img[row][col+1]==0)){
				leftBlack[row] = col;               //��¼�º��ߵ�����
				if(col <= 20 || (ABS(leftBlack[row+1]-col) <= 20)){
					getLeftBlack ++;
					break;
				}
			}
		}
		if((leftBlack[row] == -1) && (leftBlack[row + 1] != -1)){
			leftBlack[row] = leftBlack[row+1];
		}

		for (col = IMG_W/2; col <= (IMG_W -1); ++col) {  // �����ұߺ���
			if ((img[row][col] != 0) && (img[row][col-1] == 0)){     //���ֺ���
				rightBlack[row] = col;   //��¼�º��ߵ�����
				if(col >= 60 || (ABS(rightBlack[row+1]-col) <= 20)){
					getRightBlack ++;
					break;
				}
			}
		}
		if((rightBlack[row] == IMG_W) && (rightBlack[row+1] != IMG_W)){
			rightBlack[row] = rightBlack[row+1];
		}
		row --;
	}while(getLeftBlack != 5 && getRightBlack != 5);

	leftStart = leftEnd = leftBlack[row+1];
	rightStart = rightEnd = rightBlack[row+1];

	do{  //����ߺ���
		getLeftBlack = 0;
		leftStart += 3;
		leftEnd -= 3;
		if (leftEnd < 0) leftEnd = 0;
		if (leftStart > IMG_W -1) leftStart = IMG_W-1;//�����������Խ��

		for (col = leftStart; col >= leftEnd ; --col) { //������������
			if ((img[row][col] != 0) && (img[row][col+1] == 0) ){//�ҵ�����
				leftBlack[row] = col;
				leftStart = leftEnd = col ;
				getLeftBlack = 1;
				break;  //�ҵ������˳�forѭ��
			}
		}
		if(getLeftBlack != 1){ //û���ҵ�����
			leftBlack[row] = leftBlack[row+1]+ (leftBlack[row+1] -leftBlack[row+4])/3;
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
			if ((img[row][col]!=0) && (img[row][col-1] == 0) ){ //�ҵ�����
				rightBlack[row] = col;
				rightStart = rightEnd = col;
				getRightBlack = 1;
				break;      //����forѭ��
			}
		}
		if (getRightBlack != 1){ //û���ҵ�����
			rightBlack[row] = rightBlack[row+1] + (rightBlack[row+1]- rightBlack[row+4])/3;

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
#if 0
	int8_t getLeft=0, getRight=0;
	int8_t row, col;
	int8_t start, end;
	int8_t slop1, slop2;

	memset(leftBlack, -1, sizeof(leftBlack));
	memset(rightBlack, IMG_W, sizeof(rightBlack));
	memset(middle, (IMG_W/2), sizeof(middle));

	lostRow = 3;

	for(row=IMG_H-1; row > (IMG_H-8); --row){
		getLeft = getRight = 0;

		for(col=IMG_W/2; col>0; --col){
			if((img[row][col] == 0) && (img[row][col-1] != 0)){
				getLeft = 1;
				leftBlack[row] = col - 1;
				break;
			}
		}
		if(!getLeft){
			for(col=IMG_W/2; col < IMG_W; ++col){
				if(img[row][col] != 0 && (img[row][col+1] == 0)){
					getLeft = 1;
					leftBlack[row] = col;
					break;
				}
			}
		}

		for(col=IMG_W/2; col < IMG_W; ++col){
			if((img[row][col]==0) && (img[row][col+1] != 0)){
				getRight = 1;
				rightBlack[row] = col + 1;
				break;
			}
		}
		if(!getRight){
			for(col=IMG_W/2; col > 0; --col){
				if((img[row][col] != 0) && (img[row][col-1] == 0)){
					getRight = 1;
					rightBlack[row] = col;
					break;
				}
			}
		}

		if(getLeft && getRight && (leftBlack[row] < rightBlack[row]) &&
				(ABS(rightBlack[row]-leftBlack[row]) > 20)){            //�ҵ����ߺ���
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
		end     = leftBlack[row+1] -3;
		if (end < 0) end = 0;
		for(col=start; col > end; --col){   //��������������
			if((img[row][col] == 0) && (img[row][col-1] != 0)){
				getLeft = 1;
				leftBlack[row] = col-1;

				break;
			}
		}

		end = rightBlack[row+1] + 3;
		if (end > IMG_W-1){
			end = IMG_W-1;
		}

		for(col=start; col < end; ++col){   //��������������
			if((img[row][col] == 0) && (img[row][col+1] != 0)){
				getRight = 1;
				rightBlack[row] = col+1;
				break;
			}
		}

		if(getLeft && getRight && (leftBlack[row] < rightBlack[row]) && ((rightBlack[row]-leftBlack[row]) > 10)){            //�ҵ����ߺ���
			middle[row] = (leftBlack[row]+rightBlack[row])/2;
		}else if(getLeft && !getRight &&
				(((leftBlack[row]-leftBlack[row+1]) > -10) && ((leftBlack[row] -leftBlack[row+1])) < 10)
		){     //��ʧ�ұߺ���
			middle[row] = middle[row+1] + (leftBlack[row] - leftBlack[row+1]);
		}else if(getLeft && !getRight){
			middle[row] = (leftBlack[row] + IMG_W) / 2;
		}
		else if(!getLeft && getRight &&
				(((rightBlack[row]-rightBlack[row+1]) > -10) && ((rightBlack[row] -rightBlack[row+1])) < 10)
		){     //��ʧ��ߺ���
			middle[row] = middle[row+1] + (rightBlack[row] - rightBlack[row+1]);
		}else if(!getLeft && getRight){
			middle[row] = rightBlack[row] / 2;
		}
		else if(!getLeft && !getRight){    //���߶���
			// ??
			middle[row] = middle[row+1];
		}

		if((middle[row] <3) || (middle[row] > (IMG_W-2))){
			lostRow = row;
			return;
		}
	}

	for(row=IMG_H-4;row>=lostRow+2; ++row){
		middle[row] = (middle[row-1]+middle[row]+middle[row+1])/3;
	}
#else

	lostRow = 3;

	memset((void *)middle, 0 , sizeof(middle));

	for (int row = IMG_H-8; row > 0; --row) {
		if(leftBlack[row] != -1 && rightBlack[row] != IMG_W && (leftBlack[row] < rightBlack[row])){
			middle[row] = (leftBlack[row] + rightBlack[row])/2;
			continue;
		}else if(leftBlack[row] <= -1 && rightBlack[row] != IMG_W){     //��ʧ����
			if(row > 50){
				middle[row] = rightBlack[row] / 2 ;
			}else{
				middle[row] = middle[row+1] + (rightBlack[row+1] - rightBlack[row+2]);
			}
		}else if(leftBlack[row] != -1 && rightBlack[row] >= IMG_W-1){     //��ʧ����
			if(row > 50){
				middle[row] = (leftBlack[row]+IMG_W) /2 ;
			}else{
				middle[row] = middle[row+1] + (leftBlack[row+1] - leftBlack[row+2]);
			}
		}
	}

	for(int row = IMG_H-8; row > 1; --row){
		middle[row]= (middle[row+1]+middle[row] + middle[row-1])/3;

		if(middle[row]<3 || middle[row] > (IMG_W-3)){
			if(lostRow == 3){
				lostRow = row;
			}
		}
	}

#endif
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
int
imgStartLine(void)
{
	return 0;
}