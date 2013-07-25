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
#include "string.h"				//Ϊ��ʹ��memset����

//// ��ģ�鹫����������
static int8 leftBlack[IMG_H] = {0};						//��¼��ߺ���λ��
static int8 rightBlack[IMG_H]={0};						//��¼�ұߺ���λ��
static int8 middle[IMG_H] = {0};                        //��¼����λ��

static uint8 nrf_buff[CAMERA_SIZE + MAX_ONCE_TX_NUM];	//���߷����õ�����
static uint8 *srcImg = (uint8 *)(((uint8 *)&nrf_buff)+COM_LEN); //��������ͷ�ɼ�����
static vuint8 img[IMG_H][IMG_W];                         //������ͷ�ɼ���������������
static int8 leftLostRow=0, rightLostRow =0;              //���ұ��߶�ʧ������


static int8 lostRow;					//ͼ���ߵ�����
static int8 startRow;

int8 maxspeed ;							//��������ٶ�
int8 minspeed ;							//������С�ٶ�

////// �ⲿ������������
extern volatile IMG_STATE img_flag;		//ͼ��ɼ�״̬����
extern vint32 imgspeed;					//��������ͼ���������ٶȵı���(������)
extern speedChoice choice;				//�ٶȿ��Ʋ���
extern int32 imgcount;					//����ͼ��ɼ��ĳ���, ��������һ��ʼ�������߼��
extern PidObject pidSteer;				//���PID���ƽṹ��
extern int8 isStraight;					//�ж��Ƿ���ֱ�� 1 Ϊֱ�� 0 Ϊ��ֱ��

vint8 startLine = 0;

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
	static int32 ret;			//�����ٶȿ�����
	int error = 0;				//����ͼ������������ͷ���ĵ�ƫ��
	int sum = 0;				//����õ���ʱ����
	int8 average;				//����������Ч�е�ͼ�����ĵ�ƽ��ֵ
	
#ifdef SENDIMG					//��������� SENDIMG �� ��ʹ��AT2401����ģ�鷢��ͼ������
	int8 status = 0;
#endif

#ifdef SDCARD					//��������� SDCARD ��, ��ʹ��TF������ͼ������
	FATFS fs;
	FIL file;
	int res;
	uint32 filesize;

	f_mount(0,&fs);
#endif

	imgGetImg();

	if(IMG_FINISH == img_flag)  {      // ��ͼ��ɼ���� ��ʼ����ͼ��
		img_flag = IMG_PROCESS;

		imgcount ++;

		imgResize();					//��ͼ���ѹ���浽IMG_W x IMG_H �������з��㴦��
		img_flag = IMG_READY;
		imgGetImg();					//��ʱͼ���Ѿ���ѹ���,���Խ�����һ�ε�DMAͼ��ɼ� ����ͼ��ɼ���ʱ����

		imgFilter();					// �˳�ͼ�񲿷����ҵĵ�
		imgFindLine();					// �ҵ��ܵ����������ߺ���
		imgGetMidLine();				// �ҵ��ܵ���������
		average = imgAverage(MAX(lostRow+4, 5), 50);  //������Ч�е�������ƽ��ֵ

#ifdef SENDIMG
		NRF_MSG_send(COM_IMG, nrf_buff);

		do{
			status = NRF_MSG_send_state();
		}while(status == TX_ISR_SEND);

#endif

		if((imgcount >= 500) && 			// ��������ʱ��������
				startLine && isStraight &&  // ����Ϊ��������� ��Ŀǰ��ͼ��û�в��� ����СS���߻�ʮ��·��
				(ABS(average-IMG_MID)<=3)){ // ��ǰͼ��Ϊֱ��     
			imgStartLine();					// ��������� �����⵽��ɲ��
		}

		error = average - IMG_MID;			// ����ͼ������������ͷ���ĵ�ƫ��ֵ

		//�Ƕȿ���
		if(ABS(error) <= 3){					//���ƫ��ֵ��С
			pidSteer.kp = error*error/8 + 20;   //������Ƶ�Pֵ��С
			pidSteer.kd = 300;					//������Ƶ�Dֵ��С
		}else{									//���ƫ��ֵ�ϴ�
			pidSteer.kp = error*error/5 + 50;   //������Ƶ�Pֵ�ϴ�	
			pidSteer.kd = 400;					//������Ƶ�Dֵ�ϴ�
		}

		ret = steerUpdate(error);				//ʹ��λ��ʽPID���������ֵ
		ret += STEER_MID;
		steerSetDuty(ret);						//����������ֵ

		//�ٶȿ���
		error = imgAverage(lostRow, lostRow+4)-IMG_MID;		//�����Զ�������ߵ�ƽ��ֵ
		ret = maxspeed - (maxspeed-minspeed)*(error)*(error)/1600;	//�����ٶȿ�����

#ifdef SDCARD		//��ָ����ʽ����ͼ��
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

#ifdef SERIALIMG		//��ָ����ʽ����ͼ��
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
 * �����ֽڱ���˸����ص��ͼ������ ��ѹΪһ���ֽ�һ�����ص������
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

	int8_t leftStart, leftEnd, rightStart, rightEnd; //ÿ�����ұ���ɨ�����ֹ��
	int8_t getLeftBlack=0, getRightBlack=0;  //��־�Ƿ��ҵ�����
	int8_t leftLostCnt =0, rightLostCnt=0;   //��¼�¶�ʧ���ұ��ߵĴ���
	int8_t slop1, slop2;

	memset((void *)leftBlack, -1, sizeof(leftBlack));
	memset((void *)rightBlack, IMG_W, sizeof(rightBlack));

	isStraight = 1;

	row = IMG_H -3;				//�ӵ��������п�ʼ����ɨ��
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

		for (col = IMG_W/2; col <= (IMG_W -1); ++col) {  // �����ұߺ���
			if ((img[row][col] != 0) && (img[row][col-1] == 0)){     //���ֺ���
				rightBlack[row] = col;   //��¼�º��ߵ�����
				if(rightBlack[row] < leftBlack[row] || (ABS(rightBlack[row]-leftBlack[row]) < 10)){
					rightBlack[row] = IMG_W;
					continue;
				}
				if(col >= 60 || (ABS(rightBlack[row+1]-col) <= 20)){
					getRightBlack ++;
					break;
				}
			}
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
				if((ABS(leftBlack[row]-leftBlack[row+5]) > 20) && leftBlack[row+5] != -1){
					leftBlack[row] = -1;
					if(row > 10 && row < 50){
						isStraight = 0;			//����в��� ��˵������ֱ��
					}
					continue;
				}
				leftStart = leftEnd = col;
				if(leftBlack[row] != -1 && leftBlack[row+2] != -1 && leftBlack[row+4] != -1){
					slop1 = leftBlack[row] - leftBlack[row+2];
					slop2 = leftBlack[row+2] - leftBlack[row+4];
					if(slop1*slop2 < 0){
						leftBlack[row+1] = leftBlack[row+2] + slop2 /2;
						leftBlack[row]   = leftBlack[row+2] + slop2;
					}
				}

				getLeftBlack = 1;
				leftLostCnt = 0;
				break;  //�ҵ������˳�forѭ��
			}
		}
		if(getLeftBlack != 1){ //û���ҵ�����
			leftBlack[row] = leftBlack[row+1]+ (leftBlack[row+2] -leftBlack[row+4])/2;
			if(row > 10 && row < 50){
				isStraight = 0;
			}
			if(leftBlack[row] <= 0){
				leftBlack[row] = -1;
			}

			if(leftLostCnt > 3){
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
				if(rightBlack[row] < leftBlack[row] || (ABS(rightBlack[row]-leftBlack[row]) < 10) || ((ABS(rightBlack[row]-rightBlack[row+5]) > 20) && rightBlack[row+5] != IMG_W)){
					rightBlack[row] = IMG_W;
					if(row > 10 && row < 50){
						isStraight = 0;			//����в����� ��˵������ֱ��
					}
					continue;
				}
				rightStart = rightEnd = col;
				rightLostCnt = 0;
				getRightBlack = 1;
				if(rightBlack[row] != IMG_W && rightBlack[row+2] != IMG_W && rightBlack[row+4]!= IMG_W){
					slop1 = rightBlack[row] - rightBlack[row+2];
					slop2 = rightBlack[row+2] - rightBlack[row+4];
					if(slop1*slop2 < 0){
						rightBlack[row+1] = rightBlack[row+2] + slop2 /2;
						rightBlack[row]   = rightBlack[row+2] + slop2;
					}
				}
				break;      //����forѭ��
			}
		}
		if (getRightBlack != 1){ //û���ҵ�����
			rightBlack[row] = rightBlack[row+1] + (rightBlack[row+2]- rightBlack[row+4])/2;
			if(row > 10 && row < 50){
				isStraight = 0;
			}
			if(rightBlack[row] >= IMG_W){
				rightBlack[row] = IMG_W;
			}
			if(rightLostCnt > 3){
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
	lostRow = 3;

	memset((void *)middle, 0 , sizeof(middle));

	for (int row = IMG_H-5; row > 0; --row) {
		if(leftBlack[row] != -1 && rightBlack[row] != IMG_W && (leftBlack[row] < rightBlack[row])){
			middle[row] = (leftBlack[row] + rightBlack[row])/2;
			continue;
		}else if(leftBlack[row] <= -1 && rightBlack[row] != IMG_W){     //��ʧ����
			if(row > 50){
				middle[row] = rightBlack[row] / 2 ;
			}else if((rightBlack[row+1] != IMG_W) && (rightBlack[row+2] != IMG_W)){
				middle[row] = middle[row+1] + (rightBlack[row+1] - rightBlack[row+2]);
			}else{
				middle[row] = middle[row+1];
			}
		}else if(leftBlack[row] != -1 && rightBlack[row] >= IMG_W-1){     //��ʧ����
			if(row > 50){
				middle[row] = (leftBlack[row]+IMG_W) /2 ;
			}else if((leftBlack[row+1] != -1) && (leftBlack[row+2] != -1)){
				middle[row] = middle[row+1] + (leftBlack[row+1] - leftBlack[row+2]);
			}else{
				middle[row] = middle[row+1];
			}
		}else if(leftBlack[row] == -1 && rightBlack[row] == IMG_W){
			if(row > 50){
				middle[row] = IMG_MID;
			}else{
				middle[row] = middle[row+1];
			}
		}else{
			if((leftBlack[row+5] == -1) &&(rightBlack[row+1]!=IMG_W) &&(rightBlack[row+2] != IMG_W)){
				middle[row] = middle[row+1] + rightBlack[row+1] - rightBlack[row+2];
			}else if((rightBlack[row+5] == IMG_W)&&(leftBlack[row+1]!=-1) &&(leftBlack[row+2] != -1)){
				middle[row] = middle[row+1] + leftBlack[row+1] - leftBlack[row+2];
			}
		}
	}

	for(int row = IMG_H-5; row > 1; --row){
		middle[row]= (middle[row+1] + middle[row] + middle[row-1])/3;

		if(middle[row]<3 || middle[row] > (IMG_W-3)){
			if(lostRow == 3){
				lostRow = row;
			}
		}
	}
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
	int8 row, col;
	int8 count = 0;		//��¼��������
	int8 tiaobian[8] = {0};

	for(row = IMG_H-8; row > (15); --row){
		count = 0;
		for(col =0; col < IMG_W-1; ++col){
			if(img[row][col] != img[row][col+1]){
				tiaobian[count ++] = col;		//��¼ĳһ�е������λ��
			}
		}

		if(count == 6){				//�����������պ�Ϊ6�� (�����ߴ�����Ϊ6��)
			if(ABS(tiaobian[0] -leftBlack[row])<2 && ABS(tiaobian[5] -rightBlack[row])<2){ 
				//�� ��������� ���������Ϊ�ҵ������ұ��� ˵���������� ��ͣ��
				stopcar();
			}
		}
	}

	return 0;
}