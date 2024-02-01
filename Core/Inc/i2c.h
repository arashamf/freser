/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.h
  * @brief   This file contains all the function prototypes for
  *          the i2c.c file
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
#ifndef __I2C_H__
#define __I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define FLAG_WRITE 	0x0 // операци¤ записи
#define FLAG_READ 	0x1 	// операци¤ чтени¤
/* USER CODE END Private defines */

void MX_I2C1_Init(void);

/* USER CODE BEGIN Prototypes */
void i2c_read_array (uint8_t , uint16_t , uint8_t * , uint8_t );
void i2c_write_byte (uint8_t , uint8_t * , uint8_t );
void i2c_write_buffer_16bit_registr (uint8_t , uint16_t , uint8_t *, uint8_t);
void i2c_write_buffer_8bit_registr (uint8_t , uint8_t , uint8_t *, uint8_t);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __I2C_H__ */

