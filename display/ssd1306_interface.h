/**
  ******************************************************************************
  * @file           : ssd1306_interface.h
  * @brief          : SSD1306 driver interface header
  * @author         : MicroTechnics (microtechnics.ru)
  ******************************************************************************
  */

#ifndef SSD1306_INTERFACE_H
#define SSD1306_INTERFACE_H



/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private define ------------------------------------------------------------*/
#define SSD1306_I2C_ADDRESS                                	0x78
#define SSD1306_BYTE_COMMAND                                0x00
#define SSD1306_BYTE_DATA                                   0x40
/* Private variables ---------------------------------------------------------*/
typedef enum
{
  SSD1306_READY = 0x00,
  SSD1306_BUSY  = 0x01
} SSD1306_State;

/* Functions -----------------------------------------------------------------*/
void ssd1306_SendCommand(uint8_t );
void ssd1306_SendByteData(uint8_t );
void ssd1306_SendDataBuffer(uint8_t *, uint16_t );

#endif // #ifndef SSD1306_INTERFACE_H
