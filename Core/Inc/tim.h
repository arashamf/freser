/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.h
  * @brief   This file contains all the function prototypes for
  *          the tim.c file
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
#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define MAX_xTIMERS  6
/* USER CODE END Private defines */

void MX_TIM3_Init(void);

/* USER CODE BEGIN Prototypes */
void encoder_init(void);
void delay_us(uint16_t);
void repeat_time (uint16_t );
void timers_ini (void);
void TimerDisplayStart (void);
void TimerDisplayDelete (void);

//Variables -----------------------------------------------------------------------//
extern uint8_t end_bounce;

//Exported types -----------------------------------------------------------------//
typedef void * xTimerHandle;
typedef void (*tmrTIMER_CALLBACK)(xTimerHandle xTimer);	
typedef uint32_t (*portTickType)(void);

//-----------------------------------------------------
typedef enum 
{
	__IDLE = 0, 
	__ACTIVE, 
	__DONE
} tmrTIMER_STATE;

//-----------------------------------------------------
typedef struct
{
	uint32_t expiry;
  uint32_t periodic;
	tmrTIMER_STATE State;
	FunctionalState	AutoReload;
	tmrTIMER_CALLBACK	CallbackFunction;
} xTIMER;	
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */

