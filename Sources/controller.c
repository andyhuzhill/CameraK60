/**
 * =================================================================================
 * controller.c
 *
 *  Created on: Apr 2, 2013
 *      Author: scout
 *		E-mail: andyhuzhill@gmail.com
 *    Homepage: http://andyhuzhill.github.com
 *	   
 * Description: 
 *    Revision:
 */

#include "controller.h"

#define DEFAULT_PID_INTEGRATION_LIMIT  5000.0

PidObject pidSteer;
PidObject pidMotor;

void pidInit(PidObject *pid, const float desired, const float kp,
		const float ki, const float kd)
{
	pid->error = 0;
	pid->prevError = 0;
	pid->integ = 0;
	pid->deriv = 0;
	pid->desired = desired;
	pid->kp = kp;
	pid->ki = ki;
	pid->kd = kd;

	pid->iLimit = DEFAULT_PID_INTEGRATION_LIMIT;
}

void pidSetKp(PidObject *pid, const float kp)
{
	pid->kp = kp;
}

void pidSetKi(PidObject *pid, const float ki)
{
	pid->ki = ki;
}

void pidSetKd(PidObject *pid, const float kd)
{
	pid->kd = kd;
}

float UpdatePID(PidObject *pid, const float measured)
{
	float output;

	pid->error = pid->desired - measured;       //计算误差

	pid->integ += pid->error;                   //计算积分

	if(pid->integ > pid->iLimit) {               //避免积分和超过限制
		pid->integ = pid->iLimit;
	} else if (pid->integ < - pid->iLimit) {
		pid->integ = -pid->iLimit;
	}

	pid->deriv = (pid->error - pid->prevError); //计算微分

	//计算PID输出
	output = pid->kp * pid->error + pid->ki*pid->integ + pid->kd *pid->deriv; 

	pid->prevError = pid->error;

	return output;
}

void steerInit(void)
{
	FTM_PWM_init(STEER_FTM, STEER_CHN, STEER_FREQ, STEER_DEFAULT_DUTY);

	pidInit(&pidSteer, 0, PID_STEER_KP, PID_STEER_KI, PID_STEER_KD);
	pidSteer.iLimit = PID_STEER_INTEGRATION_LIMIT;
}

int32
steerUpdate(int8 error)
{
	int32 out;

	out = UpdatePID(&pidSteer, error);

	return out;
}

void steerSetDuty(int32 duty)
{   
	if((int32)duty > (int32)STEER_MAX){
		duty = STEER_MAX;
	}else if((int32)duty < (int32)STEER_MIN){
		duty = STEER_MIN;
	}
	FTM_PWM_Duty(STEER_FTM, STEER_CHN, duty);
}

void 
motorInit(void)
{
	FTM_PWM_init(MOTOR1_FTM, MOTOR1_CHN, MOTOR1_FREQ, MOTOR1_DEFAULT_DUTY);
	FTM_PWM_init(MOTOR2_FTM, MOTOR2_CHN, MOTOR2_FREQ, MOTOR2_DEFAULT_DUTY);

	pidInit(&pidMotor, 0, PID_MOTOR_KP, PID_MOTOR_KI, PID_MOTOR_KD);
	pidMotor.iLimit = PID_MOTOR_INTEGRATION_LIMIT;

	// TAGS: 编码器PTA8输入 下降沿中断 上拉 带滤波
	port_init(ENCODER_PIN, IRQ_FALLING | PULLUP | PF);                 

	pit_init_ms(PIT0, 1);  //1ms 触发一次PIT中断 进行测速
}

extern vint8 getEncoder;
vuint32  speed_cnt;      // 编码器采集到的现在的速度值

void 
motorSetSpeed(int32 speed)
{
	static float duty;
	int32 pwm;

#ifdef SPEED
	uint8 txbuff[4];
	uint8 status;
#endif

#ifdef CLOSE_LOOP
	if(getEncoder)  {
		pidMotor.desired = speed;
		pwm = (int32)UpdatePID(&pidMotor, speed_cnt);

		duty = duty + pwm;

#ifdef SPEED			//如果是调试速度控制参数 用AT2401发送编码器返回的值
		txbuff[0] = speed_cnt / (1 << 24);
		txbuff[1] = speed_cnt/(1 << 16) - (speed_cnt/(1<<24) << 8);
		txbuff[2] = speed_cnt/(1 << 8) - (speed_cnt/(1<<16) << 8);
		txbuff[3] = speed_cnt - (speed_cnt/(1<<8) << 8);

		NRF_ISR_Tx_Dat(txbuff, 4);

		do{
			status = NRF_ISR_Tx_State();
		}while ( status == TX_ISR_SEND);
#endif

		if(duty >= FTM_PRECISON*2/3) duty = FTM_PRECISON*2/3;
		if(duty <= 0) duty = 0;

		if(speed_cnt > speed){	//如果当前速度大于期望速度 则刹车
			FTM_PWM_Duty(MOTOR1_FTM, MOTOR1_CHN, 0);
			FTM_PWM_Duty(MOTOR2_FTM, MOTOR2_CHN, 0);
		}else{
			FTM_PWM_Duty(MOTOR1_FTM, MOTOR1_CHN, MOTOR1_DEFAULT_DUTY);
			FTM_PWM_Duty(MOTOR2_FTM, MOTOR2_CHN, (uint32)(FTM_PRECISON - duty));
		}
		getEncoder = 0;
	}
#else
	pwm = FTM_PRECISON - speed;

	if(pwm > FTM_PRECISON) pwm = FTM_PRECISON;
	if(pwm<0) pwm = 0;

	FTM_PWM_Duty(MOTOR2_FTM, MOTOR2_CHN, (uint32) pwm);

#endif
}

/*
 * 停车
 */
extern vint8 startLine;
void
stopcar(void)
{
	FTM_PWM_Duty(MOTOR1_FTM, MOTOR1_CHN, 0);
	FTM_PWM_Duty(MOTOR2_FTM, MOTOR2_CHN, 0);

	while(1)
	{
		startLine = 0;
		imgProcess();		//刹车时 舵机不能打死
		GPIOD_PDOR &= ~(0xff << 8);  // 刹车后 指示灯全亮
	}
}