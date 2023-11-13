
#ifndef ssd1306_I2C_H
#define ssd1306_I2C_H 161

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx.h"

/* Define -------------------------------------------------------------------*/
#ifndef ssd1306_I2C_TIMEOUT
//#define ssd1306_I2C_TIMEOUT					2000
#endif


/* Macro ------------------------------------------------------------*/
#define SCLH  GPIOA->BSRR |= GPIO_BRR_BR_9 //I2C 
#define SCLL  GPIOA->BRR  |= GPIO_BSRR_BS_9
#define SDAH  GPIOA->BSRR |= GPIO_BRR_BR_10
#define SDAL  GPIOA->BRR  |= GPIO_BSRR_BS_10
#define				I2C_SDA   	(GPIOA->IDR&0x0400) >> 10

/*Functions prototypes ---------------------------------------------*/
void ssd1306_I2C_Init();

/**
 * @brief  Writes single byte to slave
 * @param  *I2Cx: I2C used
 * @param  address: 7 bit slave address, left aligned, bits 7:1 are used, LSB bit is not used
 * @param  reg: register to write to
 * @param  data: data to be written
 * @retval None
 */


uint8_t I2C_Read(uint8_t address, uint16_t reg);

void ssd1306_I2C_Write(uint8_t address, uint8_t reg, uint8_t data);

//void ssd1306_I2C_Write16(uint8_t address, uint16_t reg, uint8_t data);

void I2C_Write32(uint8_t address, uint16_t reg, uint32_t data);

/**
 * @brief  Writes multi bytes to slave
 * @param  *I2Cx: I2C used
 * @param  address: 7 bit slave address, left aligned, bits 7:1 are used, LSB bit is not used
 * @param  reg: register to write to
 * @param  *data: pointer to data array to write it to slave
 * @param  count: how many bytes will be written
 * @retval None
 */
void ssd1306_I2C_WriteMulti(uint8_t address, uint8_t reg, uint8_t *data, uint16_t count);

/**
 * @brief  I2C Start condition
 * @param  *I2Cx: I2C used
 * @param  address: slave address
 * @param  direction: master to slave or slave to master
 * @param  ack: ack enabled or disabled
 * @retval Start condition status
 * @note   For private use
 */
int16_t ssd1306_I2C_Start(uint8_t address, uint8_t direction, uint8_t ack);

/**
 * @brief  Stop condition on I2C
 * @param  *I2Cx: I2C used
 * @retval Stop condition status
 * @note   For private use
 */
uint8_t ssd1306_I2C_Stop();

/**
 * @brief  Writes to slave
 * @param  *I2Cx: I2C used
 * @param  data: data to be sent
 * @retval None
 * @note   For private use
 */
uint8_t ssd1306_I2C_WriteData(uint8_t data);

uint8_t ssd1306_I2C_IsDeviceConnected(uint8_t address);


#endif