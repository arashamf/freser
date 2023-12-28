
#ifndef SSD1306_H
#define SSD1306_H

// Includes ---------------------------------------------------------------------//
#include "main.h"
#include "typedef.h"

// Defines ---------------------------------------------------------------------//
#define SSD1306_X_SIZE                                  128
#define SSD1306_Y_SIZE                                  32
#define SSD1306_BUFFER_SIZE                             (SSD1306_X_SIZE*SSD1306_Y_SIZE) / 8
#define SIZE_TEMP_BUFFER   															6
#define LCD_DEFAULT_X_SIZE  														5
#define LCD_DEFAULT_Y_SIZE 															0

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
void ssd1306_Goto(unsigned char x, unsigned char y);
void ssd1306_PutChar(unsigned int c);
void ssd1306_PutString(char *string);
void ssd1306_num_to_str(unsigned int value, unsigned char nDigit);
void ssd1306_Clear(void);
void ssd1306_PutData (uint8_t coordinate_X, uint8_t coordinate_Y, char * buffer, uint8_t need_clear);
void default_screen_mode1 (angular_data_t * );
void default_screen_mode2 (milling_data_t * );
uint8_t SSD1306_IsReady(void);

// Variables ------------------------------------------------------------------//
extern char LCD_buff[20];
extern uint8_t kord_X; 
extern uint8_t kord_Y;
#endif // #ifndef SSD1306_H
