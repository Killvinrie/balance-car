#include "sr04.h"

//external variable
extern TIM_HandleTypeDef htim3;

//global variable
float distance;
uint16_t count;
SR04_statemachine SR04_STATE;


void DELAY_us(uint8_t us)
{
    __IO uint32_t Delay = us * 72 / 8;
    do
    {
        __NOP();
    } while (Delay--);
    
}

void Distance_Trig(void)
{
    HAL_GPIO_WritePin(TRIG_GPIO_Port,TRIG_Pin,SET);
    DELAY_us(12);
    HAL_GPIO_WritePin(TRIG_GPIO_Port,TRIG_Pin,RESET);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{   
    if(GPIO_Pin == ECHO_Pin)
    {
        if(HAL_GPIO_ReadPin(ECHO_GPIO_Port,ECHO_Pin) == SET)
        {
            SR04_STATE = SR04_TRIGGED;
            __HAL_TIM_SetCounter(&htim3,0);
            HAL_TIM_Base_Start(&htim3);
        }
        else
        {
            SR04_STATE = SR04_ECHO;
            HAL_TIM_Base_Stop(&htim3);
            count = __HAL_TIM_GET_COUNTER(&htim3);
            distance = count *0.017;      //        (count/ 1000000 * 340 / 2)
        }
    }
}