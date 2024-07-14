#include "motor.h"

extern TIM_HandleTypeDef htim1;

int abs(int num)
{
    if (num < 0)
    {
        return -num;
    }
    else
        return num;
}

void Duty_motor(int Motor_L, int Motor_R)
{

    // LEFT MOTOR
    if (Motor_L > 0)
    {
        HAL_GPIO_WritePin(L1_GPIO_Port, L1_Pin, SET);
        HAL_GPIO_WritePin(L2_GPIO_Port, L2_Pin, RESET);
        if (Motor_L > DUTY_MAX)
        {
            Motor_L = DUTY_MAX;
        }
    }
    else
    {
        HAL_GPIO_WritePin(L1_GPIO_Port, L1_Pin, RESET);
        HAL_GPIO_WritePin(L2_GPIO_Port, L2_Pin, SET);
        if (Motor_L < DUTY_MIN)
        {
            Motor_L = DUTY_MIN;
        }
    }
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_4, abs(Motor_L));

    // RIGHT MOTOR
    if (Motor_R > 0)
    {
        HAL_GPIO_WritePin(R1_GPIO_Port, R1_Pin, SET);
        HAL_GPIO_WritePin(R2_GPIO_Port, R2_Pin, RESET);
        if (Motor_L > DUTY_MAX)
        {
            Motor_L = DUTY_MAX;
        }
    }
    else
    {
        HAL_GPIO_WritePin(R1_GPIO_Port, R1_Pin, RESET);
        HAL_GPIO_WritePin(R2_GPIO_Port, R2_Pin, SET);
        if (Motor_L < DUTY_MIN)
        {
            Motor_L = DUTY_MIN;
        }
    }
    __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1, abs(Motor_R));
}
