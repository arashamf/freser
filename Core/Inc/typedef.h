
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

typedef struct 
{
	uint8_t teeth_gear_numbers;
	uint8_t remain_teeth_gear;
	uint16_t step_shaft_degree; 
	int8_t step_shaft_minute; 
	int8_t step_shaft_second;
	uint32_t ShaftAngleInSec;
} milling_data_t;

	//----------------------------------------------------------------------------------//
	typedef enum 
	{
		MODE_DEFAULT 				= 0,			//режим - по умолчанию
		MODE_MILLING								//режим - фрезеровка
	} MACHINE_MODE_t; 	

// Defines ----------------------------------------------------------------------//

// Private variables -----------------------------------------------------------//


#ifdef __cplusplus
}
#endif

#endif 

