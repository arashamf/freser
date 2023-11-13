/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
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
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define STEP(x) ((x)? (LL_GPIO_ResetOutputPin (PUL_GPIO_Port, PUL_Pin)) : (LL_GPIO_SetOutputPin(PUL_GPIO_Port, PUL_Pin)))
#define DIR_DRIVE(x) ((x)? (LL_GPIO_SetOutputPin (DIR_GPIO_Port, DIR_Pin)) : (LL_GPIO_ResetOutputPin(DIR_GPIO_Port, DIR_Pin)))
#define DRIVE_ENABLE(x) ((x)? (LL_GPIO_SetOutputPin (EN_DRIVE_GPIO_Port, EN_DRIVE_Pin)) : (LL_GPIO_ResetOutputPin(EN_DRIVE_GPIO_Port, EN_DRIVE_Pin)))
/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

