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

float UpdataPID(PidObject *pid, const float measured)
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

void
steerUpdate(int8 error)
{
	int32 out;

	//	pidSetKp(&pidSteer,(error)*(error)*0.5 + 3);
	out = UpdataPID(&pidSteer, error);

	out += 500;

	steerSetDuty(out);
}

void steerSetDuty(uint32 duty)
{   
	if(duty > STEER_MAX){
		duty = STEER_MAX;
	}else if(duty < STEER_MIN){
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

	// TAGS: 编码器PTA10输入 下降沿中断 上拉 带滤波
	port_init(PTA10, IRQ_FALLING | PULLUP | PF);                 

	pit_init_ms(PIT0, 500);  //100ms 触发一次PIT中断 进行测速
}

extern bool getEncoder;
vuint32  speed_cnt=0;      // 编码器采集到的现在的速度值

void 
motorSetSpeed(uint32 speed)
{
	static float duty;
	int32 pwm;

#ifdef CLOSE_LOOP
	if(true == getEncoder)  {
		pidMotor.desired = speed;
		pwm = (int32)UpdataPID(&pidMotor, speed_cnt);

		duty = duty + pwm;
		//	duty = speed;
		if(duty>FTM_PRECISON) duty = FTM_PRECISON;
		if(duty<0) duty = 0;

		printf("duty is %ld\n", (uint32)duty);
		duty = FTM_PRECISON - duty;

		printf("speed_cnt is %ld\n", speed_cnt);

		FTM_PWM_Duty(MOTOR2_FTM, MOTOR2_CHN, (uint32)duty);
		getEncoder = false;
	}
#else
	pwm = FTM_PRECISON - speed;

	if(pwm > FTM_PRECISON) pwm = FTM_PRECISON;
	if(pwm<0) pwm = 0;

	FTM_PWM_Duty(MOTOR2_FTM, MOTOR2_CHN, (uint32) pwm);

#endif
}

void
stopcar(void)
{
	FTM_PWM_Duty(MOTOR1_FTM, MOTOR1_CHN,  1);
	FTM_PWM_Duty(MOTOR2_FTM, MOTOR2_CHN, 1000);
	motorSetSpeed(1);
	while(1)
	{
		GPIOD_PSOR |= (0x55 << 8);
		DELAY_MS(500);
		GPIOD_PSOR |= (0xaa << 8);
		DELAY_MS(500);
	}
}