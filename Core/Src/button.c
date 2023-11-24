
/* Includes ------------------------------------------------------------------*/
#include "button.h"

static struct KEY_MACHINE_t Key_Machine;

//----------------------------------------------------------------------------//
uint16_t scan_keys (void)
{
	static __IO uint8_t key_state = KEY_STATE_OFF; // ��������� ��������� ������ - �� ������
	static __IO uint16_t key_code;
	static __IO uint16_t key_repeat_time_cnt; // ������� ������� �������
	
	if(key_state == KEY_STATE_OFF) //���� ������ ���� �������� - �������� �������
	{
		if(LL_GPIO_IsInputPinSet(RIGHT_BUTTON_GPIO_Port, RIGHT_BUTTON_Pin) == 1)	//���� ������ ���� ������ - ��������� ���� ������� ������
		{
			key_state =  KEY_STATE_ON; //��������� ������ - ������ ������
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
		key_state = KEY_STATE_BOUNCE; // ��������� ������ - �������
		key_repeat_time_cnt = KEY_BOUNCE_TIME; // ������� ������� �������� - �������������
	}
	
	if(key_state == KEY_STATE_BOUNCE) //�������� ��������� ��������� ��������
	{
		if(key_repeat_time_cnt > 0)
			{key_repeat_time_cnt--;} //��������� ������� ������� ��������
		
		else //������� ��������
		{
			key_state = KEY_STATE_AUTOREPEAT; //��������� ������ �����������
			key_repeat_time_cnt = KEY_AUTOREPEAT_TIME; //��������� �������� ������� ����������� 
		}
	}
	
	if (key_state == KEY_STATE_AUTOREPEAT) //���� ����� �����������
	{
		if(key_repeat_time_cnt > 0)
			{key_repeat_time_cnt--;} // ��������� ������� �����������
		
		else
		{
			if(((LL_GPIO_IsInputPinSet(RIGHT_BUTTON_GPIO_Port, RIGHT_BUTTON_Pin)) || 
				(LL_GPIO_IsInputPinSet(CENTER_BUTTON_GPIO_Port, CENTER_BUTTON_Pin)) || 
				(LL_GPIO_IsInputPinSet(LEFT_BUTTON_GPIO_Port, LEFT_BUTTON_Pin)) ||
				(LL_GPIO_IsInputPinSet(ENCODER_BUTTON_GPIO_Port, ENCODER_BUTTON_Pin))) == 0)	 // ���� ������ ��������
			{
				key_state = KEY_STATE_OFF; //������� � ��������� ��������� �������� ������� ������
				return key_code; //�������� ������ ������
			}			
			else //���� ������ ���������� ������������
			{
				key_state = KEY_STATE_AUTOREPEAT;
				key_repeat_time_cnt = KEY_AUTOREPEAT_TIME; // ��������� ������ �������� �����������
			} 
		}
	}
	return NO_KEY;
}

//----------------------------------------------------------------------------//
