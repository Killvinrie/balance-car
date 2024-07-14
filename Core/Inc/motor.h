#ifndef __MOTOR__H
#define __MOTOR__H

#include "stm32f1xx_hal.h"
#include "main.h"
#define DUTY_MAX  7200
#define DUTY_MIN  -7200

void Duty_motor(int Motor_L, int Motor_R);


#endif