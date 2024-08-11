#ifndef __PID__H
#define __PID__H

#include "stm32f1xx_hal.h"
#include "main.h"
#include "oled.h"

typedef enum
{
	Direction_G_B_DEFAULT,
	Direction_GO,
	Direction_BACK,
} Remote_Direction_GB;

typedef enum
{
	Direction_L_R_DEFAULT,
	Direction_LEFT,
	Direction_RIGHT
} Remote_Direction_LR;

typedef enum
{
	Balance_idle,
	Balance_stop,
	Balance_running,
	Balance_remote
} Balance_statemachine;

typedef enum
{
	Parameter_Med_Angle,
	Parameter_VERTI_P,
	Parameter_VERTI_D,
	Parameter_VELOC_P,
	Parameter_VELOC_I,
	Parameter_TURN_P,
	Parameter_TURN_D,
	Parameter_Med_Yaw,
	Parameter_NUM
} Parameter_CONFIG_SM;

typedef enum
{
	Parameter_Free,
	Parameter_Selected
} Parameter_state;

typedef struct
{
	float *parameter_ptr;
	Parameter_state state;
} Parameter_CONFIG_TYPE;

#define IS_Parameter_Highlight(__HANDLE__)  (__HANDLE__ == Parameter_IDX)



extern Parameter_CONFIG_TYPE PID_Parameter[Parameter_NUM];

extern int Encoder_L, Encoder_R;
extern float pitch, roll, yaw;
extern short Gyro_X, Gyro_Y, Gyro_Z;
extern int duty;
extern Balance_statemachine Balance_state;
extern int Vertical_Out, Velocity_Out, Turn_Out, Target_Speed, Target_Turn, DUTY_L, DUTY_R, Err_S;
extern float Vertical_KP, Vertical_KD, Velocity_KP, Velocity_KI, Turn_KP, Turn_KD;
extern float Med_Angle;
extern float Med_Yaw;
extern Remote_Direction_GB Direction_G_B;
extern Remote_Direction_LR Direction_L_R;
extern uint8_t Parameter_IDX;

int control();

Parameter_state IS_PARAMETER_SELECTED(Parameter_CONFIG_TYPE *parameter);
FlagStatus Parameter_IDX_LOCKED(Parameter_CONFIG_TYPE *parameter);


#endif
