
// Includes ------------------------------------------------------------------//
#include "ssd1306.h"
#include "ssd1306_interface.h"
#include "fonts.h"
#include "tim.h"

// Declarations and definitions -----------------------------------------------//
static void SSD1306_ClearScreen(void);
static void SSD1306_UpdateScreen(void);

// Private variables ---------------------------------------------------------//
//SSD1306_State SSD1306_state = SSD1306_READY;
static uint8_t 	temp_char[SIZE_TEMP_BUFFER];
static uint8_t 	LCD_X=0; 
static uint8_t	LCD_Y=0;

uint8_t kord_X	=	LCD_DEFAULT_X_SIZE ;  
uint8_t	kord_Y	=	LCD_DEFAULT_Y_SIZE ; 
char LCD_buff[20];

// Functions -----------------------------------------------------------------//
void ssd1306_Init()
{   
	ssd1306_SendCommand(DISPLAYOFF); 		//display off
	ssd1306_SendCommand(MEMORYMODE); 	//Set Memory Addressing Mode   
	ssd1306_SendCommand(HORIZONTALMODE); //00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
/*
  // Set column address
	ssd1306_SendCommand(COLUMNADDR); //
	ssd1306_SendCommand(0); //
	ssd1306_SendCommand(127); //
	
	// Set page address
	ssd1306_SendCommand(PAGEADDR ); //
	ssd1306_SendCommand(0); //
	ssd1306_SendCommand(3); //
*/
	ssd1306_SendCommand(STARTPAGEADDR); //Set Page Start Address for Page Addressing Mode,0-7
	ssd1306_SendCommand(COMSCANDEC); //Set COM Output Scan Direction
	
	ssd1306_SendCommand(SETLOWCOLUMN); //---set low column address
	ssd1306_SendCommand(SETHIGHCOLUMN); //---set high column address
	ssd1306_SendCommand(SETSTARTLINE); //--set start line address
	
	ssd1306_SendCommand(SETCONTRAST); //--set contrast control register
	ssd1306_SendCommand(0xFF);
	
	ssd1306_SendCommand(SETSEGREMAP+0x01); //--set segment re-map 0 to 127 
	ssd1306_SendCommand(NORMALDISPLAY); //--set normal display
	
	ssd1306_SendCommand(SETMULTIPLEX); //--set multiplex ratio(1 to 64)
	ssd1306_SendCommand(0x1F); //128x32
	
	ssd1306_SendCommand(0xA4); //0xA4-output follows RAM content;0xA5-output ignores RAM content
	
	ssd1306_SendCommand(SETDISPLAYOFFSET); //-set display offset
	ssd1306_SendCommand(0x00); //-not offset
	
	ssd1306_SendCommand(SETDISPLAYCLOCKDIV); //--set display clock divide ratio/oscillator frequency
	ssd1306_SendCommand(SETDIVIDERATIO); //--set divide ratio
	
	ssd1306_SendCommand(SETPRECHARGE); //--set pre-charge period
	ssd1306_SendCommand(0x22); //
	
	ssd1306_SendCommand(SETCOMPINS); //--set com pins hardware configuration
	ssd1306_SendCommand(0x12); //128x64
	//ssd1306_SendCommand(0x02); //128x32
	
	ssd1306_SendCommand(SETVCOMDETECT); 	//--set vcomh
	ssd1306_SendCommand(SWITCHCAPVCC); 		//
	
//	ssd1306_SendCommand(CHARGEPUMP); //--set DC-DC enable
//	ssd1306_SendCommand(0x14); //
	ssd1306_SendCommand(DISPLAYON); //--turn on SSD1306 panel
}

//----------------------------------------------------------------------------//
void ssd1306_Goto(unsigned char x, unsigned char y)
{

	LCD_X = x;
	LCD_Y = y;
	ssd1306_SendCommand(0xB0+y);
	ssd1306_SendCommand(x & 0xF);
	ssd1306_SendCommand(0x10 | (x>>4));
}

//-------------------------------------------------------------------------------------------------//
void ssd1306_PutChar(unsigned int c)
{
	for (unsigned char x=0; x<(SIZE_TEMP_BUFFER-1); x++) //если используется ф-я HAL_I2C_Mem_Write()
	{
		temp_char[x] = LCD_font[c*5+x]; //5 элементов из массива со шрифтом
	}
	temp_char[SIZE_TEMP_BUFFER-1] = 0; //последний элемент сообщения - 0
	ssd1306_SendDataBuffer(temp_char, SIZE_TEMP_BUFFER);
	
	LCD_X += 8; //сдвиг по оси Х
	if(LCD_X > SSD1306_X_SIZE) //если вывалились за предел дисплея
	{	LCD_X = LCD_DEFAULT_X_SIZE;	} //возврат в начало дисплея
}

//-------------------------------------------------------------------------------------------------//
void ssd1306_PutString(char *string)
{
	while(*string != '\0')
	{  
		ssd1306_PutChar(*string);
		string++;
	}
}

//------------------------------------------------------------------------------------------------//
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

//------------------------------------------------------------------------------------------------//
void ssd1306_Clear(void)
{
	unsigned short i;
	unsigned short x = 0;
	unsigned short y = 0;
	ssd1306_Goto(0,0);

	for (i=0; i<(SSD1306_BUFFER_SIZE ); i++) //перебор всех пикселей
	{
		ssd1306_PutChar(' '); //запись пробела
		x++;
		if(x > SSD1306_X_SIZE) //если вышли за пределы дисплея
		{
			x = 0;
			y++;
			ssd1306_Goto(x, y);
		}
	}
}

//------------------------------------------------------------------------------------------------//
void ssd1306_PutData (uint8_t coordinate_X, uint8_t coordinate_Y, char * buffer, uint8_t need_clear)
{
	if ( need_clear)
	{	ssd1306_Clear(); }
	ssd1306_Goto(coordinate_X, coordinate_Y);
	delay_us (3000);
	ssd1306_PutString(buffer);
}

//-----------------------------------------------------------------------------------------------//
void default_screen_mode1 (angular_data_t* rotation)
{
	snprintf (LCD_buff, sizeof(LCD_buff), "%03d* %02d' %02d\"", rotation->shaft_degree, 
	rotation->shaft_minute, rotation->shaft_second);
	ssd1306_PutData (kord_X, kord_Y, LCD_buff, DISP_CLEAR);
	
	snprintf (LCD_buff, sizeof(LCD_buff), "%03d* %02d' %02d\"", rotation->set_degree, rotation->set_minute, 
	rotation->set_second);
	ssd1306_PutData (kord_X, kord_Y+1, LCD_buff, DISP_NOT_CLEAR);
}


//-----------------------------------------------------------------------------------------------//
void default_screen_mode2 (milling_data_t* handle)
{
	snprintf (LCD_buff, sizeof(LCD_buff), "set=%03d@ rem=%03d@", handle->teeth_gear_numbers, 
	handle->remain_teeth_gear);
	ssd1306_PutData (kord_X, kord_Y, LCD_buff, DISP_CLEAR);
	
	snprintf (LCD_buff, sizeof(LCD_buff), "%03d* %02d' %02d\"", handle->step_shaft_degree, 
	handle->step_shaft_minute, handle->step_shaft_second);
	ssd1306_PutData (kord_X, kord_Y+1, LCD_buff, DISP_NOT_CLEAR);
}

//-----------------------------------------------------------------------------------------------//
void setangle_mode_screen (angular_data_t* handle)
{
	snprintf (LCD_buff, sizeof(LCD_buff), "setup mode1");
	ssd1306_PutData (kord_X, kord_Y, LCD_buff, DISP_CLEAR);	
	
	snprintf (LCD_buff, sizeof(LCD_buff), "%03d* %02d' %02d\"", handle->set_degree, 
	handle->set_minute, handle->set_second);
	ssd1306_PutData (kord_X, kord_Y+1, LCD_buff, DISP_NOT_CLEAR);
}

//-----------------------------------------------------------------------------------------------//
void return_mode_screen (angular_data_t* handle)
{	
	snprintf (LCD_buff, sizeof(LCD_buff), "%03d* %02d' %02d\"", handle->shaft_degree, 
	handle->shaft_minute, handle->shaft_second);
	ssd1306_PutData (kord_X, kord_Y, LCD_buff, DISP_CLEAR);	
	
	snprintf (LCD_buff, sizeof(LCD_buff), "<- push ->");	
	ssd1306_PutData (kord_X, kord_Y+1, LCD_buff, DISP_NOT_CLEAR);
}

//-----------------------------------------------------------------------------------------------//
void setteeth_mode_screen (milling_data_t* handle)
{	
	snprintf (LCD_buff, sizeof(LCD_buff), "setup mode2");
	ssd1306_PutData (kord_X, kord_Y, LCD_buff, DISP_CLEAR);	
	snprintf (LCD_buff, sizeof(LCD_buff), "%03d@", handle->teeth_gear_numbers);
	ssd1306_PutData (kord_X, kord_Y+1, LCD_buff, DISP_NOT_CLEAR);
}

//-----------------------------------------------------------------------------------------------//
