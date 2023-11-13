
#ifndef SSD1306_H
#define SSD1306_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Declarations and definitions ----------------------------------------------*/
#define SSD1306_X_SIZE                                            128
#define SSD1306_Y_SIZE                                            32
#define SSD1306_BUFFER_SIZE                                       (SSD1306_X_SIZE*SSD1306_Y_SIZE) / 8
#define LCD_DEFAULT_SPACE 																				5

//#define OLED_adress 							0x78
#define SETCONTRAST 							0x81
#define DISPLAYALLON_RESUME 			0xA4
#define DISPLAYALLON 							0xA5
#define NORMALDISPLAY 						0xA6
#define OLED_INVERTDISPLAY 				0xA7
#define OLED_DISPLAYOFF 					0xAE
#define DISPLAYON 								0xAF
#define SETDISPLAYOFFSET 					0xD3
#define SETCOMPINS 								0xDA
#define SETVCOMDETECT							0xDB
#define SETDISPLAYCLOCKDIV 				0xD5
#define SETPRECHARGE 							0xD9
#define SETMULTIPLEX 							0xA8
#define OLED_SETLOWCOLUMN    			0x00
#define OLED_SETHIGHCOLUMN 				0x10
#define SETSTARTLINE 							0x40
#define MEMORYMODE 								0x20
#define OLED_COLUMNADDR 					0x21
#define OLED_PAGEADDR   					0x22
#define OLED_COMSCANINC 					0xC0
#define COMSCANDEC 								0xC8
#define SEGREMAP 									0xA0
#define CHARGEPUMP 								0x8D
#define OLED_SWITCHCAPVCC 				0x02
#define OLED_NOP 									0xE3

/* Functions -----------------------------------------------------------------*/
extern void SSD1306_Init();
void ssd1306_Goto(unsigned char x, unsigned char y);
void ssd1306_PutChar(unsigned int c);
void ssd1306_PutString(char *string);
void ssd1306_num_to_str(unsigned int value, unsigned char nDigit);
void ssd1306_Clear(void);
void SSD1306_ClearScreen(void);
void SSD1306_UpdateScreen(void);
void SSD1306_DrawFilledRect(uint8_t xStart, uint8_t xEnd, uint8_t yStart, uint8_t yEnd);
uint8_t SSD1306_IsReady(void);

#endif // #ifndef SSD1306_H
