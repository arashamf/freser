
#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__

#ifdef __cplusplus
extern "C" {
#endif

// Includes ----------------------------------------------------------------------//
#include "main.h"

// Exported types ---------------------------------------------------------------//
typedef struct 
{
	uint16_t degree; 
	uint8_t minute; 
	uint8_t second;
	uint32_t AngleInSecond;	
} angular_data_t;

// Defines ----------------------------------------------------------------------//

#ifdef __cplusplus
}
#endif

#endif 

