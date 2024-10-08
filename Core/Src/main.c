/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32f1xx_it.h"
#include "oled.h"
#include "IIC.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "stdio.h"
#include "sr04.h"
#include "motor.h"
#include "encoder.h"
#include "pid.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
DMA_HandleTypeDef hdma_i2c1_tx;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
uint8_t display_buf[20];
uint8_t Frame, FPS;
uint8_t Rx_buffer[2];

OLED_Highlight Parameter_bling_state = Highlight_No;

// uint8_t return_NUM;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_TIM4_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == ECHO_Pin)
  {
    ECHO_Callback();
  }
  else if (GPIO_Pin == KEY1_Pin)
  {
    switch (Balance_state)
    {
    case Balance_idle:
      PAGE_REFRESH_STATE = Refresh_REQUEST;
      Balance_state = Balance_running;
      break;
    case Balance_running:
      PAGE_REFRESH_STATE = Refresh_REQUEST;
      Balance_state = Balance_stop;
      break;
    case Balance_stop:
      PAGE_REFRESH_STATE = Refresh_REQUEST;
      Balance_state = Balance_running;
      break;
    default:
      PAGE_REFRESH_STATE = Refresh_REQUEST;
      Balance_state = Balance_idle;
    }
  }
  else if (GPIO_Pin == KEY2_Pin) // KEY2   motor test or display page switch
  {
    // duty_test();
    switch (OLED_PAGE_IDX)
    {
    case OLED_PAGE_Sensor:
      PAGE_REFRESH_STATE = Refresh_REQUEST;
      OLED_PAGE_IDX = OLED_PAGE_Parameter;
      break;
    case OLED_PAGE_Parameter:
      PAGE_REFRESH_STATE = Refresh_REQUEST;
      OLED_PAGE_IDX = OLED_PAGE_State;
      break;
    case OLED_PAGE_State:
      PAGE_REFRESH_STATE = Refresh_REQUEST;
      OLED_PAGE_IDX = OLED_PAGE_Sensor;
      break;
    }
  }
  else // GPIO_Pin == MPU_INT_Pin)
  {
    // Encoder_L = Get_Speed(&htim2);
    // Encoder_R = Get_Speed(&htim4);
    // do nothing.
    control();
  }
}
void HAL_IncTick(void)
{
  uwTick += uwTickFreq;
  if (uwTick % 1000 == 0)
  {
    if (Parameter_Bling_Enable == 1)
    {
      Parameter_bling_state = ~Parameter_bling_state;
    }
    FPS = Frame;
    Frame = 0;
  }
}

void Sensor_Display(void)
{
  sprintf((char *)display_buf, "FPS:%d ", FPS);
  OLED_ShowString(80, 0, display_buf, 8, Highlight_No);

  sprintf((char *)display_buf, "ENC_L:%d   ", Encoder_L);
  OLED_ShowString(0, 0, display_buf, 8, Highlight_No);
  sprintf((char *)display_buf, "ENC_R:%d   ", Encoder_R);
  OLED_ShowString(0, 1, display_buf, 8, Highlight_No);

  sprintf((char *)display_buf, "pitch:%.2f   ", pitch);
  OLED_ShowString(0, 2, display_buf, 8, Highlight_No);
  sprintf((char *)display_buf, "yaw:%.2f   ", yaw);
  OLED_ShowString(0, 3, display_buf, 8, Highlight_No);
  sprintf((char *)display_buf, "roll:%.2f   ", roll);
  OLED_ShowString(0, 4, display_buf, 8, Highlight_No);
  sprintf((char *)display_buf, "Distance:%.2f  ", distance);
  OLED_ShowString(0, 5, display_buf, 8, Highlight_No);

  sprintf((char *)display_buf, "GYRO_X:%.2f   ", (float)Gyro_X);
  OLED_ShowString(0, 6, display_buf, 8, Highlight_No);

  sprintf((char *)display_buf, "MODE:%d", Control_Mode);
  OLED_ShowString(80, 6, display_buf, 8, Highlight_No);
  // Balance state display HIGHLIGHT
  if (Balance_state == Balance_idle)
    OLED_ShowString(90, 7, "IDLE", 8, Highlight_No);
  else
    OLED_ShowString(80, 7, "RUNNING", 8, Highlight_No);
}

OLED_Highlight Highlight_OR_NOT(Parameter_CONFIG_SM Parameter_idx)
{
  if (Parameter_IDX == Parameter_idx)
  {
    if (Parameter_Bling_Enable)
    {
      return Parameter_bling_state;
    }
    else
      return Highlight_Yes;
  }
  else
    return Highlight_No;
}

void Parameter_Display(void)
{
  sprintf((char *)display_buf, "FPS:%d ", FPS);
  OLED_ShowString(80, 0, display_buf, 8, Highlight_No);

  sprintf((char *)display_buf, "Med:%.2f  ", Med_Angle); // show Med_Angle
  OLED_ShowString(0, 0, display_buf, 8, Highlight_OR_NOT(Parameter_Med_Angle));
  sprintf((char *)display_buf, "Verti_P:%.2f  ", Vertical_KP);
  OLED_ShowString(0, 1, display_buf, 8, Highlight_OR_NOT(Parameter_VERTI_P));
  sprintf((char *)display_buf, "Verti_D:%.2f  ", Vertical_KD);
  OLED_ShowString(0, 2, display_buf, 8, Highlight_OR_NOT(Parameter_VERTI_D));
  sprintf((char *)display_buf, "Veloc_P:%.2f  ", Velocity_KP);
  OLED_ShowString(0, 3, display_buf, 8, Highlight_OR_NOT(Parameter_VELOC_P));
  sprintf((char *)display_buf, "Veloc_I:%.2f  ", Velocity_KI);
  OLED_ShowString(0, 4, display_buf, 8, Highlight_OR_NOT(Parameter_VELOC_I));
  sprintf((char *)display_buf, "Turn_KP:%.2f  ", Turn_KP);
  OLED_ShowString(0, 5, display_buf, 8, Highlight_OR_NOT(Parameter_TURN_P));
  sprintf((char *)display_buf, "Turn_KD:%.2f  ", Turn_KD);
  OLED_ShowString(0, 6, display_buf, 8, Highlight_OR_NOT(Parameter_TURN_D));
  sprintf((char *)display_buf, "Med_yaw:%.2f  ", Med_Yaw);
  OLED_ShowString(0, 7, display_buf, 8, Highlight_OR_NOT(Parameter_Med_Yaw));

  sprintf((char *)display_buf, "MODE:%d", Control_Mode);
  OLED_ShowString(80, 6, display_buf, 8, Highlight_No);
  // Balance state display HIGHLIGHT
  if (Balance_state == Balance_idle)
    OLED_ShowString(90, 7, "IDLE", 8, Highlight_No);
  else
    OLED_ShowString(80, 7, "RUNNING", 8, Highlight_No);
}

void State_Display(void)
{
  sprintf((char *)display_buf, "FPS:%d ", FPS);
  OLED_ShowString(80, 0, display_buf, 8, Highlight_No);

  sprintf((char *)display_buf, "Verti:%.d   ", Vertical_Out);
  OLED_ShowString(0, 0, display_buf, 8, Highlight_No);
  sprintf((char *)display_buf, "Veloc:%d   ", Velocity_Out);
  OLED_ShowString(0, 1, display_buf, 8, Highlight_No);

  OLED_ShowString(80, 2, Rx_buffer, 16, Highlight_No); // show remote instruction
  sprintf((char *)display_buf, "Target_S:%d   ", Target_Speed);
  OLED_ShowString(0, 5, display_buf, 8, Highlight_No);

  sprintf((char *)display_buf, "MODE:%d", Control_Mode);
  OLED_ShowString(80, 6, display_buf, 8, Highlight_No);
  // if (Control_Mode == Mode_Move)//Remote control Mode display
  //   OLED_ShowString(90, 6, "Move", 8, Highlight_Yes);
  // else
  //   OLED_ShowString(80, 6, "Config", 8, Highlight_Yes);
  // Balance state display HIGHLIGHT
  if (Balance_state == Balance_idle)
    OLED_ShowString(90, 7, "IDLE", 8, Highlight_Yes);
  else
    OLED_ShowString(80, 7, "RUNNING", 8, Highlight_Yes);
}


/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_TIM4_Init();
  MX_I2C1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  OLED_Init();
  OLED_Clear();

  MPU_Init();
  mpu_dmp_init();
  // OLED_ShowString(0, 0, "Init success!", 16);
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0); // Enable PB5 interrupt after the mpu was initialized
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_4);
  //  Duty_motor(-5000, 6000); //test motor
  HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);

  HAL_UART_Receive_IT(&huart2, Rx_buffer, 1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    Distance_Trig();

    switch (OLED_PAGE_IDX)
    {
    case OLED_PAGE_Sensor:
      if (PAGE_REFRESH_STATE == Refresh_REQUEST) //  WHEN PAGE SWITCH OR KEY Variable change was detected ,  OLED need to be refresh
      {
        OLED_Clear();
        PAGE_REFRESH_STATE = Refresh_IDLE;
      }
      Sensor_Display();
      Frame++;
      break;

    case OLED_PAGE_Parameter:
      if (PAGE_REFRESH_STATE == Refresh_REQUEST) //  WHEN PAGE SWITCH OR KEY Variable change was detected ,  OLED need to be refresh
      {
        OLED_Clear();
        PAGE_REFRESH_STATE = Refresh_IDLE;
      }
      Parameter_Display();
      Frame++;
      break;

    case OLED_PAGE_State:
      if (PAGE_REFRESH_STATE == Refresh_REQUEST) //  WHEN PAGE SWITCH OR KEY Variable change was detected ,  OLED need to be refresh
      {
        OLED_Clear();
        PAGE_REFRESH_STATE = Refresh_IDLE;
      }
      State_Display();
      Frame++;
      break;
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */
}

/**
 * @brief TIM1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 7199;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 7200;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);
}

/**
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
}

/**
 * @brief TIM3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 71;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
}

/**
 * @brief TIM4 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim4, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */
}

/**
 * @brief USART3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */
}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, L1_Pin | L2_Pin | R1_Pin | R2_Pin | MPU_SCL_Pin | MPU_SDA_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(TRIG_GPIO_Port, TRIG_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : L1_Pin L2_Pin R1_Pin R2_Pin */
  GPIO_InitStruct.Pin = L1_Pin | L2_Pin | R1_Pin | R2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : TRIG_Pin */
  GPIO_InitStruct.Pin = TRIG_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(TRIG_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : ECHO_Pin */
  GPIO_InitStruct.Pin = ECHO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ECHO_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : KEY1_Pin KEY2_Pin */
  GPIO_InitStruct.Pin = KEY1_Pin | KEY2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : MPU_SCL_Pin MPU_SDA_Pin */
  GPIO_InitStruct.Pin = MPU_SCL_Pin | MPU_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : MPU_INT_Pin */
  GPIO_InitStruct.Pin = MPU_INT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(MPU_INT_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  //  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 1, 0);
  //  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 1, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void duty_test()
{
  if (duty > 0)
  {
    duty += 1000;
    if (duty > 7200)
    {
      duty = 0;
    }
  }
  else
  {
    duty -= 1000;
    if (duty < -7200)
    {
      duty = 0;
    }
  }
  Duty_motor(duty, duty);
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
