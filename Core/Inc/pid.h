#ifndef __PID__H
#define __PID__H

#include "stm32f1xx_hal.h"
#include "main.h"


typedef enum
{
	Direction_GO,
	Direction_BACK,
}Remote_Direction_GB;

typedef enum
{
	Direction_LEFT,
	Direction_RIGHT
}Remote_Direction_LR;


typedef enum
{	
	Balance_idle,
	Balance_stop,
	Balance_running,
	Balance_remote
}Balance_statemachine;


extern int Encoder_L, Encoder_R;
extern float pitch, roll, yaw;
extern short Gyro_X,Gyro_Y,Gyro_Z;
extern int duty;
extern Balance_statemachine Balance_state;
extern int Vertical_Out, Velocity_Out, Turn_Out, Target_Speed, Target_Turn, DUTY_L, DUTY_R, Err_S;
extern float Vertical_KP ,Vertical_KD ,Velocity_KP ,Velocity_KI, Turn_KP, Turn_KD;
extern Remote_Direction_GB Direction_G_B;
extern Remote_Direction_LR Direction_L_R;


int control();
#endif
