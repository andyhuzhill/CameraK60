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
float motorOutput;

void 
steerUpdatePID(float steerActual, float steerDesired)
{
    pidSetDesired(&pidSteer, steerDesired);
    TRUNCATE_SINT16(steerOutput,  pidUpdate(&pidSteer, steerActual, true));
}

void
motorUpdatePID(float motorActual, float motorDesired)
{
    pidSetDesired(&pidMotor, motorDesired);
    motorOutput = pidUpdate(&pidMotor, motorActual, true);
}

void 
controllerResetPID(void)
{
    pidReset(&pidMotor);
    pidReset(&pidSteer);
}

void steerInit(void)
{
    FTM_PWM_init(STEER_FTM, STEER_CHN, STEER_FREQ, STEER_DEFAULT_DUTY);
    
    pidInit(&pidSteer, 0, PID_STEER_KP, PID_MOTOR_KI, PID_MOTOR_KD);        //PID 控制器初始化
    pidSetIntegralLimit(&pidSteer, PID_STEER_INTEGRATION_LIMIT);
}

void steerSetDuty(uint8 duty)
{
//    steerUpdatePID()
    FTM_PWM_Duty(STEER_FTM, STEER_CHN, duty);
}

void motorInit(void)
{
    FTM_PWM_init(MOTOR1_FTM, MOTOR1_CHN, MOTOR1_FREQ, MOTOR1_DEFAULT_DUTY);
    FTM_PWM_init(MOTOR2_FTM, MOTOR2_CHN, MOTOR2_FREQ, MOTOR2_DEFAULT_DUTY);
    gpio_init(MOTOR_EN_PORT, MOTOR_EN_PIN, Mode_OUT, High);          //电机驱动芯片使能

    pidInit(&pidMotor, 10, PID_MOTOR_KP, PID_MOTOR_KI, PID_MOTOR_KD);      //PID 控制器初始化
    pidSetIntegralLimit(&pidMotor, PID_MOTOR_INTEGRATION_LIMIT);

    FTM_Input_init(ENCODER_FTM, ENCODER_CHN, Rising);   //配置编码器输入测速
    pit_init_ms(PIT0, 1000);                             //1s 触发一次PIT中断 进行测速
}

void motorSetSpeed(uint32 realspeed, uint32 speed)
{
    float duty;
    float desired =0;
    uint8 status;

    if (speed >= 5) desired= speed*738-3535;
    motorUpdatePID((float)realspeed, desired);
    
    duty = (motorOutput+3535) / 738 ;
    
    uint8 *p = (unsigned char*)&duty;
    
    NRF_ISR_Tx_Dat(p, 4);
    
    do{
        status = NRF_ISR_Tx_State();
    }while(status == TX_ISR_SEND);
   
    FTM_PWM_Duty(MOTOR1_FTM, MOTOR1_CHN, (uint32)duty);
}