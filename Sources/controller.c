/*
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
#include "pid.h"

//Fancier version
#define TRUNCATE_SINT16(out, in) (out = (in<INT16_MIN)?INT16_MIN:((in>INT16_MAX)?INT16_MAX:in) )

PidObject pidSteer;
PidObject pidMotor;

int16 steerOutput;
int16 motorOutput;

void 
controllerInit(void)
{
    pidInit(&pidSteer, 0, PID_STEER_KP, PID_MOTOR_KI, PID_MOTOR_KD);
    pidInit(&pidMotor, 0, PID_MOTOR_KP, PID_MOTOR_KI, PID_MOTOR_KD);
    pidSetIntegralLimit(&pidSteer, PID_STEER_INTEGRATION_LIMIT);
    pidSetIntegralLimit(&pidMotor, PID_MOTOR_INTEGRATION_LIMIT);
}


void 
controllerUpdatePID(
        float steerActual, float motorActual,
        float steerDesired, float motorDesired)
{
    pidSetDesired(&pidSteer, steerDesired);
    TRUNCATE_SINT16(steerOutput,  pidUpdate(&pidSteer, steerActual, true));

    pidSetDesired(&pidMotor, motorDesired);
    TRUNCATE_SINT16(motorOutput, pidUpdate(&pidMotor, motorActual, true));
}

void 
controllerResetPID(void)
{
    pidReset(&pidMotor);
    pidReset(&pidSteer);
}

void
controllerGetOutput(int16* steer, int16* motor)
{
    *steer = steerOutput;
    *motor = motorOutput;
}


void steerInit(void)
{
    FTM_PWM_init(STEER_FTM, STEER_CHN, STEER_FREQ, STEER_DEFAULT_DUTY);
}

void steerSetDuty(uint8 duty)
{
    FTM_PWM_Duty(STEER_FTM, STEER_CHN, duty);
}

void motorInit(void)
{
    FTM_PWM_init(MOTOR1_FTM, MOTOR1_CHN, MOTOR1_FREQ, MOTOR1_DEFAULT_DUTY);
    FTM_PWM_init(MOTOR2_FTM, MOTOR2_CHN, MOTOR2_FREQ, MOTOR2_DEFAULT_DUTY);
    gpio_init(MOTOR_EN_PORT, MOTOR_EN_PIN, Mode_OUT, High);          //电机驱动芯片使能

    FTM_Input_init(ENCODER_FTM, ENCODER_CHN, Rising);   //配置编码器输入测速
    pit_init_ms(PIT0, 500);                             //500ms 触发一次PIT中断 进行测速
}


void motorSetSpeed(uint32 speed, uint32 realspeed)
{
    controllerUpdatePID(0,(float)realspeed, 0, (float)speed);
    FTM_PWM_Duty(MOTOR1_FTM, MOTOR1_CHN,(uint32)motorOutput);
}