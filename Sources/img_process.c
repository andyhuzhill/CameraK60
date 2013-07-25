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
#include "string.h"				//为了使用memset函数

//// 本模块公共变量声明
static int8 leftBlack[IMG_H] = {0};						//记录左边黑线位置
static int8 rightBlack[IMG_H]={0};						//记录右边黑线位置
static int8 middle[IMG_H] = {0};                        //记录中线位置

static uint8 nrf_buff[CAMERA_SIZE + MAX_ONCE_TX_NUM];	//无线发送用的数组
static uint8 *srcImg = (uint8 *)(((uint8 *)&nrf_buff)+COM_LEN); //保存摄像头采集数据
static vuint8 img[IMG_H][IMG_W];                         //将摄像头采集数据另存入此数组
static int8 leftLostRow=0, rightLostRow =0;              //左右边线丢失的行数


static int8 lostRow;					//图像丢线的行数
static int8 startRow;

int8 maxspeed ;							//设置最大速度
int8 minspeed ;							//设置最小速度

////// 外部公共变量声明
extern volatile IMG_STATE img_flag;		//图像采集状态变量
extern vint32 imgspeed;					//用来测试图像处理程序的速度的变量(调试用)
extern speedChoice choice;				//速度控制策略
extern int32 imgcount;					//计算图像采集的场数, 用来跳过一开始的起跑线检测
extern PidObject pidSteer;				//舵机PID控制结构体
extern int8 isStraight;					//判断是否是直道 1 为直道 0 为非直道

vint8 startLine = 0;

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


int
imgProcess(void)
{
	static int32 ret;			//返回速度控制量
	int error = 0;				//计算图像中心与摄像头中心的偏差
	int sum = 0;				//求和用的临时变量
	int8 average;				//计算所有有效行的图像中心的平均值
	
#ifdef SENDIMG					//如果定义了 SENDIMG 宏 将使用AT2401无线模块发送图像数据
	int8 status = 0;
#endif

#ifdef SDCARD					//如果定义了 SDCARD 宏, 将使用TF卡保存图像数据
	FATFS fs;
	FIL file;
	int res;
	uint32 filesize;

	f_mount(0,&fs);
#endif

	imgGetImg();

	if(IMG_FINISH == img_flag)  {      // 当图像采集完毕 开始处理图像
		img_flag = IMG_PROCESS;

		imgcount ++;

		imgResize();					//将图像解压保存到IMG_W x IMG_H 的数组中方便处理
		img_flag = IMG_READY;
		imgGetImg();					//此时图像已经解压完成,可以进行下一次的DMA图像采集 减少图像采集的时间间隔

		imgFilter();					// 滤除图像部分杂乱的点
		imgFindLine();					// 找到跑道的左右两边黑线
		imgGetMidLine();				// 找到跑道的中心线
		average = imgAverage(MAX(lostRow+4, 5), 50);  //计算有效行的中心线平均值

#ifdef SENDIMG
		NRF_MSG_send(COM_IMG, nrf_buff);

		do{
			status = NRF_MSG_send_state();
		}while(status == TX_ISR_SEND);

#endif

		if((imgcount >= 500) && 			// 超过发车时的起跑线
				startLine && isStraight &&  // 设置为检测起跑线 且目前的图像没有补线 不是小S虚线或十字路口
				(ABS(average-IMG_MID)<=3)){ // 当前图像为直道     
			imgStartLine();					// 检测起跑线 如果检测到将刹车
		}

		error = average - IMG_MID;			// 计算图像中心与摄像头中心的偏差值

		//角度控制
		if(ABS(error) <= 3){					//如果偏差值较小
			pidSteer.kp = error*error/8 + 20;   //舵机控制的P值较小
			pidSteer.kd = 300;					//舵机控制的D值较小
		}else{									//如果偏差值较大
			pidSteer.kp = error*error/5 + 50;   //舵机控制的P值较大	
			pidSteer.kd = 400;					//舵机控制的D值较大
		}

		ret = steerUpdate(error);				//使用位置式PID计算舵机输出值
		ret += STEER_MID;
		steerSetDuty(ret);						//输出舵机控制值

		//速度控制
		error = imgAverage(lostRow, lostRow+4)-IMG_MID;		//计算较远处中心线的平均值
		ret = maxspeed - (maxspeed-minspeed)*(error)*(error)/1600;	//计算速度控制量

#ifdef SDCARD		//按指定格式保存图像
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

#ifdef SERIALIMG		//按指定格式发送图像
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
 * 将以字节保存八个像素点的图像数据 解压为一个字节一个像素点的数组
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
	int8_t row, col;

	int8_t leftStart, leftEnd, rightStart, rightEnd; //每次左右边线扫描的起止点
	int8_t getLeftBlack=0, getRightBlack=0;  //标志是否找到黑线
	int8_t leftLostCnt =0, rightLostCnt=0;   //记录下丢失左右边线的次数
	int8_t slop1, slop2;

	memset((void *)leftBlack, -1, sizeof(leftBlack));
	memset((void *)rightBlack, IMG_W, sizeof(rightBlack));

	isStraight = 1;

	row = IMG_H -3;				//从倒数第三行开始往上扫描
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

		for (col = IMG_W/2; col <= (IMG_W -1); ++col) {  // 再找右边黑线
			if ((img[row][col] != 0) && (img[row][col-1] == 0)){     //发现黑线
				rightBlack[row] = col;   //记录下黑线的列数
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

	do{  //找左边黑线
		getLeftBlack = 0;
		leftStart += 3;
		leftEnd -= 3;
		if (leftEnd < 0) leftEnd = 0;
		if (leftStart > IMG_W -1) leftStart = IMG_W-1;//避免数组访问越界

		for (col = leftStart; col >= leftEnd ; --col) { //从右向左搜索
			if ((img[row][col] != 0) && (img[row][col+1] == 0) ){//找到黑线
				leftBlack[row] = col;
				if((ABS(leftBlack[row]-leftBlack[row+5]) > 20) && leftBlack[row+5] != -1){
					leftBlack[row] = -1;
					if(row > 10 && row < 50){
						isStraight = 0;			//如果有补线 就说明不是直道
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
				break;  //找到黑线退出for循环
			}
		}
		if(getLeftBlack != 1){ //没有找到黑线
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

		getRightBlack = 0;    //找右边黑线
		rightStart -= 3;
		rightEnd += 3;
		if (rightStart < 0) rightStart = 0;
		if (rightEnd > IMG_W -1) rightEnd = IMG_W-1; //避免数组访问越界

		for (col = rightStart; col <= rightEnd; ++col){  //从左往右搜索
			if ((img[row][col]!=0) && (img[row][col-1] == 0) ){ //找到黑线
				rightBlack[row] = col;
				if(rightBlack[row] < leftBlack[row] || (ABS(rightBlack[row]-leftBlack[row]) < 10) || ((ABS(rightBlack[row]-rightBlack[row+5]) > 20) && rightBlack[row+5] != IMG_W)){
					rightBlack[row] = IMG_W;
					if(row > 10 && row < 50){
						isStraight = 0;			//如果有补线了 就说明不是直道
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
				break;      //跳出for循环
			}
		}
		if (getRightBlack != 1){ //没有找到黑线
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
 * 说明: 提取图像中线
 * 影响到的变量:  middle[];
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
		}else if(leftBlack[row] <= -1 && rightBlack[row] != IMG_W){     //丢失左线
			if(row > 50){
				middle[row] = rightBlack[row] / 2 ;
			}else if((rightBlack[row+1] != IMG_W) && (rightBlack[row+2] != IMG_W)){
				middle[row] = middle[row+1] + (rightBlack[row+1] - rightBlack[row+2]);
			}else{
				middle[row] = middle[row+1];
			}
		}else if(leftBlack[row] != -1 && rightBlack[row] >= IMG_W-1){     //丢失右线
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
	int8 row, col;
	int8 count = 0;		//记录跳变点个数
	int8 tiaobian[8] = {0};

	for(row = IMG_H-8; row > (15); --row){
		count = 0;
		for(col =0; col < IMG_W-1; ++col){
			if(img[row][col] != img[row][col+1]){
				tiaobian[count ++] = col;		//记录某一行的跳变点位置
			}
		}

		if(count == 6){				//如果跳变点数刚好为6个 (起跑线处跳变为6个)
			if(ABS(tiaobian[0] -leftBlack[row])<2 && ABS(tiaobian[5] -rightBlack[row])<2){ 
				//且 最左和最右 两个跳变点为找到的左右边线 说明是起跑线 则停车
				stopcar();
			}
		}
	}

	return 0;
}