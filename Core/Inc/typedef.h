
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
	//сохранение шага хода вала в формате гр/мин/сек/
	uint16_t 	set_degree; 			//часть установленного шага вала в градусах
	int8_t 		set_minute; 			//часть установленного шага вала в минутах
	int8_t 		set_second; 			 //часть установленного шага вала в секундах
	//текущее положение вала в формате гр/мин/сек/
	uint16_t 	shaft_degree; 		//часть текущего положения вала в градусах
	int8_t 		shaft_minute; 		//часть текущего положения  вала в минутах
	int8_t 		shaft_second; 		//часть текущего положения  вала в секундах
	
	uint32_t 	StepAngleInSec;				//полный установленный шаг вала в секундах
	uint32_t 	ShaftAngleInSec; 			//полное текущее положение вала в секундах
	uint32_t 	SetShaftAngleInSec; 	//установленное положение вала в секундах
	float mode1_error;
} angular_data_t;

typedef struct 
{
	int32_t 	prevCounter_SetAngle; 			//сохранённое показание энкодера
	int32_t 	currCounter_SetAngle; 			//текущее показание энкодера
	int32_t 	prevCounter_ShaftRotation;  //сохранённое показание энкодера
	int32_t 	currCounter_ShaftRotation;	//текущее показание энкодера
	int32_t delta;
//	int8_t 		flag_DirShaftRotation;
} encoder_data_t;

//------------------------------------------------------------------------------------//
typedef struct 
{
	uint8_t 	teeth_gear_numbers; //установленное количество зубьев
	uint8_t 	remain_teeth_gear; //оставшееся количество зубьев
	uint16_t 	step_shaft_degree; //часть угла поворота вала в режиме фрезеровки в градусах
	int8_t 		step_shaft_minute; //часть угла поворота вала в режиме фрезеровки в минутах
	int8_t 		step_shaft_second; //часть угла поворота вала в режиме фрезеровки в секундах
	uint32_t 	AngleTeethInSec; //угол поворота в режиме фрезеровки в секундах
	float milling_error;
} milling_data_t;

//----------------------------------------------------------------------------------//
typedef enum 
{
	MODE_DEFAULT 				= 0,				//режим - по умолчанию
	MODE_MILLING										//режим - фрезеровка
} MACHINE_MODE_t; 	

//----------------------------------------------------------------------------------//
typedef union
{
	struct
	{
		uint8_t tool_mode			: 1;
		uint8_t left_flag			: 1;
		uint8_t right_flag		: 1;
		uint8_t raw						:	5;
	};
	uint8_t flag;
}STATUS_FLAG_t;

//код нажатой кнопки----------------------------------------------------------------//
typedef enum 
{
	NO_KEY 						= 	0x00,			//кнопка не нажата	
	KEY_LEFT 					= 	0x01,			//левая кнопка
	KEY_CENTER_SHORT 	= 	0x02,			//короткое нажатие центральной кнопки
	KEY_CENTER_LONG 	= 	0x03,			//длинное нажатие центральной кнопки
	KEY_RIGHT					= 	0x04, 		//правая кнопка
	KEY_ENC_SHORT			= 	0x05,			//короткое нажатие кнопки энкодера
	KEY_ENC_LONG			=		0x06,			//длинное нажатие кнопки энкодера
	KEY_MODE_SHORT		=		0x07,			//короткое нажатие выбора режима
	KEY_MODE_LONG			=		0x08,			//короткое нажатие выбора режима
} KEY_CODE_t; 					

//-----------------------------------------------------------------------------------//
typedef enum 
{
	KEY_STATE_OFF 				= 0	,			//режим - кнопка не нажата
	KEY_STATE_ON							,				//режим - кнопка нажата
	KEY_STATE_BOUNCE					, 			//режим -  дребезг кнопки
	KEY_STATE_AUTOREPEAT			,	 			//режим - режим ожидания (автоповтора) отжатия кнопки
	KEY_STATE_WAIT_TURNOFF
} KEY_STATE_t; 									//статус сканирования клавиатуры

//-----------------------------------------------------------------------------------//
struct KEY_MACHINE_t
{
	KEY_CODE_t 		key_code;
	KEY_STATE_t 	key_state;
};

// Defines ----------------------------------------------------------------------//
#define   REDUCER 					40 //делитель редуктора
#define 	STEPS_IN_REV			20000 	//количество микрошагов в одном полном обороте (360 гр) с учётом делителя драйвера
#define 	CIRCLE_IN_STEP		200		 //количество шагов (1,8гр) в одном полном обороте (360 гр)
#define 	STEP_DIV 					(STEPS_IN_REV/CIRCLE_IN_STEP)		//количество микрошагов (100) в одном шаге двигателя (1,8гр)
#define 	STEP_TOOL					(STEPS_IN_REV*REDUCER) //количество микрошагов в одном полном обороте (360 гр) с учётом делителя драйвера и редуктора 

#define 	STEP18_IN_SEC			6480 //количество секунд в одном шаге двигателя (1,8гр)
#define 	CIRCLE_IN_SEC			(STEP18_IN_SEC*CIRCLE_IN_STEP)	//количество секунд в одном полном обороте двигателя (360 гр)
#define 	SECOND_PER_MINUTE 		60
#define 	SECOND_PER_DEGREE 		3600
// Private variables -----------------------------------------------------------//

#ifdef __cplusplus
}
#endif

#endif 

