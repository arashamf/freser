
#ifndef FONTS_H
#define FONTS_H 

#ifdef __cplusplus
extern C {
#endif

#include "main.h"    

// Exported types ---------------------------------------------------------------//
typedef struct 
{
	uint8_t FontWidth;    //!< Font width in pixels 
	uint8_t FontHeight;   //!< Font height in pixels 
	const uint16_t *data; //!< Pointer to data font data array 
} FontDef_t;

typedef struct 
{
	uint16_t Length;      //!< String length in units of pixels 
	uint16_t Height;      //!< String height in units of pixels 
} FONTS_SIZE_t;


extern FontDef_t	 	Font_7x10; //7 x 10 pixels font size structure 
extern FontDef_t 		Font_11x18; //11 x 18 pixels font size structure 
extern FontDef_t 		Font_16x26; //16 x 26 pixels font size structure 

/* Functions -----------------------------------------------------------------*/
char* FONTS_GetStringSize(char* str, FONTS_SIZE_t* SizeStruct, FontDef_t* Font);
#ifdef __cplusplus /* C++ detection */
}
#endif

#endif