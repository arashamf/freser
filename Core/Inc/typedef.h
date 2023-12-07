
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
	uint16_t set_degree; 
	int8_t set_minute; 
	int8_t set_second;
	uint16_t shaft_degree; 
	int8_t shaft_minute; 
	int8_t shaft_second;
	uint32_t StepAngleInSec;	
	uint32_t ShaftAngleInSec;
} angular_data_t;

typedef struct 
{
	int32_t prevCounter_SetAngle;
	int32_t currCounter_SetAngle;
	int32_t prevCounter_ShaftRotation;
	int32_t currCounter_ShaftRotation;	
	int8_t flag_DirShaftRotation;
} encoder_data_t;

// Defines ----------------------------------------------------------------------//

// Private variables -----------------------------------------------------------//


#ifdef __cplusplus
}
#endif

#endif 

