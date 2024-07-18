#ifndef __OLED_H__
#define __OLED_H__

#include "stm32f1xx_hal.h"
// π”√”≤º˛I2C1 
///
#define   OLED_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOA_CLK_ENABLE()

#define   GPIOx_OLED_PORT               GPIOB
#define   OLED_SCK_PIN                  GPIO_PIN_8
#define   OLED_SCK_ON()                 HAL_GPIO_WritePin(GPIOx_OLED_PORT, OLED_SCK_PIN, GPIO_PIN_SET)
#define   OLED_SCK_OFF()                HAL_GPIO_WritePin(GPIOx_OLED_PORT, OLED_SCK_PIN, GPIO_PIN_RESET)
#define   OLED_SCK_TOGGLE()             HAL_GPIO_TogglePin(GPIOx_OLED_PORT, OLED_SCK_PIN)
#define   OLED_SDA_PIN                  GPIO_PIN_9
#define   OLED_SDA_ON()                 HAL_GPIO_WritePin(GPIOx_OLED_PORT, OLED_SDA_PIN, GPIO_PIN_SET)
#define   OLED_SDA_OFF()                HAL_GPIO_WritePin(GPIOx_OLED_PORT, OLED_SDA_PIN, GPIO_PIN_RESET)
#define   OLED_SDA_TOGGLE()             HAL_GPIO_TogglePin(GPIOx_OLED_PORT, OLED_SDA_PIN)
///

typedef enum 
{
    OLED_PAGE_Sensor,
    OLED_PAGE_Parameter,
    OLED_PAGE_State,
    OLED_PAGE_NUM
}OLED_PAGE_SM;

typedef enum 
{
    Highlight_No,
    Highlight_Yes
}OLED_Highlight;

typedef enum
{
    Refresh_IDLE,
    Refresh_REQUEST
}PAGE_REFRESH;

extern OLED_PAGE_SM OLED_PAGE_IDX;
extern PAGE_REFRESH PAGE_REFRESH_STATE;

void WriteCmd(void);
void OLED_WR_CMD(uint8_t cmd);
void OLED_WR_DATA(uint8_t data);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Set_Pos(uint8_t x, uint8_t y);
void OLED_On(void);
void OLED_ShowNum(uint8_t x,uint8_t y,unsigned int num,uint8_t len,uint8_t size2,OLED_Highlight highlight);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size,OLED_Highlight highlight);
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t Char_Size,OLED_Highlight Highlight);
void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no);


#endif
