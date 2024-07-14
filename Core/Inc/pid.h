#ifndef __PID__H
#define __PID__H

#include "stm32f1xx_hal.h"
#include "main.h"


extern int Encoder_L, Encoder_R;
extern float pitch, roll, yaw;
extern int duty;


int control();
#endif
