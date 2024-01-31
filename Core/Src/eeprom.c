
// Includes --------------------------------------------------------------------//
#include "eeprom.h"

// Functions -------------------------------------------------------------------//

// Defines ---------------------------------------------------------------------//
#define EEPROM_I2C1_ADDRESS     	0xA0	 // A0 = A1 = A2 = 0                                        
#define I2C_REQUEST_WRITE       	0x00
#define I2C_REQUEST_READ        	0x01
#define EEPROM_I2C_TIMEOUT    		0xFF

// Private typedef ------------------------------------------------------------//
extern I2C_HandleTypeDef hi2c1;

// Private variables ---------------------------------------------------------//
//uint8_t eeprom_rx_buffer[20] = {0};
uint8_t eeprom_tx_buffer[EEPROM_NUMBER_BYTES+1] = {0} ;

//---------------------------------------------------------------------//
void EEPROM_WriteBytes (uint16_t addr, uint8_t *buf, uint16_t bytes_count)
{
	HAL_I2C_Mem_Write (&hi2c1, EEPROM_I2C1_ADDRESS, addr, 2, buf, bytes_count, EEPROM_I2C_TIMEOUT);
}

//------------------------------------------------------------------------------------------------//
void EEPROM_ReadBytes (uint16_t addr, uint8_t *buf, uint16_t bytes_count)
{  
	HAL_I2C_Mem_Read (&hi2c1, EEPROM_I2C1_ADDRESS, addr, 2, buf, bytes_count, EEPROM_I2C_TIMEOUT);
}
//------------------------------------------------------------------------------------------------//
