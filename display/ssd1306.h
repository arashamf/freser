
#ifndef SSD1306_H
#define SSD1306_H

// Includes ---------------------------------------------------------------------//
#include "main.h"
#include "fonts.h"
#include "ssd1306_interface.h"

// Exported types ---------------------------------------------------------------//
typedef enum 
{
	SSD1306_COLOR_BLACK = 0x00, // Black color, no pixel 
	SSD1306_COLOR_WHITE = 0x01  // Pixel is set. Color depends on LCD 
} SSD1306_COLOR_t;

// Defines ---------------------------------------------------------------------//
#define SSD1306_WRITECOMMAND(command)			ssd1306_SendCommand (command) // Write command 
#define SSD1306_WRITEDATA(data)						ssd1306_SendByteData(data) // Write data 
#define ABS(x)  ((x) > 0 ? (x) : -(x)) 		// Absolute value 

#define SSD1306_X_SIZE                                  132
#define SSD1306_Y_SIZE                                  32
#define SSD1306_BUFFER_SIZE                             (SSD1306_X_SIZE*SSD1306_Y_SIZE) / 8
#define LCD_BUFFER_SIZE 																16
#define LCD_DEFAULT_X_SIZE  														0
#define LCD_DEFAULT_Y_SIZE 															1

//#define OLED_adress 				0x78
#define DISPLAYOFF 						0xAE
#define DISPLAYON 						0xAF
#define MEMORYMODE 						0x20
#define HORIZONTALMODE 				0x00
#define VERTICALMODE 					0x01
#define STARTPAGEADDR   			0xB0
#define COMSCANDEC 						0xC8
#define SETLOWCOLUMN    			0x00
#define SETHIGHCOLUMN 				0x10
#define SETSTARTLINE 					0x40
#define SETCONTRAST 					0x81
#define SETSEGREMAP 					0xA0
#define NORMALDISPLAY 				0xA6
#define INVERTDISPLAY 				0xA7
#define SETMULTIPLEX 					0xA8
#define DISPLAYALLON_RESUME 	0xA4
#define DISPLAYALLON 					0xA5
#define SETDISPLAYOFFSET 			0xD3
#define SETDISPLAYCLOCKDIV 		0xD5
#define SETDIVIDERATIO				0xF0
#define SETPRECHARGE 					0xD9
#define SETCOMPINS 						0xDA
#define SETVCOMDETECT					0xDB
#define SWITCHCAPVCC 					0x02
#define CHARGEPUMP 						0x8D
#define COLUMNADDR 						0x21
#define PAGESDDR 							0x22
#define OMSCANINC 						0xC0


// Functions -----------------------------------------------------------------//
void ssd1306_Init();
void SSD1306_DrawPixel(uint16_t , uint16_t , SSD1306_COLOR_t );
char SSD1306_PutChar(char , FontDef_t* , SSD1306_COLOR_t );
char SSD1306_PutString(uint8_t* , FontDef_t* , SSD1306_COLOR_t );
void SSD1306_UpdateScreen(void);
void SSD1306_Clear_Screen (void);
void SSD1306_ToggleInvert(void);
void SSD1306_Fill(SSD1306_COLOR_t );
void SSD1306_GotoXY(uint16_t , uint16_t );
void SSD1306_DrawLine(uint16_t , uint16_t , uint16_t , uint16_t , SSD1306_COLOR_t );
void SSD1306_DrawRectangle(uint16_t , uint16_t , uint16_t , uint16_t , SSD1306_COLOR_t );
void SSD1306_DrawFilledRectangle(uint16_t , uint16_t , uint16_t , uint16_t , SSD1306_COLOR_t );
void SSD1306_DrawTriangle(uint16_t , uint16_t , uint16_t , uint16_t , uint16_t , uint16_t , SSD1306_COLOR_t );
void SSD1306_DrawFilledTriangle(uint16_t , uint16_t , uint16_t , uint16_t , uint16_t , uint16_t , SSD1306_COLOR_t );
void SSD1306_DrawCircle(int16_t , int16_t , int16_t , SSD1306_COLOR_t );
void SSD1306_DrawFilledCircle(int16_t , int16_t , int16_t , SSD1306_COLOR_t );
void default_screen_mode1 (angular_data_t * );
void default_screen_mode2 (milling_data_t * , STATUS_FLAG_t * );
void setangle_mode_screen (angular_data_t* );
void return_mode_screen (angular_data_t* );
void setteeth_mode_screen (milling_data_t* handle);
void select_rotation_mode_screen (void);

// Variables ------------------------------------------------------------------//
extern uint8_t LCD_buff[LCD_BUFFER_SIZE];
#endif // #ifndef SSD1306_H
