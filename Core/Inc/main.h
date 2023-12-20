/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

#include "stm32f0xx_ll_tim.h"
#include "stm32f0xx_ll_system.h"
#include "stm32f0xx_ll_gpio.h"
#include "stm32f0xx_ll_exti.h"
#include "stm32f0xx_ll_bus.h"
#include "stm32f0xx_ll_cortex.h"
#include "stm32f0xx_ll_rcc.h"
#include "stm32f0xx_ll_utils.h"
#include "stm32f0xx_ll_pwr.h"
#include "stm32f0xx_ll_dma.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include "typedef.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define IN1_Pin LL_GPIO_PIN_0
#define IN1_GPIO_Port GPIOF
#define IN2_Pin LL_GPIO_PIN_1
#define IN2_GPIO_Port GPIOF
#define LEFT_BUTTON_Pin LL_GPIO_PIN_0
#define LEFT_BUTTON_GPIO_Port GPIOA
#define CENTER_BUTTON_Pin LL_GPIO_PIN_1
#define CENTER_BUTTON_GPIO_Port GPIOA
#define RIGHT_BUTTON_Pin LL_GPIO_PIN_2
#define RIGHT_BUTTON_GPIO_Port GPIOA
#define ENCODER_BUTTON_Pin LL_GPIO_PIN_3
#define ENCODER_BUTTON_GPIO_Port GPIOA
#define PUL_Pin LL_GPIO_PIN_4
#define PUL_GPIO_Port GPIOA
#define DIR_Pin LL_GPIO_PIN_5
#define DIR_GPIO_Port GPIOA
#define EN_DRIVE_Pin LL_GPIO_PIN_1
#define EN_DRIVE_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

#define ON 									1
#define OFF 								0
#define FORWARD 						1
#define BACKWARD 						0
#define DISP_CLEAR 					1
#define DISP_NOT_CLEAR 			0

#define CPU_CLOCK_VALUE			(48000000UL)	// Частота контроллера //
#define TICKS_PER_SECOND		1000 

//extern angular_data_t curr_rotation; //структура с угловыми данными привода
//extern encoder_data_t encoder_data; //структура с данными энкодера
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
