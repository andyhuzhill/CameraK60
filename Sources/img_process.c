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

int8 maxspeed ;
int8 minspeed ;
static int8 getStartLine = 0;

////// �ⲿ������������
extern volatile IMG_STATE img_flag;
extern vint32 imgspeed;
extern speedChoice choice;
extern int32 imgcount;

vint8 startLine =0;

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
		//		imgFilter();
		imgFindLine();
		imgGetMidLine();
		average = imgAverage(MAX(lostRow+5, 4), 50);

#ifdef SENDIMG
		NRF_MSG_send(COM_IMG, nrf_buff);

		do{
			status = NRF_MSG_send_state();
		}while(status == TX_ISR_SEND);

#endif
		//
		if(//(imgcount >= 500) && 
				(ABS(average-IMG_MID)<=3)){
			startLine = 1;
		}else{
			startLine = 0;
		}

		error = average - IMG_MID;

		if(ABS(error) <= 3){
			pidSteer.kp = error*error/8 + 10;
			pidSteer.kd = 300;
		}else{
			pidSteer.kp = error*error/5 + 50;
			pidSteer.kd = 500;
		}

		ret = steerUpdate(error);

		ret += FTM_PRECISON/2;
		steerSetDuty(ret);


		error = imgAverage(lostRow, lostRow+5)-IMG_MID;
		ret = maxspeed - (maxspeed-minspeed)*(error)*(error)/1600;

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
#if 0
	int8_t row, col;

	int8_t leftStart, leftEnd, rightStart, rightEnd;
	int8_t getLeftBlack=0, getRightBlack=0;  //��־�Ƿ��ҵ�����
	int8_t leftLostCnt =0, rightLostCnt=0;

	memset((void *)leftBlack, -1, sizeof(leftBlack));
	memset((void *)rightBlack, IMG_W, sizeof(rightBlack));

	row = IMG_H -3;
	getRightBlack = getLeftBlack = 0;
	do{
		for (col = IMG_W*2/3 ; col >= 0; --col) {  // ������ߺ���
			if ((img[row][col] != 0) && (img[row][col+1]==0)){
				leftBlack[row] = col;               //��¼�º��ߵ�����
				getLeftBlack ++;
				break;
			}
		}
		if((leftBlack[row] == -1) && (leftBlack[row + 1] != -1)){
			leftBlack[row] = leftBlack[row+1];
		}

		for (col = IMG_W/3; col <= (IMG_W -1); ++col) {  // �����ұߺ���
			if ((img[row][col] != 0) && (img[row][col-1] == 0)){     //���ֺ���
				rightBlack[row] = col;   //��¼�º��ߵ�����
				getRightBlack ++;
				break;
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
			if ((img[row][col]!=0) && (img[row][col-1] == 0) ){ //�ҵ�����
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
#else
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
			if(leftBlack[row] < -1){
				leftBlack[row] = -1;
			}
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
			if(rightBlack[row] > IMG_W){
				rightBlack[row] = IMG_W;
			}
			if(rightLostCnt > 4){
				rightStart += 3;
				rightEnd -= 3;
			}
		}
		row --;
	}while(row > 0);
#endif
}


/**
 * ˵��: ��ȡͼ������
 * Ӱ�쵽�ı���:  middle[];
 */
__relocate_code__
void
imgGetMidLine(void)
{
#if 1
	lostRow = 3;

	memset((void *)middle, IMG_MID , sizeof(middle));

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
#else
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
				middle[row] = rightBlack[row] / 2 ;
			}else{
				middle[row] = middle[row+1] + (rightBlack[row+1] - rightBlack[row+2]);
			}
		}else if(leftBlack[row] != -1 && rightBlack[row] == IMG_W){     //��ʧ����
			if(row > 50){
				middle[row] = (leftBlack[row]+IMG_W) /2 ;
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

#endif
}



/*
 * ������ƽ��ֵ
 * start < end
 */
__relocate_code__
int8
imgAverage(int8_t start, int8_t end)
{
	int sum=0, average=0;

	for(int i= start; i < end; ++i){
		sum += middle[i];
	}

	average = sum / (end -start);
	return average;
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
int
imgStartLine(void)
{

	if((GPIO_read_bit(PORT_B, 20) + GPIO_read_bit(PORT_B, 21) + GPIO_read_bit(PORT_B, 22) + GPIO_read_bit(PORT_B,23)) <= 2 ){
		stopcar();
	}
	//	int8 row, col;
	//	int8 count = 0;
	//	int8 tiaobian[8] = {0};
	//
	//	for(row = IMG_H-1; row > (2); --row){
	//		count = 0;
	//		for(col=0;col<(IMG_W-1); ++col){
	//			if(img[row][col]!= img[row][col+1]){
	//				tiaobian[count++] = row;
	//				if(count >= 5){
	//					if((ABS((tiaobian[2]-tiaobian[1])-(tiaobian[4]-tiaobian[3])) <= 2) //&&
	//							//							((tiaobian[2]-tiaobian[1]) >= 15) &&
	//							//							(tiaobian[3]-tiaobian[2] >= 15)
	//					)
	//					{
	//						stopcar();
	//						return ;
	//					}
	//				}
	//			}
	//		}
	//	}
}