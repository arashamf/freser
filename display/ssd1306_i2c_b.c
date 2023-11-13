
#include "ssd1306_i2c.h"

/* Private variables */
static uint16_t ssd1306_I2C_TIMEOUT=20000;


/* Private defines */
#define I2C_TRANSMITTER_MODE   0
#define I2C_RECEIVER_MODE      1
#define I2C_ACK_ENABLE         1
#define I2C_ACK_DISABLE        0

//------------------------------------------------------------------------------------------//
void del1 (uint32_t delay) 
{
	for (uint32_t n=0; n<delay; n++) {} ;
}
#define I2C_DEL  5
//uint16_t I2C_DEL = 5;    // 20

//------------------------------------------------------------------------------------------//
void ssd1306_I2C_Init() 
{
	RCC->AHBENR |= (RCC_AHBENR_GPIOAEN);
	RCC->APB1ENR |= (RCC_APB1ENR_I2C1EN);
    
	GPIOA->MODER &= ~(GPIO_MODER_MODER9 | GPIO_MODER_MODER10); //сброс битов
	GPIOA->MODER |= GPIO_MODER_MODER9_0 | GPIO_MODER_MODER10_0; //режим выхода
	GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR9 | GPIO_OSPEEDR_OSPEEDR10; //High speed
	GPIOA->OTYPER |= GPIO_OTYPER_OT_10;	//SDA - open drain 
	GPIOA->PUPDR |= GPIO_PUPDR_PUPDR9_0 | GPIO_PUPDR_PUPDR10_0; // Pull-up
}

//------------------------------------------------------------------------------------------//
/* Private functions */
int16_t ssd1306_I2C_Start(uint8_t address, uint8_t direction, uint8_t ack) 
{
	SDAH; 
	del1(I2C_DEL); 
  SCLH; 
  del1(I2C_DEL); 
  SDAL; 
  del1(I2C_DEL); 
  SCLL; 
  del1(I2C_DEL);
	uint8_t ack1 = ssd1306_I2C_WriteData(address | direction);
	return ack1;

}

//------------------------------------------------------------------------------------------//
uint8_t I2C_ReadData(void) 
{
	uint8_t data=0;
	for (uint8_t n=0; n<8; n++)
	{
		del1(I2C_DEL); SCLH; del1(I2C_DEL);
		if (I2C_SDA) data|=(0x80>>n); del1(I2C_DEL);
		{	SCLL; }
  }
	SDAH;
	del1(I2C_DEL); 
	SCLH; 
	del1(I2C_DEL); 
	SCLL; 
	del1(I2C_DEL); 
	SDAH; 
	del1(I2C_DEL);
	return data;	
}

//------------------------------------------------------------------------------------------//
uint8_t I2C_Read(uint8_t address, uint16_t reg) 
{
	//I2C_DEL=20;
	uint8_t data;
	ssd1306_I2C_Start(address, I2C_TRANSMITTER_MODE, I2C_ACK_DISABLE);
	ssd1306_I2C_WriteData((reg>>8)&0x00FF);
	ssd1306_I2C_WriteData(reg&0x00FF);
	ssd1306_I2C_Start(address, I2C_RECEIVER_MODE, I2C_ACK_ENABLE);
	data = I2C_ReadData();
	ssd1306_I2C_Stop(); 
	del1(I2C_DEL); //I2C_DEL=5;
	return data;
}

//------------------------------------------------------------------------------------------//
void I2C_Write32(uint8_t address, uint16_t reg, uint32_t data) 
{
	//I2C_DEL=20;
	ssd1306_I2C_Start(address, I2C_TRANSMITTER_MODE, I2C_ACK_DISABLE); //del1(I2C_DEL);
	ssd1306_I2C_WriteData((reg>>8)&0x00FF); //del1(I2C_DEL);
	ssd1306_I2C_WriteData(reg&0x00FF); //del1(I2C_DEL);
	
	ssd1306_I2C_WriteData((data>>24)&0x000000FF); //del1(I2C_DEL);
	ssd1306_I2C_WriteData((data>>16)&0x000000FF); //del1(I2C_DEL);
	ssd1306_I2C_WriteData((data>> 8)&0x000000FF); //del1(I2C_DEL);
	ssd1306_I2C_WriteData((data>> 0)&0x000000FF); //del1(I2C_DEL);
	ssd1306_I2C_Stop(); 
	del1(I2C_DEL<<3);
	
}

//------------------------------------------------------------------------------------------//
void ssd1306_I2C_WriteMulti(uint8_t address, uint8_t reg, uint8_t* data, uint16_t count) 
{
	uint8_t i;
	ssd1306_I2C_Start(address, I2C_TRANSMITTER_MODE, I2C_ACK_DISABLE); 
	ssd1306_I2C_WriteData(reg);
	for (i = 0; i < count; i++) 
	{	ssd1306_I2C_WriteData(data[i]);	}
	ssd1306_I2C_Stop();
}
 
//------------------------------------------------------------------------------------------//
uint8_t ssd1306_I2C_WriteData(uint8_t data)
{
	uint8_t ACK=1;
	
	for (uint8_t n=0; n<8; n++)
  {
		if (data&(0x80>>n))
		{	SDAH; }
		else 
		{	SDAL;}
    del1(I2C_DEL); 
		SCLH; 
		del1(I2C_DEL); 
		SCLL; 
		del1(I2C_DEL); 
	}
  SDAH; del1(I2C_DEL); SCLH; del1(I2C_DEL);

  for (uint8_t n=0; n<180; n++)
  {
		if (I2C_SDA) 
			ACK=1;  
		else 
		{
			ACK=0; 
			break;
		} 
		del1(1); //
	}
	SCLL; //SDAL;
	del1(I2C_DEL);	
	
	return ACK;
		
}

//------------------------------------------------------------------------------------------//
void ssd1306_I2C_Write(uint8_t address, uint8_t reg, uint8_t data) 
{
	ssd1306_I2C_Start(address, I2C_TRANSMITTER_MODE, I2C_ACK_DISABLE);
	ssd1306_I2C_WriteData(reg);
	ssd1306_I2C_WriteData(data);
	ssd1306_I2C_Stop();
}

//------------------------------------------------------------------------------------------//
uint8_t ssd1306_I2C_Stop() 
{
    SDAL; del1(I2C_DEL); 
    SCLH; del1(I2C_DEL); 
    SDAH; del1(I2C_DEL);
    
	return 0;
}

//------------------------------------------------------------------------------------------//
uint8_t ssd1306_I2C_IsDeviceConnected(uint8_t address) 
{
	uint8_t connected = 0;
	/* Try to start, function will return 0 in case device will send ACK */
	if (!ssd1306_I2C_Start(address, I2C_TRANSMITTER_MODE, I2C_ACK_ENABLE)) {
		connected = 1;
	}

	/* STOP I2C */
	ssd1306_I2C_Stop();

	/* Return status */
	return connected;
}
//------------------------------------------------------------------------------------------//
