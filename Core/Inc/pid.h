#ifndef __PID__H
#define __PID__H

#include "stm32f1xx_hal.h"
#include "main.h"

typedef enum
{	
	Balance_idle,
	Balance_stop,
	Balance_running
}Balance_statemachine;


extern int Encoder_L, Encoder_R;
extern float pitch, roll, yaw;
extern short Gyro_X,Gyro_Y,Gyro_Z;
extern int duty;
extern Balance_statemachine Balance_state;
extern int Vertical_Out, Velocity_Out, Turn_Out, Target_Speed, Target_Turn, DUTY_L, DUTY_R;


int control();
#endif
