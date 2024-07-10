#ifndef __SR04__H
#define __SR04__H

#include "stm32f1xx_hal.h"
#include "main.h"

typedef enum
{   
    SR04_READY,
    SR04_TRIGGED,
    SR04_ECHO
}SR04_statemachine;

extern float distance;
extern SR04_statemachine SR04_STATE;

void Distance_Trig(void);
void ECHO_Callback(void);
#endif