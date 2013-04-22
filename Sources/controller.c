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

float UpdataPID(PidObject *pid, const float measured)
{
    float output;

    pid->error = pid->desired - measured;       //�������

    pid->integ += pid->error;                   //�������

    if(pid->integ > pid->iLimit)                //������ֺͳ�������
    {
        pid->integ = pid->iLimit;
    }
    else if (pid->integ < - pid->iLimit)
    {
        pid->integ = -pid->iLimit;
    }

    pid->deriv = (pid->error - pid->prevError); //����΢��

    //����PID���
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

void steerSetDuty(uint8 duty)
{
    FTM_PWM_Duty(STEER_FTM, STEER_CHN, duty);
}


void motorInit(void)
{
    FTM_PWM_init(MOTOR1_FTM, MOTOR1_CHN, MOTOR1_FREQ, MOTOR1_DEFAULT_DUTY);
    FTM_PWM_init(MOTOR2_FTM, MOTOR2_CHN, MOTOR2_FREQ, MOTOR2_DEFAULT_DUTY);
    gpio_init(MOTOR_EN_PORT, MOTOR_EN_PIN, Mode_OUT, High);          //�������оƬʹ��

    pidInit(&pidMotor, 0, PID_MOTOR_KP, PID_MOTOR_KI, PID_MOTOR_KD);
    pidMotor.iLimit = PID_MOTOR_INTEGRATION_LIMIT;

//    FTM_Input_init(ENCODER_FTM, ENCODER_CHN, Rising);   //���ñ������������
    pit_init_ms(PIT1, 500);  //100ms ����һ��PIT�ж� ���в���
}


volatile bool getEncoder= false;

void motorSetSpeed(uint32 realspeed, uint32 speed)
{
    static float duty;
    int32 pwm;

    printf("getEncoder is %d\n",(getEncoder == true));
    printf("duty is %ld\n", (uint32)duty);
    printf("speed_cnt is %ld\n", realspeed);

    if(true == getEncoder) 
    {
        pidMotor.desired = speed;
        pwm = (int32)UpdataPID(&pidMotor, realspeed);

        duty = duty + PWM2DUTY(pwm);
        if(duty>100) duty = 100;
        if(duty<0) duty = 0;

        printf("duty is %ld\n", (uint32)duty);
        printf("speed_cnt is %ld\n", realspeed);
        
        FTM_PWM_Duty(MOTOR2_FTM, MOTOR2_CHN, (uint32)duty);
        getEncoder = false;
    }
}