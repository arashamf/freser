
#ifndef __BUTTON_H__
#define __BUTTON_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

// Exported types -------------------------------------------------------------------//
	typedef enum 
	{
		NO_KEY = 0,					// код кнопки, состоит из кода сканирования и считанных данных: 0 - не нажата
		KEY_LEFT = 0x01,
		KEY_CENTER = 0x02, 
		KEY_RIGHT = 0x04, 
		KEY_ENC = 0x08,
	} KEY_CODE_t; 					//код нажатой кнопки

	//----------------------------------------------------------------------------------//
	typedef enum 
	{
		KEY_STATE_OFF = 0,				//статус - кнопка не нажата
		KEY_STATE_ON,							//статус - кнопка нажата
		KEY_STATE_BOUNCE, 				//статус -  дребезг кнопки
		KEY_STATE_AUTOREPEAT 			//статус - режим автоповтора
	} KEY_STATE_t; 							//статус сканирования клавиатуры

	struct KEY_MACHINE_t
	{
		KEY_CODE_t 		key_code;
		KEY_STATE_t 	key_state;
	} ;

//Private defines ------------------------------------------------------------------//
#define KEY_BOUNCE_TIME 100 				// время дребезга в мс
#define KEY_AUTOREPEAT_TIME 100 	// время автоповтора в мс

/* Prototypes ------------------------------------------------------------------*/
uint16_t scan_keys (void);

#ifdef __cplusplus
}
#endif

#endif /* __BUTTON_H__ */

