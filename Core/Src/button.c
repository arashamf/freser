
// Includes ------------------------------------------------------------------//
#include "button.h"
#include "typedef.h"
#include "tim.h"

// Exported types -------------------------------------------------------------//
static struct KEY_MACHINE_t Key_Machine;

// Prototypes ---------------------------------------------------------------------------------------//
static uint8_t scan_buttons_GPIO (void);

//Private defines -----------------------------------------------------------------------------------//
#define KEY_BOUNCE_TIME 			50 				// время дребезга в мс
#define KEY_AUTOREPEAT_TIME 	100 			// время автоповтора в мс
#define COUNT_REPEAT_BUTTON 	5

// Private variables -------------------------------------------------------------------------------//
uint8_t count_autorepeat = 0; //подсчёт удержания кнопки

//--------------------------------------------------------------------------------------------------//
uint16_t scan_keys (void)
{
	static __IO uint8_t key_state = KEY_STATE_OFF; // начальное состояние кнопки - не нажата
	static __IO uint16_t key_code;
	//static __IO uint16_t key_repeat_time; // счетчик времени повтора
	
	if(key_state == KEY_STATE_OFF) //если кнопка была отпущена - ожидание нажатия
	{
		if(LL_GPIO_IsInputPinSet(RIGHT_BUTTON_GPIO_Port, RIGHT_BUTTON_Pin) == 1)	//если кнопка была нажата - получение кода нажатой кнопки
		{
			key_state =  KEY_STATE_ON; //переход в режим нажатия кнопки
			key_code = KEY_RIGHT;
		}
		else
		{
			if (LL_GPIO_IsInputPinSet(CENTER_BUTTON_GPIO_Port, CENTER_BUTTON_Pin) == 1)
			{
				key_state =  KEY_STATE_ON;
				key_code = KEY_CENTER_SHORT;
			}
			else
			{
				if (LL_GPIO_IsInputPinSet(LEFT_BUTTON_GPIO_Port, LEFT_BUTTON_Pin) == 1)
				{
					key_state =  KEY_STATE_ON;
					key_code = KEY_LEFT;
				}
				else
				{
					if (LL_GPIO_IsInputPinSet(ENCODER_BUTTON_GPIO_Port, ENCODER_BUTTON_Pin) == 1)
					{
						key_state = KEY_STATE_ON;
						key_code = KEY_ENC_SHORT;
					}
					else
					{
						if (LL_GPIO_IsInputPinSet(MODE_BUTTON_GPIO_Port, MODE_BUTTON_Pin) == 1)
						{
							key_state =  	KEY_STATE_ON;
							key_code 	= 	KEY_MODE_SHORT;
						}
					}
				}
			}
		}
	}
	
	if (key_state ==  KEY_STATE_ON)  //режим нажатия кнопки
	{
		repeat_time (KEY_BOUNCE_TIME); //запуск таймера ожидания окончания дребезга
		key_state = KEY_STATE_BOUNCE; // переход в режим окончания дребезга
	}
	
	if(key_state == KEY_STATE_BOUNCE) //режим окончания дребезга
	{
		if (end_bounce == SET) //если флаг окончания дребезга установлен
		{
			end_bounce = RESET;  //сброс флага
			if(scan_buttons_GPIO() == 0)	 // если кнопка отпущена (нажатие менее 50 мс это дребезг)
			{
				key_state = KEY_STATE_OFF; //переход в начальное состояние ожидания нажатия кнопки
				return NO_KEY; //возврат 0 
			}	
			else //если кнопка продолжает удерживаться
			{	
				repeat_time (KEY_AUTOREPEAT_TIME); //установка таймера ожидания отключения кнопки
				key_state = KEY_STATE_AUTOREPEAT;   //переход в режим автоповтора 
				count_autorepeat = 0;
			}
		}
	}
	
	if (key_state == KEY_STATE_AUTOREPEAT) //если активен режим автоповтора
	{
		if (end_bounce == SET) //если флаг окончания дребезга установлен (устанавливается в прерывании таймера)
		{
			end_bounce = RESET; //сброс флага
			if(scan_buttons_GPIO() == 0)	 // если кнопка была отпущена (короткое нажатие кнопки < 150 мс)
			{
				key_state = KEY_STATE_OFF; //переход в начальное состояние ожидания нажатия кнопки
				return key_code; //возврата номера кнопки
			}
			else //если кнопка продолжает удерживаться
			{			
				if (count_autorepeat < COUNT_REPEAT_BUTTON)
				{	count_autorepeat++;	}
				else //если кнопка удерживалась более 650 мс
				{	
					switch (key_code)
					{
						case KEY_CENTER_SHORT:
							key_code = KEY_CENTER_LONG;	
							break;
						
						case KEY_ENC_SHORT:
							key_code = KEY_ENC_LONG;	
							break;
						
						case KEY_MODE_SHORT:
							key_code = KEY_MODE_LONG;	
							break;	
						
						default:
							break;	
					}
				}
				repeat_time (KEY_AUTOREPEAT_TIME); //установка таймера ожидания отключения кнопки
			} 	
		}					
	}
	return NO_KEY;
}

//-------------------------------------------------------------------------------------------------//
static uint8_t scan_buttons_GPIO (void)
{
	return ((LL_GPIO_IsInputPinSet(RIGHT_BUTTON_GPIO_Port, RIGHT_BUTTON_Pin)) 		|| 
					(LL_GPIO_IsInputPinSet(CENTER_BUTTON_GPIO_Port, CENTER_BUTTON_Pin)) 	|| 
					(LL_GPIO_IsInputPinSet(LEFT_BUTTON_GPIO_Port, LEFT_BUTTON_Pin)) 			||
					(LL_GPIO_IsInputPinSet(ENCODER_BUTTON_GPIO_Port, ENCODER_BUTTON_Pin)) ||
					(LL_GPIO_IsInputPinSet(MODE_BUTTON_GPIO_Port, MODE_BUTTON_Pin))) 			; 																																		
}

