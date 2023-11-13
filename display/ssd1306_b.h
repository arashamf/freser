
#ifndef SSD1306_H
#define SSD1306_H 100

/* C++ detection */
#ifdef __cplusplus
extern C {
#endif

/*
 * This SSD1306 LCD uses I2C for communication
 * Library features functions for drawing lines, rectangles and circles.
 * It also allows you to draw texts and characters using appropriate functions provided in library.
 * Default pinout
SSD1306    |STM32F10x    |DESCRIPTION
VCC        |3.3V         |
GND        |GND          |
SCL        |PB6          |Serial clock line
SDA        |PB7          |Serial data line
 */

#include "stm32f0xx.h"    
#include "ssd1306_i2c.h"
#include "fonts.h"
#include "stdlib.h"
#include "string.h"
    /* I2C settings */
#ifndef SSD1306_I2C
    #define SSD1306_I2C              I2C1
#endif

/* I2C address */
#ifndef SSD1306_I2C_ADDR
    #define SSD1306_I2C_ADDR         0x78
#endif

/* SSD1306 settings */
/* SSD1306 width in pixels */
#ifndef SSD1306_WIDTH
#define SSD1306_WIDTH            132
#endif
/* SSD1306 LCD height in pixels */
#ifndef SSD1306_HEIGHT
#define SSD1306_HEIGHT           32
#endif

//  @brief  SSD1306 color enumeration
typedef enum {
	SSD1306_COLOR_BLACK = 0x00, /*!< Black color, no pixel */
	SSD1306_COLOR_WHITE = 0x01  /*!< Pixel is set. Color depends on LCD */
} SSD1306_COLOR_t;




uint8_t SSD1306_Init(void);

//   Updates buffer from internal RAM to LCD
//   This function must be called each time you do some changes to LCD, to update buffer from RAM to LCD
void SSD1306_UpdateScreen(void);


//  @brief  Toggles pixels invertion inside internal RAM
//  SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
void SSD1306_ToggleInvert(void);

 
// Fills entire LCD with desired color
// SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
void SSD1306_Fill(SSD1306_COLOR_t Color);


// Draws pixel at desired location
void SSD1306_DrawPixel(uint16_t x, uint16_t y, SSD1306_COLOR_t color);

// Sets cursor pointer to desired location for strings
// param  x: X location. This parameter can be a value between 0 and SSD1306_WIDTH - 1
// param  y: Y location. This parameter can be a value between 0 and SSD1306_HEIGHT - 1
void SSD1306_GotoXY(uint16_t x, uint16_t y);

// * @brief  Puts character to internal RAM
// param  ch: Character to be written
// param  *Font: Pointer to @ref FontDef_t structure with used font // param  color: Color used for drawing. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
// retval Character written
char SSD1306_Putc(char ch, FontDef_t* Font, SSD1306_COLOR_t color);

// brief  Puts string to internal RAM
// param  *str: String to be written
// param  *Font: Pointer to @ref FontDef_t structure with used font // param  color: Color used for drawing. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
// retval Zero on success or character value when function failed
char SSD1306_Puts(uint8_t* str, FontDef_t* Font, SSD1306_COLOR_t color);

/**
 * @brief  Draws line on LCD
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x0: Line X start point. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y0: Line Y start point. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  x1: Line X end point. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y1: Line Y end point. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
void SSD1306_DrawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, SSD1306_COLOR_t c);

/**
 * @brief  Draws rectangle on LCD
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x: Top left X start point. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y: Top left Y start point. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  w: Rectangle width in units of pixels
 * @param  h: Rectangle height in units of pixels
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
void SSD1306_DrawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t c);

/**
 * @brief  Draws filled rectangle on LCD
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x: Top left X start point. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y: Top left Y start point. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  w: Rectangle width in units of pixels
 * @param  h: Rectangle height in units of pixels
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
void SSD1306_DrawFilledRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, SSD1306_COLOR_t c);

/**
 * @brief  Draws triangle on LCD
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x1: First coordinate X location. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y1: First coordinate Y location. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  x2: Second coordinate X location. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y2: Second coordinate Y location. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  x3: Third coordinate X location. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y3: Third coordinate Y location. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
void SSD1306_DrawTriangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t x3, uint16_t y3, SSD1306_COLOR_t color);

/**
 * @brief  Draws circle to STM buffer
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x: X location for center of circle. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y: Y location for center of circle. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  r: Circle radius in units of pixels
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
void SSD1306_DrawCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t c);

/**
 * @brief  Draws filled circle to STM buffer
 * @note   @ref SSD1306_UpdateScreen() must be called after that in order to see updated LCD screen
 * @param  x: X location for center of circle. Valid input is 0 to SSD1306_WIDTH - 1
 * @param  y: Y location for center of circle. Valid input is 0 to SSD1306_HEIGHT - 1
 * @param  r: Circle radius in units of pixels
 * @param  c: Color to be used. This parameter can be a value of @ref SSD1306_COLOR_t enumeration
 * @retval None
 */
void SSD1306_DrawFilledCircle(int16_t x0, int16_t y0, int16_t r, SSD1306_COLOR_t c);

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif