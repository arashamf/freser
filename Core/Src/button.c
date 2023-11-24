
/* Includes ------------------------------------------------------------------*/
#include "button.h"

static struct KEY_MACHINE_t Key_Machine;

//----------------------------------------------------------------------------//
uint16_t scan_keys (void)
{
	static __IO uint8_t key_state = KEY_STATE_OFF; // начальное состояние кнопки - не нажата
	static __IO uint16_t key_code;
	static __IO uint16_t key_repeat_time_cnt; // счетчик времени повтора
	
	if(key_state == KEY_STATE_OFF) //если кнопка была отпущена - ожидание нажатия
	{
		if(LL_GPIO_IsInputPinSet(RIGHT_BUTTON_GPIO_Port, RIGHT_BUTTON_Pin) == 1)	//если кнопка была нажата - получение кода нажатой кнопки
		{
			key_state =  KEY_STATE_ON; //установка режима - кнопка нажата
			key_code = KEY_RIGHT;
		}
		else
		{
			if (LL_GPIO_IsInputPinSet(CENTER_BUTTON_GPIO_Port, CENTER_BUTTON_Pin) == 1)
			{
				key_state =  KEY_STATE_ON;
				key_code = KEY_CENTER;
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
						key_state =  KEY_STATE_ON;
						key_code = KEY_ENC;
					}
				}
			}
		}
	}
	
	if (key_state ==  KEY_STATE_ON)
	{
		key_state = KEY_STATE_BOUNCE; // состояние кнопки - дребезг
		key_repeat_time_cnt = KEY_BOUNCE_TIME; // счетчик времени дребезга - устанавливаем
	}
	
	if(key_state == KEY_STATE_BOUNCE) //ожидание окончания интервала дребезга
	{
		if(key_repeat_time_cnt > 0)
			{key_repeat_time_cnt--;} //уменьшаем счетчик времени дребезга
		
		else //дребезг кончился
		{
			key_state = KEY_STATE_AUTOREPEAT; //установка режима автоповтора
			key_repeat_time_cnt = KEY_AUTOREPEAT_TIME; //установка счетчика времени автоповтора 
		}
	}
	
	if (key_state == KEY_STATE_AUTOREPEAT) //если режим автоповтора
	{
		if(key_repeat_time_cnt > 0)
			{key_repeat_time_cnt--;} // уменьшаем счетчик автоповтора
		
		else
		{
			if(((LL_GPIO_IsInputPinSet(RIGHT_BUTTON_GPIO_Port, RIGHT_BUTTON_Pin)) || 
				(LL_GPIO_IsInputPinSet(CENTER_BUTTON_GPIO_Port, CENTER_BUTTON_Pin)) || 
				(LL_GPIO_IsInputPinSet(LEFT_BUTTON_GPIO_Port, LEFT_BUTTON_Pin)) ||
				(LL_GPIO_IsInputPinSet(ENCODER_BUTTON_GPIO_Port, ENCODER_BUTTON_Pin))) == 0)	 // если кнопка отпущена
			{
				key_state = KEY_STATE_OFF; //возврат в начальное состояние ожидания нажатия кнопки
				return key_code; //возврата номера кнопки
			}			
			else //если кнопка продолжает удерживаться
			{
				key_state = KEY_STATE_AUTOREPEAT;
				key_repeat_time_cnt = KEY_AUTOREPEAT_TIME; // установка нового счетчика автоповтора
			} 
		}
	}
	return NO_KEY;
}

//----------------------------------------------------------------------------//
