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

static uint8 nrf_buff[CAMERA_SIZE + MAX_ONCE_TX_NUM];
static uint8 *srcImg = (uint8 *)(((uint8 *)&nrf_buff)+COM_LEN); //保存摄像头采集数据
static vuint8 img[IMG_H][IMG_W];                         //将摄像头采集数据另存入此数组
static int8 leftLostRow=0, rightLostRow =0;              //左右边线丢失的行数


static int8 lostRow;
static int8 startRow;

int8 maxspeed ;
int8 minspeed ;
static int8 getStartLine = 0;

////// 外部公共变量声明
extern volatile IMG_STATE img_flag;
extern vint32 imgspeed;
extern speedChoice choice;
extern int32 imgcount;

vint8 startLine =0;

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

	if(IMG_FINISH == img_flag)  {      // 当图像采集完毕 开始处理图像
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
 * 影响到的变量 img[]
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
 * 滤波 将孤立的噪声去掉
 * 影响到的变量: img[];
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
 * 找两边黑线
 * 影响到的变量 leftBlack[] 和 rightBlack[]
 */
__relocate_code__
void
imgFindLine(void)
{
#if 0
	int8_t row, col;

	int8_t leftStart, leftEnd, rightStart, rightEnd;
	int8_t getLeftBlack=0, getRightBlack=0;  //标志是否找到黑线
	int8_t leftLostCnt =0, rightLostCnt=0;

	memset((void *)leftBlack, -1, sizeof(leftBlack));
	memset((void *)rightBlack, IMG_W, sizeof(rightBlack));

	row = IMG_H -3;
	getRightBlack = getLeftBlack = 0;
	do{
		for (col = IMG_W*2/3 ; col >= 0; --col) {  // 先找左边黑线
			if ((img[row][col] != 0) && (img[row][col+1]==0)){
				leftBlack[row] = col;               //记录下黑线的列数
				getLeftBlack ++;
				break;
			}
		}
		if((leftBlack[row] == -1) && (leftBlack[row + 1] != -1)){
			leftBlack[row] = leftBlack[row+1];
		}

		for (col = IMG_W/3; col <= (IMG_W -1); ++col) {  // 再找右边黑线
			if ((img[row][col] != 0) && (img[row][col-1] == 0)){     //发现黑线
				rightBlack[row] = col;   //记录下黑线的列数
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

	do{  //找左边黑线
		getLeftBlack = 0;
		leftStart += 3;
		leftEnd -= 3;
		if (leftEnd < 0) leftEnd = 0;
		if (leftStart > IMG_W -1) leftStart = IMG_W-1;//避免数组访问越界

		for (col = leftStart; col >= leftEnd ; --col) { //从右向左搜索
			if ((img[row][col] != 0) && (img[row][col+1] == 0) ){//找到黑线
				leftBlack[row] = col;
				leftStart = leftEnd = col;
				getLeftBlack = 1;
				if(leftLostCnt == 1){  //补线
					leftBlack[row+1] = (leftBlack[row+2] + leftBlack[row]) /2;
				}
				if(leftLostCnt == 2){ //补线
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
				break;  //找到黑线退出for循环
			}
		}
		if(getLeftBlack != 1){ //没有找到黑线
			leftLostCnt ++;
			leftBlack[row] = -1;
			if(leftLostCnt > 4){
				leftStart -= 3;
				leftEnd += 3;
			}
		}

		getRightBlack = 0;    //找右边黑线
		rightStart -= 3;
		rightEnd += 3;
		if (rightStart < 0) rightStart = 0;
		if (rightEnd > IMG_W -1) rightEnd = IMG_W-1; //避免数组访问越界

		for (col = rightStart; col <= rightEnd; ++col){  //从左往右搜索
			if ((img[row][col]!=0) && (img[row][col-1] == 0) ){ //找到黑线
				rightBlack[row] = col;
				rightStart = rightEnd = col;
				getRightBlack = 1;
				if(rightLostCnt == 1){ //补线
					rightBlack[row+1] = (rightBlack[row+2]+rightBlack[row]) /2;
				}
				if(rightLostCnt == 2){  //补线
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
				break;      //跳出for循环
			}
		}
		if (getRightBlack != 1){ //没有找到黑线
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
	int8_t getLeftBlack=0, getRightBlack=0;  //标志是否找到黑线
	int8_t leftLostCnt =0, rightLostCnt=0;

	memset((void *)leftBlack, -1, sizeof(leftBlack));
	memset((void *)rightBlack, IMG_W, sizeof(rightBlack));

	row = IMG_H -3;
	getRightBlack = getLeftBlack = 0;
	do{
		for (col = IMG_W/2 ; col >= 0; --col) {  // 先找左边黑线
			if ((img[row][col] != 0) && (img[row][col+1]==0)){
				leftBlack[row] = col;               //记录下黑线的列数
				if(col <= 20 || (ABS(leftBlack[row+1]-col) <= 20)){
					getLeftBlack ++;
					break;
				}
			}
		}
		if((leftBlack[row] == -1) && (leftBlack[row + 1] != -1)){
			leftBlack[row] = leftBlack[row+1];
		}

		for (col = IMG_W/2; col <= (IMG_W -1); ++col) {  // 再找右边黑线
			if ((img[row][col] != 0) && (img[row][col-1] == 0)){     //发现黑线
				rightBlack[row] = col;   //记录下黑线的列数
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

	do{  //找左边黑线
		getLeftBlack = 0;
		leftStart += 3;
		leftEnd -= 3;
		if (leftEnd < 0) leftEnd = 0;
		if (leftStart > IMG_W -1) leftStart = IMG_W-1;//避免数组访问越界

		for (col = leftStart; col >= leftEnd ; --col) { //从右向左搜索
			if ((img[row][col] != 0) && (img[row][col+1] == 0) ){//找到黑线
				leftBlack[row] = col;
				leftStart = leftEnd = col ;
				getLeftBlack = 1;
				break;  //找到黑线退出for循环
			}
		}
		if(getLeftBlack != 1){ //没有找到黑线
			leftBlack[row] = leftBlack[row+1]+ (leftBlack[row+1] -leftBlack[row+4])/3;
			if(leftBlack[row] < -1){
				leftBlack[row] = -1;
			}
			if(leftLostCnt > 4){
				leftStart -= 3;
				leftEnd += 3;
			}
		}

		getRightBlack = 0;    //找右边黑线
		rightStart -= 3;
		rightEnd += 3;
		if (rightStart < 0) rightStart = 0;
		if (rightEnd > IMG_W -1) rightEnd = IMG_W-1; //避免数组访问越界

		for (col = rightStart; col <= rightEnd; ++col){  //从左往右搜索
			if ((img[row][col]!=0) && (img[row][col-1] == 0) ){ //找到黑线
				rightBlack[row] = col;
				rightStart = rightEnd = col;
				getRightBlack = 1;
				break;      //跳出for循环
			}
		}
		if (getRightBlack != 1){ //没有找到黑线
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
 * 说明: 提取图像中线
 * 影响到的变量:  middle[];
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
		}else if(leftBlack[row] <= -1 && rightBlack[row] != IMG_W){     //丢失左线
			if(row > 50){
				middle[row] = rightBlack[row] / 2 ;
			}else{
				middle[row] = middle[row+1] + (rightBlack[row+1] - rightBlack[row+2]);
			}
		}else if(leftBlack[row] != -1 && rightBlack[row] >= IMG_W-1){     //丢失右线
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
		}else if(leftBlack[row] == -1 && rightBlack[row] != IMG_W){     //丢失左线
			if(row > 50){
				middle[row] = rightBlack[row] / 2 ;
			}else{
				middle[row] = middle[row+1] + (rightBlack[row+1] - rightBlack[row+2]);
			}
		}else if(leftBlack[row] != -1 && rightBlack[row] == IMG_W){     //丢失右线
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
 * 求中线平均值
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
// *  使用最小二乘法计算跑道方向
// *  输入变量:  BaseLine起始行 < FinalLine终止行
// *  输出变量:  k, 斜率 b 常数项
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
 * 起跑线检测
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