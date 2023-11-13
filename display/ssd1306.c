
/* Includes ------------------------------------------------------------------*/
#include "ssd1306.h"
#include "ssd1306_interface.h"
#include "fonts.h"

/* Declarations and definitions ----------------------------------------------*/
#define SIZE_TEMP_BUFFER   7

SSD1306_State SSD1306_state = SSD1306_READY;
static uint8_t pixelBuffer[SSD1306_BUFFER_SIZE];
static uint8_t temp_char[SIZE_TEMP_BUFFER];
uint8_t LCD_X, LCD_Y;

/* Functions -----------------------------------------------------------------*/
void SSD1306_Init()
{   
 // uint8_t data[3];
   
 /* ssd1306_SendCommand(OLED_DISPLAYOFF); // Set display off
  
  ssd1306_SendCommand (SETDISPLAYCLOCKDIV );   // Set oscillator frequency
	ssd1306_SendCommand (0x80);
  
	ssd1306_SendCommand(SETMULTIPLEX); // Set MUX ratio
  //ssd1306_SendCommand(0x3F); //128x64
	ssd1306_SendCommand(0x1F); //128x32
	
	ssd1306_SendCommand(SETDISPLAYOFFSET );   // Set display offset
	ssd1306_SendCommand(0x00);
	
	ssd1306_SendCommand(SETSTARTLINE);  // Set display start line
	
  ssd1306_SendCommand(CHARGEPUMP);   // Enable charge pump regulator
  ssd1306_SendCommand(0x14);

  ssd1306_SendCommand(MEMORYMODE); // Set horizontal addressing mode
  ssd1306_SendCommand(0x00);
	
  ssd1306_SendCommand(SEGREMAP | 0x01); 	// Set segment remap
	
  ssd1306_SendCommand(COMSCANDEC); // Set COM output scan direction
  
  ssd1306_SendCommand(SETCOMPINS);   // Set COM pins hardware configuration
 // ssd1306_SendCommand(0x12); //128x64
	ssd1306_SendCommand(0x02); //128x32
	
	// Set contrast
	ssd1306_SendCommand(SETCONTRAST);
  ssd1306_SendCommand(0x7F);
	
	ssd1306_SendCommand(SETPRECHARGE);
  ssd1306_SendCommand(0xF1);
  
	ssd1306_SendCommand(SETVCOMDETECT);
  ssd1306_SendCommand(0x40);
	
  // Set column address
//  ssd1306_SendCommand(0x21);
//  ssd1306_SendCommand(0x00);
  //ssd1306_SendCommand(127);
  
  // Set page address
 // ssd1306_SendCommand(0x22);
 // ssd1306_SendCommand(0x00);
 // ssd1306_SendCommand(7);

  ssd1306_SendCommand(DISPLAYALLON_RESUME);   				// Entire display on
  ssd1306_SendCommand(NORMALDISPLAY); 								//Set normal display
  ssd1306_SendCommand(DISPLAYON );   									// Set display on*/
	
	ssd1306_SendCommand(0xAE); //display off
	ssd1306_SendCommand(0x20); //Set Memory Addressing Mode   
	ssd1306_SendCommand(0x00); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
/*
	ssd1306_SendCommand(0x21); //
	ssd1306_SendCommand(0); //
	ssd1306_SendCommand(127); //
	ssd1306_SendCommand(0x22); //
	ssd1306_SendCommand(0); //
	ssd1306_SendCommand(3); //
*/
	ssd1306_SendCommand(0xB0); //Set Page Start Address for Page Addressing Mode,0-7
	ssd1306_SendCommand(0xC8); //Set COM Output Scan Direction
	ssd1306_SendCommand(0x00); //---set low column address
	ssd1306_SendCommand(0x10); //---set high column address
	ssd1306_SendCommand(0x40); //--set start line address
	ssd1306_SendCommand(0x81); //--set contrast control register
	ssd1306_SendCommand(0xFF);
	ssd1306_SendCommand(0xA1); //--set segment re-map 0 to 127  0xA1
	ssd1306_SendCommand(0xA6); //--set normal display
	ssd1306_SendCommand(0xA8); //--set multiplex ratio(1 to 64)
	ssd1306_SendCommand(0x1F); //
	ssd1306_SendCommand(0xA4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	ssd1306_SendCommand(0xD3); //-set display offset
	ssd1306_SendCommand(0x00); //-not offset
	ssd1306_SendCommand(0xD5); //--set display clock divide ratio/oscillator frequency
	ssd1306_SendCommand(0xF0); //--set divide ratio
	ssd1306_SendCommand(0xD9); //--set pre-charge period
	ssd1306_SendCommand(0x22); //
	ssd1306_SendCommand(0xDA); //--set com pins hardware configuration
	ssd1306_SendCommand(0x12);
	ssd1306_SendCommand(0xDB); //--set vcomh
	ssd1306_SendCommand(0x02); //0x20,0.77xVcc     00XXXX00b
//	ssd1306_SendCommand(0x8D); //--set DC-DC enable
//	ssd1306_SendCommand(0x14); //
	ssd1306_SendCommand(0xAF); //--turn on SSD1306 panel
}

/*----------------------------------------------------------------------------*/
void ssd1306_Goto(unsigned char x, unsigned char y)
{

	LCD_X = x;
	LCD_Y = y;
	ssd1306_SendCommand(0xB0 + y);
	ssd1306_SendCommand(x & 0xF);
	ssd1306_SendCommand(0x10 | (x >> 4));
}

/*----------------------------------------------------------------------------*/
void ssd1306_PutChar(unsigned int c)
{
	temp_char[0] = 0x40;
	for (unsigned char x=0; x<5; x++)
	{
		temp_char[x+1] = LCD_Buffer[c*5+x];
	}
	temp_char[6] = 0;
	ssd1306_SendDataBuffer(temp_char, SIZE_TEMP_BUFFER);
	
	LCD_X += 8;
	if(LCD_X>SSD1306_X_SIZE )
	{
		LCD_X = LCD_DEFAULT_SPACE;
	}
}

/*----------------------------------------------------------------------------*/
void ssd1306_PutString(char *string)
{
	while(*string != '\0')
	{  
		ssd1306_PutChar(*string);
		string++;
	}
}

/*----------------------------------------------------------------------------*/
void ssd1306_num_to_str(unsigned int value, unsigned char nDigit)
{
	switch(nDigit)
	{
		case 5: ssd1306_PutChar(value/10000+48);
		case 4: ssd1306_PutChar((value/1000)%10+48);
		case 3: ssd1306_PutChar((value/100)%10+48);
		case 2: ssd1306_PutChar((value/10)%10+48);
		case 1: ssd1306_PutChar(value%10+48);
	}
}

/*----------------------------------------------------------------------------*/
void ssd1306_Clear(void)
{
	unsigned short i;
	unsigned short x=0;
	unsigned short y=0;
	ssd1306_Goto(0,0);

	for (i=0; i<(SSD1306_BUFFER_SIZE ); i++) //(SSD1306_LCDWIDTH*SSD1306_LCDHEIGHT/8)
	{
		ssd1306_PutChar(' ');
		x ++;
		if(x>SSD1306_X_SIZE)
		{
			x =0;
			y++;
			ssd1306_Goto(0,y);
		}
	}
	LCD_X = LCD_DEFAULT_SPACE;
	LCD_Y =0;
}

/*----------------------------------------------------------------------------*/
void SSD1306_ClearScreen()
{
  for (uint16_t i = 0; i < SSD1306_BUFFER_SIZE; i++)
  {
    pixelBuffer[i] = 0x00;
  }

  SSD1306_UpdateScreen();
}

/*----------------------------------------------------------------------------*/
void SSD1306_UpdateScreen()
{  
  ssd1306_SendDataBuffer(pixelBuffer, SSD1306_BUFFER_SIZE);
}

/*----------------------------------------------------------------------------*/
static void SetPixel(uint8_t x, uint8_t y)
{
  pixelBuffer[x + (y / 8) * SSD1306_X_SIZE] |= (1 << (y % 8));
}



/*----------------------------------------------------------------------------*/
void SSD1306_DrawFilledRect(uint8_t xStart, uint8_t xEnd, uint8_t yStart, uint8_t yEnd)
{
  for (uint8_t i = xStart; i < xEnd; i++)
  {
    for (uint8_t j = yStart; j <  yEnd; j++)
    {
      SetPixel(i, j);
    }
  }
}



/*----------------------------------------------------------------------------*/
uint8_t SSD1306_IsReady()
{
  if (SSD1306_state == SSD1306_BUSY)
  {
    return 0;
  }  
  return 1;
}


