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
static uint8 middle2[IMG_H] = {0};

static uint8 srcImg[CAMERA_SIZE];                        //保存摄像头采集数据
static vuint8 img[IMG_H][IMG_W];                         //将摄像头采集数据另存入此数组
static int8 leftLostRow=0, rightLostRow =0;              //左右边线丢失的行数

static int8 lostRow;

//static const int8 jiaozheng[IMG_H] = {6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 12, 
//		12, 13, 13, 14, 14, 15, 15, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21,
//		22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 28};

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

#ifdef SDCARD
	FATFS fs;
	FIL file;
	int res;
	uint32 filesize;

	f_mount(0,&fs);
#endif

	imgGetImg();

	DEBUG_OUT("get img\n",0);

	if(IMG_FINISH == img_flag)  {      // 当图像采集完毕 开始处理图像
		img_flag = IMG_PROCESS;
		DEBUG_OUT("img is under processing\n",0);
		imgResize();
		imgFindLine();
		imgGetMidLine();
		imgLeastsq(MAX(lostRow,C), A, &k, &b);

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

//		ufc.f = k;
//
//		f_printf(&file,"ufc=%d+%d+%d+%d\n",ufc.ch[0],ufc.ch[1],ufc.ch[2],ufc.ch[3]);

		f_close(&file);
		
#endif 

#ifdef SERIAL
		printf("img\n");
		for (int row = 0; row < IMG_H; ++row) {
			for (int col = 0; col < IMG_W ; ++col)  {
				printf( "%d,",img[row][col]);
			}
			printf("\n");
		}
		printf("\n");

		ufc.f = k;
		printf("ufc=%d+%d+%d+%d\n",ufc.ch[0],ufc.ch[1],ufc.ch[2],ufc.ch[3]);

#endif
		ret = k*25+b;
		if(ABS(k) <= 0.09 && (ABS(ret - 25) < 3)){
			steerSetDuty(500);
			motorSetSpeed(250);
		}else{
			if((k > 0) && ((ret) >25)){
				steerSetDuty(500);
				motorSetSpeed(150);
			}else if((k < 0) && ((ret) < 25)){
				steerSetDuty(500);
				motorSetSpeed(150);
			}else{
				motorSetSpeed(200);
				steerUpdate(ret-25);
			}
		}

		img_flag = IMG_READY;

		DEBUG_OUT("img is ready #################################\n",0);

		return ret;
	}
	return ret;
}


/**
 * 影响到的变量 img[]
 */
void
imgResize(void)              
{
	uint32 tmpIndex, tmpRow;
	uint16 col,row;    
	uint16 oldCol,oldRow; 

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
}

/**
 * 滤波 将孤立的噪声去掉
 * 影响到的变量: img[];
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


/*
 * 找两边黑线
 * 影响到的变量 leftBlack[] 和 rightBlack[]
 */

void
imgFindLine(void)
{
	int8 row, col;

	int8 leftStart, leftEnd, rightStart, rightEnd;  
	int8 getLeftBlack=0, getRightBlack=0;             //标志是否找到黑线
	int8 leftLostCnt =0, rightLostCnt =0;

	memset(leftBlack, -1, sizeof(leftBlack));
	memset(rightBlack, IMG_W, sizeof(rightBlack));
	memset((void *)middle, (IMG_W/2), sizeof(middle));

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
}


/**
 * 说明: 提取图像中线
 * 影响到的变量:  middle[];
 */
void
imgGetMidLine(void)
{
	int leftCnt=0, rightCnt=0;
	lostRow = 10;
	for (int row = IMG_H-3; row > 2; --row)  {

		if(leftBlack[row] != -1 && rightBlack[row] != IMG_W){
			middle[row] = (leftBlack[row] + rightBlack[row]) /2;
			leftCnt = 0;
			rightCnt = 0;
			continue;       //继续for循环
		}

		if(leftBlack[row] == rightBlack[row] || (leftBlack[row] > rightBlack[row])){
			lostRow = row;
			if(leftBlack[row+5] != -1 && rightBlack[row+5] == IMG_W) {        //右线找到左线
				rightLostRow = row;
				rightBlack[row] = IMG_W;
			}else if(rightBlack[row+5] != IMG_W && leftBlack[row+5] == -1){   //左线找到右线
				leftLostRow = row;
				leftBlack[row] = -1;
			}
		}

		if(leftBlack[row] ==-1 && rightBlack[row] != IMG_W){           //丢失左线
			leftCnt ++;
			if(row < 30 && leftCnt >= 2){
				middle[row] = middle[row+1] + (rightBlack[row] - rightBlack[row+1]);
			}else{
				middle[row] = rightBlack[row]/2;
			}
		}else if(leftBlack[row] != -1 && rightBlack[row] == IMG_W){     //丢失右线
			rightCnt ++;
			if(row < 30 && rightCnt >= 2){
				middle[row] = middle[row+1] + (leftBlack[row] - leftBlack[row+1]);
			}else{
				middle[row] = (IMG_W+leftBlack[row])/2;
			}
		}else if((leftBlack[row] == -1) && (rightBlack[row] == IMG_W)){    //十字丢线
			middle[row] = middle[row+1] ;
		}

		if(leftCnt >=25 && rightCnt >= 25){
			stopcar();
		}

		if((middle[row] <= 3) || (middle[row] >= (IMG_W-3))){
			if(lostRow == 10){
				lostRow = row;
			}
		}
	}

	for (int row = IMG_H-2; row > 1; --row)  {      //平滑中线
		middle[row] = (middle[row-1] + middle[row+1])/2;
	}
}


/**
 *  使用最小二乘法计算跑道方向
 *  输入变量:  BaseLine起始行 < FinalLine终止行
 *  输出变量:  k, 斜率 b 常数项
 */
void
imgLeastsq(int8 BaseLine, int8 FinalLine, float *k, int8 *b)
{
	int32 sumX=0, sumY=0;
	int32 averageX=0, averageY=0;
	int8 i;
	int8 availableLines = FinalLine - BaseLine;
	//	float error=0;

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
	//
	//	for (i = BaseLine; i < FinalLine; ++i) {
	//		error += (*k*i+*b - middle[i])*(*k*i+*b-middle[i]);
	//	}

	//	return error;
}
///*
// * 拉格朗日差值函数
// */
//static int
//lagrange(uint8 x0, uint8 x1, uint8 x2,uint8 y0,uint8 y1,uint8 y2,uint8 x)
//{
//	int tmp;
//	tmp = (x-x1)*(x-x2)/((x0-x1)*(x0-x2))*y0 +
//			(x-x0)*(x-x2)/((x1-x0)*(x1-x2))*y1+
//			(x-x0)*(x-x1)/((x2-x0)*(x2-x1))*y2;
//	return tmp;
//}
//
///*
// * 使用拉格朗日插值拟合曲线
// */
//void
//imgLagrange(int row1,int row2,int row3)
//{
//	for(int row=0;row< IMG_H; ++row){
//		middle2[row] = lagrange(row1, row2, row3, middle[row1], middle[row2], middle[row3], row);
//	}
//}
//
///*
// * 牛人 卡马克写的开平方函数
// */
//static float 
//carmaSqrt(float x)      
//{
//	float xhalf = 0.5f*x;
//	int i = *(int*)&x;      // get bits for floating VALUE
//	i = 0x5f3759df- (i>>1); // gives initial guess y0
//	x = *(float*)&i;        // convert bits BACK to float
//	x = x*(1.5f-xhalf*x*x); // Newton step, repeating increases accuracy
//	return (float)1/x;
//}
//
//
///**
// * 使用面积法计算中线方向和曲率
// * 返回值： 曲率
// */
//int
//imgArea(int row1,int row2, int row3,int *area)
//{
//	int k;        //曲率
//	int32 AB=0, BC=0, AC=0;
//	*area = ((row2-row1)*(middle[row3]-middle[row1])-(row3-row1)*(middle[row2]-middle[row1]))/2;
//
//	AB = carmaSqrt((row1-row2)*(row1-row2)+(middle[row1]-middle[row2])*(middle[row1]-middle[row2]));
//	BC = carmaSqrt((row2-row3)*(row2-row3)+(middle[row2]-middle[row3])*(middle[row2]-middle[row3]));
//	AC = carmaSqrt((row1-row3)*(row1-row3)+(middle[row1]-middle[row3])*(middle[row1]-middle[row3]));
//	k = 4*(*area)/(AB*BC*AC);
//
//	return k;
//}

/*
 * 起跑线检测
 */
int
imgStartLine(void)
{

}