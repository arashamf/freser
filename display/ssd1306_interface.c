
/* Includes ------------------------------------------------------------------*/
#include "ssd1306_interface.h"

/* Private define ------------------------------------------------------------*/
#define SSD1306_I2C_TIMEOUT                                	0xFF

/* Private typedef -----------------------------------------------------------*/
extern I2C_HandleTypeDef hi2c1;
extern SSD1306_State SSD1306_state;

/* Private variables ---------------------------------------------------------*/
static uint8_t temp[2];
static uint8_t msg_size = 0;

/* Functions -----------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
void ssd1306_SendCommand(uint8_t type_command)
{
	temp[0] = SSD1306_BYTE_COMMAND ;
	temp[1] = type_command;
	msg_size = 2;
	HAL_I2C_Master_Transmit(&hi2c1, SSD1306_I2C_ADDRESS, temp, msg_size, SSD1306_I2C_TIMEOUT);
}

/*----------------------------------------------------------------------------*/
void ssd1306_SendByteData(uint8_t data)
{
	temp[0] = SSD1306_BYTE_DATA ;
	temp[1] = data;
	msg_size = 2;
	HAL_I2C_Master_Transmit (&hi2c1, SSD1306_I2C_ADDRESS, temp, msg_size, SSD1306_I2C_TIMEOUT);
}

/*----------------------------------------------------------------------------*/
void ssd1306_SendDataBuffer(uint8_t *data, uint16_t data_size)
{

	//HAL_I2C_Mem_Write_IT(&hi2c1, SSD1306_I2C_ADDRESS, SSD1306_BYTE_DATA, 1, data, data_size);
		HAL_I2C_Mem_Write(&hi2c1, SSD1306_I2C_ADDRESS, SSD1306_BYTE_DATA, 1, data, data_size, SSD1306_I2C_TIMEOUT);
  //SSD1306_state = SSD1306_BUSY; //ожидание прерывания по окончанию передачи
	//HAL_I2C_Master_Transmit (&hi2c1, SSD1306_I2C_ADDRESS, data, data_size, SSD1306_I2C_TIMEOUT);
}

/*----------------------------------------------------------------------------*/
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
 // SSD1306_state = SSD1306_READY;
}

/*----------------------------------------------------------------------------*/
