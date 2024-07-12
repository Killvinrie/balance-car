#include "encoder.h"

int Get_Speed(TIM_HandleTypeDef *htim)
{
    int cnt;
    cnt = (short)__HAL_TIM_GetCounter(htim);
    __HAL_TIM_SetCounter(htim,0);
    return cnt;
}