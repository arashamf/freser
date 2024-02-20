/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i2c.c
  * @brief   This file provides code for the configuration
  *          of the I2C instances.
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
/* Includes ------------------------------------------------------------------*/
#include "i2c.h"

/* USER CODE BEGIN 0 */
#include "tim.h"
/* USER CODE END 0 */

/* I2C1 init function */
void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  LL_I2C_InitTypeDef I2C_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  /**I2C1 GPIO Configuration
  PA9   ------> I2C1_SCL
  PA10   ------> I2C1_SDA
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_4;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */

  /** I2C Initialization
  */
  LL_I2C_DisableOwnAddress2(I2C1);
  LL_I2C_DisableGeneralCall(I2C1);
  LL_I2C_EnableClockStretching(I2C1);
  I2C_InitStruct.PeripheralMode = LL_I2C_MODE_I2C;
  I2C_InitStruct.Timing = 0x0000020B;
  I2C_InitStruct.AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE;
  I2C_InitStruct.DigitalFilter = 0;
  I2C_InitStruct.OwnAddress1 = 0;
  I2C_InitStruct.TypeAcknowledge = LL_I2C_ACK;
  I2C_InitStruct.OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT;
  LL_I2C_Init(I2C1, &I2C_InitStruct);
  LL_I2C_EnableAutoEndMode(I2C1);
  LL_I2C_SetOwnAddress2(I2C1, 0, LL_I2C_OWNADDRESS2_NOMASK);
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/* USER CODE BEGIN 1 */
//-------------------------------------------------------------------------------------------//
void i2c_read_array (uint8_t slave_address, uint16_t registr_adr, uint8_t * data, uint8_t numb_byte)
{
	uint8_t count = 0;
	uint8_t tmp_buf[2] ;
	tmp_buf[0] = (uint8_t)registr_adr>>8;
	tmp_buf[1] = (uint8_t)(0x00FF & registr_adr);
	
	LL_I2C_HandleTransfer(I2C1, slave_address, LL_I2C_ADDRSLAVE_7BIT, 2, LL_I2C_MODE_RELOAD, LL_I2C_GENERATE_START_WRITE);	
	
	while((LL_I2C_IsActiveFlag_TXE(I2C1)) == RESET) {}; //Ожидание отправки из регистра I2C_TXDR предыдущих данных
	LL_I2C_TransmitData8(I2C1, tmp_buf[0]);
	while((LL_I2C_IsActiveFlag_TXE(I2C1)) == RESET) {}; //Ожидание отправки из регистра I2C_TXDR предыдущих данных
	LL_I2C_TransmitData8(I2C1, tmp_buf[1]);	

//	while((LL_I2C_IsActiveFlag_TCR(I2C1)) == SET) {}; //When RELOAD=1 and NBYTES date have been transferred
		
	LL_I2C_HandleTransfer(I2C1, slave_address, LL_I2C_ADDRSLAVE_7BIT, numb_byte, LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_RESTART_7BIT_READ);
	
	for (count = 0; count < numb_byte; count++) 
	{
		while(LL_I2C_IsActiveFlag_RXNE(I2C1) == RESET) {}; //Ожидание получение данных
		*(data+count) = LL_I2C_ReceiveData8(I2C1);
	}
	
	LL_I2C_GenerateStopCondition(I2C1); //условие STOP
	LL_I2C_ClearFlag_STOP(I2C1);
		
}

//-------------------------------------------------------------------------------------//
void i2c_write_byte (uint8_t slave_address, uint8_t * data_buffer, uint8_t numb_byte)
{	
	LL_I2C_HandleTransfer(I2C1, slave_address, LL_I2C_ADDRSLAVE_7BIT, numb_byte+1, LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_RESTART_7BIT_WRITE);

	while((LL_I2C_IsActiveFlag_TXE(I2C1)) == RESET) {};
		
	for (uint8_t count = 0; count < numb_byte; count++)
  {			
		LL_I2C_TransmitData8(I2C1, *(data_buffer+count));
		while((LL_I2C_IsActiveFlag_TXE(I2C1)) == RESET) {}; //ожидание окончания передачи
	}	
	
	LL_I2C_GenerateStopCondition(I2C1); //условие STOP
	LL_I2C_ClearFlag_STOP(I2C1);
}

//--------------------------------------------------------------------------------------//
void i2c_write_buffer_16bit_registr (uint8_t slave_address, uint16_t registr_adr, uint8_t * data_buffer, uint8_t numb_byte)
{
	uint8_t tmp_buf[2] ;
	tmp_buf[0] = (uint8_t)(registr_adr>>8);
	tmp_buf[1] = (uint8_t)(0x00FF & registr_adr);
	uint8_t count = 0;
	
	LL_I2C_HandleTransfer(I2C1, slave_address, LL_I2C_ADDRSLAVE_7BIT, numb_byte+2, LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_START_WRITE);
	while(LL_I2C_IsActiveFlag_TXIS(I2C1) == RESET) {};
		
	for (count = 0; count < 2; count++) //передача адреса регистра (2 байта)
	{
		LL_I2C_TransmitData8(I2C1, tmp_buf[count]);
		while((LL_I2C_IsActiveFlag_TXE(I2C1)) == RESET) {}; //ожидание окончания передачи
	}
	
	for (count = 0; count < numb_byte; count++)
  {			
		LL_I2C_TransmitData8(I2C1, *(data_buffer+count));
		while((LL_I2C_IsActiveFlag_TXE(I2C1)) == RESET) {}; //ожидание окончания передачи
	}	
	
	LL_I2C_GenerateStopCondition(I2C1); //условие STOP
	LL_I2C_ClearFlag_STOP(I2C1);
}

//--------------------------------------------------------------------------------------//


//-------------------------------------------------------------------------------------//
void i2c_write_buffer_8bit_registr (uint8_t slave_address, uint8_t registr_adr, uint8_t * data_buffer, uint8_t numb_byte)
{
	uint8_t count = 0;
	
	LL_I2C_HandleTransfer(I2C1, slave_address, LL_I2C_ADDRSLAVE_7BIT, numb_byte+1, 
	LL_I2C_MODE_SOFTEND, LL_I2C_GENERATE_START_WRITE);
	while(LL_I2C_IsActiveFlag_TXIS(I2C1) == RESET) {};
			
	LL_I2C_TransmitData8(I2C1, registr_adr);
	while((LL_I2C_IsActiveFlag_TXE(I2C1)) == RESET) {}; //ожидание окончания передачи
	
	for (count = 0; count < numb_byte; count++)
  {	
		LL_I2C_TransmitData8(I2C1, *(data_buffer+count));
		while((LL_I2C_IsActiveFlag_TXE(I2C1)) == RESET) {}; //ожидание окончания передачи
	}	
	
	LL_I2C_GenerateStopCondition(I2C1); //условие STOP
	LL_I2C_ClearFlag_STOP(I2C1);
}
/* USER CODE END 1 */
