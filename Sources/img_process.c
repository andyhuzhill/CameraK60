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
static uint8 *srcImg = (uint8 *)(((uint8 *)&nrf_buff)+COM_LEN);                        //保存摄像头采集数据
static vuint8 img[IMG_H][IMG_W];                         //将摄像头采集数据另存入此数组
static int8 leftLostRow=0, rightLostRow =0;              //左右边线丢失的行数

static int8 lostRow;

////// 外部公共变量声明
extern volatile IMG_STATE img_flag;

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

extern vint32 imgspeed;

int
imgProcess(void)
{
	float k;
	int8 b;
	static int ret;
	int A = 40;
	int C = 10;
	union {
		float f;
		char ch[4];
	} ufc;
	
	int8 status = 0 ;

	int sum = 0;
	int average;
	int i;

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
		
		imgResize();
		imgFindLine();
		imgGetMidLine();

		b = MAX(lostRow,10);
		for(i= b ;i<40;i++){
			sum += middle[i];
		}
		average = sum / (40-b);
		
		// 北科大算法
		pidSteer.kp = (average-24)*(average-24)/2+10;
		ret = steerUpdate(average-25);
		ret += FTM_PRECISON/2;
		steerSetDuty(ret);
		ret = 10-(24-average)*(24-average)*(10-5)/576;

		//		imgLeastsq(MAX(lostRow,C), A, &k, &b);

		//		ret = middle[30];

		//		if(ABS(k) <= 0.1 && (ABS(ret - 25) < 2)){
		//			steerSetDuty(FTM_PRECISON/2);
		//			steerUpdate(ret -25);
		//			ret = 13;
		//		}else{
		//			if(b == 0){
		//				steerUpdate(ret -25);
		//				ret = 8;
		//			}else{
		//				ret = steerUpdate(ret - 25);
		//				ret += FTM_PRECISON/2;
		//				steerSetDuty(ret);
		//				ret = 6;
		//			}
		//		}

		GPIOD_PTOR |= (1 << 9);

		img_flag = IMG_READY;
		
#ifdef AT2401
		NRF_MSG_send(COM_IMG, nrf_buff);
		
		do{
			status = NRF_MSG_send_state();
		}while(status == TX_ISR_SEND);
		
		if(status == TX_ISR_SUCCEED){
			printf("img send success!\n");
		}else if ( status == TX_ISR_FAIL){
			printf("img send failed\n");
		}
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
 * 影响到的变量 img[]
 */
__relocate_code__
void
imgResize(void)              
{
#if 0
	int32 tmpIndex, tmpRow;
	int16 col,row;    
	int16 oldCol,oldRow; 

	for(row=0; row < IMG_H; row++) {
		oldRow = ((row * CAMERA_H  + (IMG_H >> 1) ) / IMG_H);
		//注释：row = (oldRow - 1/2)*(IH/CH)
		tmpRow = oldRow * CAMERA_W ;

		for(col=0;col<IMG_W;col++) {
			oldCol = ((col * CAMERA_W  + (IMG_W >> 1) ) / IMG_W);
			//注释：col = (oldCol - 1/2)*(IH/CH)
			tmpIndex = tmpRow + oldCol;
			if( (srcImg[tmpIndex>>3] & (1<<(7- (tmpIndex & 0x07))) ) == 0  ) {
				img[row][col] = 0;
			}else{
				img[row][col] = 1;
			}
		}
	}
#else
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
#endif
}

/**
 * 滤波 将孤立的噪声去掉
 * 影响到的变量: img[];
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
 * 找两边黑线
 * 影响到的变量 leftBlack[] 和 rightBlack[]
 */
__relocate_code__
void
imgFindLine(void)
{
	int8 row, col;

	int8 leftStart, leftEnd, rightStart, rightEnd;  
	int8 getLeftBlack=0, getRightBlack=0;             //标志是否找到黑线
	int8 leftLostCnt =0, rightLostCnt =0;

	memset(leftBlack, -1, sizeof(leftBlack));
	memset(rightBlack, IMG_W, sizeof(rightBlack));

	for (row = IMG_H -10; row > (IMG_H -16); --row){  //搜索前五行
		getLeftBlack = 0;
		getRightBlack = 0;
		for (col = (IMG_W /2); col >= 0; --col) {  // 先找左边黑线
			if (img[row][col] != 0){
				getLeftBlack = 1;
				leftBlack[row] = col;//记录下黑线的列数
				break;
			}
		}
		if (getLeftBlack != 1)  { //  没有发现黑线
			leftBlack[row] = -1;
		}

		for (col = (IMG_W /2); col <= (IMG_W -1); ++col) {  // 再找右边黑线
			if (img[row][col] != 0){     //发现黑线
				rightBlack[row] = col;   //记录下黑线的列数
				getRightBlack = 1;
				break;
			}
		}
		if (getRightBlack != 1) { // 没有发现黑线
			rightBlack[row] = IMG_W;
		}
	}

	//避免数组访问越界
	if(leftBlack[IMG_H-15] != -1){
		leftStart = leftBlack[IMG_H -15];
		leftEnd = leftBlack[IMG_H -15];
	}else{
		leftStart = 0;
		leftEnd = 0;
	}
	if(rightBlack[IMG_H-15] != IMG_W){
		rightStart = rightBlack[IMG_H -15];
		rightEnd = rightBlack[IMG_H -15];
	}else{
		rightStart = IMG_W-1;
		rightEnd = IMG_W-1;
	}

	for (row = IMG_H -16; row > 0 ; --row){  //找左边黑线
		getLeftBlack = 0;

		leftStart += 3;
		leftEnd -= 3;
		if (leftEnd < 0) leftEnd = 0;
		if (leftStart > IMG_W -1) leftStart = IMG_W-1;//避免数组访问越界

		for (col = leftStart; col >= leftEnd ; --col) { //从右向左搜索
			if (img[row][col] != 0 ){//找到黑线
				leftBlack[row] = col;
				leftStart = col;
				leftEnd = col;
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
				if((10 < row && row < 35) &&(leftLostCnt >= 4)){
					leftLostRow = row + leftLostCnt;
				}
			}
		}

		getRightBlack = 0;    //找右边黑线

		rightStart -= 3;
		rightEnd += 3;
		if (rightStart < 0) rightStart = 0;
		if (rightEnd > IMG_W -1) rightEnd = IMG_W-1; //避免数组访问越界

		for (col = rightStart; col <= rightEnd; ++col){  //从左往右搜索
			if (img[row][col]!=0  ){ //找到黑线
				rightBlack[row] = col;
				rightStart = col;
				rightEnd = col;
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
				rightLostCnt = 0;
				break;
			}
		}
		if (getRightBlack != 1){ //没有找到黑线
			rightBlack[row] = IMG_W;
			rightLostCnt ++;
			if(rightLostCnt > 4){
				rightStart += 3;
				rightEnd -= 3;
				if((10 < row && row < 35) &&(rightLostCnt >= 4)){
					rightLostRow = row + rightLostCnt;
				}
			}
		}
	}

	for (row = 2; row < IMG_W-2; ++row) {
		if(leftBlack[row+1] == -1 && leftBlack[row] != -1 && leftBlack[row-1] == -1){
			leftBlack[row] = -1;
		}
		if(rightBlack[row+1] == IMG_W && rightBlack[row] != IMG_W && rightBlack[row-1] == IMG_W){
			rightBlack[row] = IMG_W;
		}
	}
}


/**
 * 说明: 提取图像中线
 * 影响到的变量:  middle[];
 */
__relocate_code__
void
imgGetMidLine(void)
{
	int leftCnt=0, rightCnt=0;
	lostRow = 10;
	int slop1 = 0, slop2 = 0;

	memset((void *)middle, (IMG_W/2), sizeof(middle));
	for (int row = IMG_H-3; row > 2; --row)  {
		if(leftBlack[row] == rightBlack[row] || (leftBlack[row] > rightBlack[row])){
			if(lostRow == 10){
				lostRow = row;
			}
			if(leftBlack[row+3] != -1 && rightBlack[row+3] == IMG_W) {        //右线找到左线
				rightBlack[row] = IMG_W;
			}else if(rightBlack[row+3] != IMG_W && leftBlack[row+3] == -1){   //左线找到右线
				leftBlack[row] = -1;
			}else if((leftBlack[row+3] == -1) && (rightBlack[row+3] == IMG_W)){
				if((leftBlack[row-3] != -1) && (rightBlack[row-3] == IMG_W)){   //右线找到左线
					rightBlack[row] = IMG_W;
				}else if((leftBlack[row-3] == -1) && (rightBlack[row-3] != IMG_W)){ // 左线找到右线
					leftBlack[row] = -1;
				}else{
					return ;
				}
			}
		}

		if(leftBlack[row] ==-1 && rightBlack[row] != IMG_W){           //丢失左线
			leftCnt ++;
			if(row < 30 && leftCnt >= 2){
				middle[row] = (middle[row+1] + (rightBlack[row] - rightBlack[row+1]) +middle[row+1])/2;
			}else{
				middle[row] = (rightBlack[row]/2 + middle[row+1])/2;
			}
		}else if(leftBlack[row] != -1 && rightBlack[row] == IMG_W){     //丢失右线
			rightCnt ++;
			if(row < 30 && rightCnt >= 2){
				middle[row] = (middle[row+1] + (leftBlack[row] - leftBlack[row+1]) + middle[row+1])/2;
			}else{
				middle[row] = ((IMG_W+leftBlack[row])/2 + middle[row+1]) /2;
			}
		}else if((leftBlack[row] == -1) && (rightBlack[row] == IMG_W)){    //十字丢线
			middle[row] = middle[row+1];
		}

		if((middle[row] <= 3) || (middle[row] >= (IMG_W-3)) || (ABS(middle[row] - middle[row+1]) >= 10)&&(row < 30)){
			if(lostRow == 10){
				lostRow = row + 2;
			}
		}

		if(leftBlack[row] != -1 && rightBlack[row] != IMG_W){
			middle[row] = (leftBlack[row] + rightBlack[row]) /2;
			leftCnt = 0;
			rightCnt = 0;
		}
	}

	for (int row = IMG_H-2; row > 1; --row)  {      //平滑中线
		if(row < 35){
			slop1 = middle[row+2] - middle[row];
			slop2 = middle[row] - middle[row-2];

			if(slop1*slop2 < 0){       // middle[row]是拐点
				if((ABS(slop1)+ABS(slop2)) > 5){
					middle[row-1] = middle[row];
				}
			}
		}
		middle[row] = (middle[row-1] + middle[row+1])/2;
	}
}


/**
 *  使用最小二乘法计算跑道方向
 *  输入变量:  BaseLine起始行 < FinalLine终止行
 *  输出变量:  k, 斜率 b 常数项
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
 * 起跑线检测
 */
int
imgStartLine(void)
{
	return 0;
}