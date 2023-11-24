
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
/*void ssd1306_SendCommand ()
{
  uint16_t i;
  //LL_I2C_DisableBitPOS(I2C1); //Disable Pos
  LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK); //the generation of a ACKnowledge or Non ACKnowledge condition after the address receive match code or next received byte
  LL_I2C_GenerateStartCondition(I2C1); //генерирования условия START
  while(LL_I2C_IsActiveFlag_BUSY(I2C1)){}; //SET: When a Start condition is detected
  (void) I2C1->ISR; //
  LL_I2C_TransmitData8(I2C1, EEPROM_I2C1_ADDRESS | I2C_REQUEST_WRITE);
  while(!LL_I2C_IsActiveFlag_ADDR(I2C1)){}; //SET: When the received slave address matched with one of the enabled slave address
  LL_I2C_ClearFlag_ADDR(I2C1); //Установка этого бита очищает флаг ADDR в регистре I2C_ISR
  
	LL_I2C_TransmitData8(I2C1, (uint8_t) (addr>>8)); //старшая часть 16 битного адреса микросхемы памяти
  while(!LL_I2C_IsActiveFlag_TXE(I2C1)){};
  LL_I2C_TransmitData8(I2C1, (uint8_t) addr);
  while(!LL_I2C_IsActiveFlag_TXE(I2C1)){};
		
  for(i=0;i<bytes_count;i++) //передача данных побайтно
  {
    LL_I2C_TransmitData8(I2C1, buf[i]);
    while(!LL_I2C_IsActiveFlag_TXE(I2C1)){};
  }
  LL_I2C_GenerateStopCondition(I2C1);
}*/


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
