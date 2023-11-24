
/* Includes ------------------------------------------------------------------*/
#include "eeprom.h"

/* Functions -----------------------------------------------------------------*/

#define EEPROM_I2C1_ADDRESS     	(0xA0)	 // A0 = A1 = A2 = 0                                        

#define I2C_REQUEST_WRITE       	0x00
#define I2C_REQUEST_READ        	0x01

#define EEPROM_I2C_TIMEOUT    		0xFF

/* Private typedef -----------------------------------------------------------*/
extern I2C_HandleTypeDef hi2c1;

/* Private variables ---------------------------------------------------------*/
static	uint8_t tmp_buf[50];

//---------------------------------------------------------------------//
void EEPROM_WriteBytes (uint16_t addr, uint8_t *buf, uint16_t bytes_count)
{
 /* uint16_t i;
  //LL_I2C_DisableBitPOS(I2C1); //Disable Pos
  LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK); //the generation of a ACKnowledge or Non ACKnowledge condition after the address receive match code or next received byte
  LL_I2C_GenerateStartCondition(I2C1); //генерирования условия START
  while(LL_I2C_IsActiveFlag_BUSY(I2C1)){}; //SET: When a Start condition is detected
  (void) I2C1->ISR; //
  LL_I2C_TransmitData8(I2C1, EEPROM_I2C1_ADDRESS | I2C_REQUEST_WRITE);
 // while(!LL_I2C_IsActiveFlag_ADDR(I2C1)){}; //SET: When the received slave address matched with one of the enabled slave address
 // LL_I2C_ClearFlag_ADDR(I2C1); //Установка этого бита очищает флаг ADDR в регистре I2C_ISR
  
	LL_I2C_TransmitData8(I2C1, (uint8_t) (addr>>8)); //старшая часть 16 битного адреса страницы памяти 
  while(!LL_I2C_IsActiveFlag_TXE(I2C1)){};
  LL_I2C_TransmitData8(I2C1, (uint8_t) addr);
  while(!LL_I2C_IsActiveFlag_TXE(I2C1)){};
		
  for(i=0;i<bytes_count;i++) //передача данных побайтно
  {
    LL_I2C_TransmitData8(I2C1, buf[i]);
    while(!LL_I2C_IsActiveFlag_TXE(I2C1)){};
  }
  LL_I2C_GenerateStopCondition(I2C1);*/
//	HAL_I2C_Mem_Write(&hi2c1, EEPROM_I2C1_ADDRESS, addr, 1, buf, bytes_count, EEPROM_I2C_TIMEOUT);
	tmp_buf[0] = (uint8_t)addr>>8;
	tmp_buf[1] = (uint8_t) addr;
	for (uint8_t count = 0; count < (bytes_count); count++)
		tmp_buf[count+2] = *(buf + count);
	HAL_I2C_Master_Transmit (&hi2c1, EEPROM_I2C1_ADDRESS, tmp_buf, bytes_count+2, EEPROM_I2C_TIMEOUT);
//	HAL_I2C_Master_Transmit(&hi2c1, EEPROM_I2C1_ADDRESS, buf, bytes_count, EEPROM_I2C_TIMEOUT);
}

//--------------------------------------------------------------------//
void EEPROM_ReadBytes (uint16_t addr, uint8_t *buf, uint16_t bytes_count)
{  
	/*uint16_t i;  
  //LL_I2C_DisableBitPOS(I2C1); //Disable Pos
  LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_ACK);
  LL_I2C_GenerateStartCondition(I2C1);
  while(LL_I2C_IsActiveFlag_BUSY(I2C1)){};
  (void) I2C1->ISR; //
  LL_I2C_TransmitData8(I2C1, EEPROM_I2C1_ADDRESS | I2C_REQUEST_WRITE);
  while(!LL_I2C_IsActiveFlag_ADDR(I2C1)){};
  LL_I2C_ClearFlag_ADDR(I2C1);
		
  LL_I2C_TransmitData8(I2C1, (uint8_t) (addr>>8));
  while(!LL_I2C_IsActiveFlag_TXE(I2C1)){};
  LL_I2C_TransmitData8(I2C1, (uint8_t) addr);
  while(!LL_I2C_IsActiveFlag_TXE(I2C1)){};
		
  LL_I2C_GenerateStartCondition(I2C1);
  while(LL_I2C_IsActiveFlag_BUSY(I2C1)){};
  (void) I2C1->ISR; //
  LL_I2C_TransmitData8(I2C1, EEPROM_I2C1_ADDRESS | I2C_REQUEST_READ);
 // while (!LL_I2C_IsActiveFlag_ADDR(I2C1)){};
 // LL_I2C_ClearFlag_ADDR(I2C1);
		
  for(i=0;i<bytes_count;i++)
  {
    if(i<(bytes_count-1))
    {
      while(!LL_I2C_IsActiveFlag_RXNE(I2C1)){};
      buf[i] = LL_I2C_ReceiveData8(I2C1);
    }
    else
    {
      LL_I2C_AcknowledgeNextData(I2C1, LL_I2C_NACK);
      LL_I2C_GenerateStopCondition(I2C1);
      while(!LL_I2C_IsActiveFlag_RXNE(I2C1)){};
      buf[i] = LL_I2C_ReceiveData8(I2C1);
    }
  }*/
//	HAL_I2C_Mem_Read (&hi2c1, EEPROM_I2C1_ADDRESS, addr, 1, buf, bytes_count, EEPROM_I2C_TIMEOUT);
	tmp_buf[0] = (uint8_t)addr>>8;
	tmp_buf[1] = (uint8_t) addr;
	HAL_I2C_Master_Transmit (&hi2c1, EEPROM_I2C1_ADDRESS, tmp_buf, 2, EEPROM_I2C_TIMEOUT);
	HAL_I2C_Master_Receive(&hi2c1, EEPROM_I2C1_ADDRESS, buf, bytes_count, EEPROM_I2C_TIMEOUT);
}

//--------------------------------------------------------------------//

