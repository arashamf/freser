
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "drive.h"
#include "gpio.h"
#include "tim.h"
#include "typedef.h"
#include "ssd1306.h"
#include "eeprom.h"
#include "angle_calc.h"

// Functions -----------------------------------------------------------------//

// Variables ------------------------------------------------------------------//
__IO uint32_t need_step = 0;
__IO float step_unit= (float)STEP18_IN_SEC/STEP_DIV;
//----------------------------------------------------------------------------------------------------//
void rotate_one_step (uint8_t need_step)
{
	for (uint8_t count = 0; count < need_step; count++)
	{
			STEP(ON);
			delay_us (5);
			STEP(OFF);
			delay_us (5);
	}
}

//----------------------------------------------------------------------------------------------------//
void step_angle (uint8_t dir, uint32_t need_step)
{
	uint16_t quant = 0;
	/*DRIVE_ENABLE(ON);
	delay_us (6);*/
	DIR_DRIVE (dir);
	delay_us (6);
	
	if ((quant = (uint16_t)need_step/STEP_DIV) > 0) //количество целых шагов по 1,8 градуса
	{
		for (uint16_t count = 0; count < quant; count++)
		{
			DRIVE_ENABLE(ON);
			delay_us (6);
			rotate_one_step (STEP_DIV); //поворот на один шаг  по 1,8 градуса
			DRIVE_ENABLE(OFF);
			delay_us (2000);
		}
	}
		
	if ((quant = need_step%STEP_DIV) > 0)
	{
		DRIVE_ENABLE(ON);
		delay_us (6);
		rotate_one_step (quant);
		DRIVE_ENABLE(OFF);
		delay_us (3);
	}
	
/*	if (dir == FORWARD)
		DIR_DRIVE (BACKWARD);
	else
		DIR_DRIVE (FORWARD);*/
}

//----------------------------------------------------------------------------------------------------//
void set_angle (angular_data_t * HandleAng, encoder_data_t * HandleEncData) 
{
	int32_t currCounter=0;
	int32_t delta = 0;
	
	currCounter = LL_TIM_GetCounter(TIM3); //текущее показание энкодера
	HandleEncData->currCounter_SetAngle = (32767 - ((currCounter-1) & 0xFFFF))/2; //деление на 2, счёт щелчков (щелчок = 2 импульса)
	
	if(HandleEncData->currCounter_SetAngle != HandleEncData->prevCounter_SetAngle) //если текущее значение энкодера на равно предыдущему
	{
		delta = (HandleEncData->currCounter_SetAngle - HandleEncData->prevCounter_SetAngle); //разница между текущим и предыдущим показанием энкодера
    HandleEncData->prevCounter_SetAngle = HandleEncData->currCounter_SetAngle; //сохранение текущего показанаия энкодера    
    if((delta > -20) && (delta < 20)) // защита от дребезга контактов и переполнения счетчика (переполнение будет случаться очень редко)
		{
			if (delta != 0) //если изменилось положение энкодера
			{  
				snprintf (LCD_buff, sizeof(LCD_buff), "delta=%d", delta);
				ssd1306_PutData (kord_X, kord_Y, LCD_buff, DISP_CLEAR); //1 строка дисплея
				
				HandleAng->set_minute += delta; //сохранение разницы
				if (HandleAng->set_minute <= 0) //если значение минут меньше 0
				{			 
					if ((HandleAng->set_degree > 0) ) //если значение градусов больше 0
					{ 
						HandleAng->set_degree--; //уменьшаем градус на единицу
						HandleAng->set_minute = 59;
					}
					else
					{	
						if (HandleAng->set_degree == 0)
						{
							if (HandleAng->set_minute <= 0) //если значение градусов равно 0
							{	HandleAng->set_minute = 1;	} //установка 1 в значение минут
						}
					}
				}
				else
				{
					if (HandleAng->set_minute > 59) //если минут больше 59
					{
						HandleAng->set_degree++; 				//увеличение значения градусов на единицу
						if (HandleAng->set_degree > 359) 
						{	HandleAng->set_degree = 0;	}
						HandleAng->set_minute = 0;
					}	
				}				
			}
		}
		snprintf (LCD_buff, sizeof(LCD_buff), "%03d* %02d' %02d\" edit", HandleAng->set_degree, 
		HandleAng->set_minute, HandleAng->set_second);
		ssd1306_PutData (kord_X, kord_Y+1, LCD_buff, DISP_NOT_CLEAR); //2 строка дисплея
	}
}

//---------------------------------------------------------------------------------------------------//
void enc_shaft_rotation (angular_data_t * HandleAng, encoder_data_t * HandleEncData) 
{
	int32_t currCounter=0;
	currCounter= LL_TIM_GetCounter(TIM3); //текущее показание энкодера
	HandleEncData->currCounter_ShaftRotation= (32767 - ((currCounter-1) & 0xFFFF))/2; //деление на 2, счёт щелчков (щелчок = 2 импульса)
	int32_t delta = 0;
	
	if(HandleEncData->currCounter_ShaftRotation != HandleEncData->prevCounter_ShaftRotation) //если показания энкодера изменились
	{
		delta = (HandleEncData->currCounter_ShaftRotation - HandleEncData->prevCounter_ShaftRotation); //разница между текущим и предыдущим показанием энкодера
    HandleEncData->prevCounter_ShaftRotation = HandleEncData->currCounter_ShaftRotation; //сохранение текущего показания энкодера    
		if((delta > -20) && (delta < 20)) // защита от дребезга контактов и переполнения счетчика 
		{
			if (delta != 0) //если изменилось положение энкодера
			{ 
				SetAngle_in_Seconds (HandleAng); //перевод угловых данных шага в секунды
				
				if (delta > 0) //если энкодер повернулся по часовой стрелке
				{
					HandleEncData->flag_DirShaftRotation++; //увеличение значения флага достоверности
					if (HandleEncData->flag_DirShaftRotation>1) 
					{ HandleEncData->flag_DirShaftRotation = 1;	}

					if (HandleEncData->flag_DirShaftRotation > 0) //если значение флага достоверности положительное
					{
						while (delta > 0) //пока дельта больше нуля
						{
							need_step = HandleAng->StepAngleInSec/step_unit; //количество необходимых шагов
							step_angle (FORWARD, need_step); //поворот по часовой стрелке
							delta--;  //уменьшение дельты
						}						
						
						HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec + HandleAng->StepAngleInSec;
						if (HandleAng->ShaftAngleInSec > (CIRCLE_IN_SEC-1)) //если угол положения вала 360 гр. или больше
						{	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec - CIRCLE_IN_SEC;	} 
						
						GetAngleShaft_from_Seconds(HandleAng); //конвертация текущего угла вала в секундах в формат гр/мин/сек
						
						snprintf (LCD_buff, sizeof(LCD_buff), "%03d* %02d' %02d\" f", HandleAng->shaft_degree, 
						HandleAng->shaft_minute, HandleAng->shaft_second);	
						ssd1306_PutData (kord_X, kord_Y, LCD_buff, DISP_CLEAR);	
						snprintf (LCD_buff, sizeof(LCD_buff), "step=%d", need_step);
						ssd1306_PutData (kord_X, kord_Y+1, LCD_buff, DISP_NOT_CLEAR);	
					}
				}
				else
				{
					if (delta < 0) //если энкодер повернулся против часовой стрелке
					{
						HandleEncData->flag_DirShaftRotation--; //уменьшение значения флага достоверности
						if (HandleEncData->flag_DirShaftRotation<(-1))
						{ HandleEncData->flag_DirShaftRotation = -1;	}
		
						if (HandleEncData->flag_DirShaftRotation < 0) //если значение флага достоверности отрицательное
						{
							while (delta < 0) //пока дельта меньше нуля
							{
								need_step = HandleAng->StepAngleInSec/step_unit; //количество необходимых шагов
								step_angle (BACKWARD, need_step); //поворот против часовой стрелки
								delta++; //увеличение дельты
							}							
							if (HandleAng->ShaftAngleInSec >= HandleAng->StepAngleInSec) //если угол положения вала больше угла шага
							{	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec - HandleAng->StepAngleInSec; }
							else 
							{
								if (HandleAng->ShaftAngleInSec < HandleAng->StepAngleInSec) //если угол положения вала меньше угла шага
								{	HandleAng->ShaftAngleInSec = CIRCLE_IN_SEC - (HandleAng->StepAngleInSec - HandleAng->ShaftAngleInSec); }	//перевод в формат 359гр. ххмин.
							}
							GetAngleShaft_from_Seconds(HandleAng); //конвертация текущего угла вала в секундах в формат гр/мин/сек
							snprintf (LCD_buff, sizeof(LCD_buff), "%03d* %02d' %02d\" b", HandleAng->shaft_degree, 
							HandleAng->shaft_minute, HandleAng->shaft_second);	
							ssd1306_PutData (kord_X, kord_Y, LCD_buff, DISP_CLEAR);	
							snprintf (LCD_buff, sizeof(LCD_buff), "step=%d", need_step);
							ssd1306_PutData (kord_X, kord_Y+1, LCD_buff, DISP_NOT_CLEAR);	
						}
					}
				}
				angle_to_EEPROMbuf (HandleAng, eeprom_tx_buffer); //перенос данных угла в буффер
				EEPROM_WriteBytes (MEMORY_PAGE_ANGLE_ROTATION, eeprom_tx_buffer, 8); //запись буффера с данными угла поворота в памяти					
			}
			else
			{	delta = 0;	}
		}
	}
}


//---------------------------------------------------------------------------------------------------//
void rbt_shaft_rotation (angular_data_t * HandleAng) 
{
	SetAngle_in_Seconds (HandleAng); //перевод угловых данных шага в секунды
	need_step = HandleAng->StepAngleInSec/step_unit; //количество необходимых шагов
	step_angle (FORWARD, need_step); //поворот по часовой стрелке
	
	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec + HandleAng->StepAngleInSec;
	if (HandleAng->ShaftAngleInSec > (CIRCLE_IN_SEC-1)) //если угол положения вала 360 гр. или больше
	{	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec - CIRCLE_IN_SEC;	} 
	
	GetAngleShaft_from_Seconds(HandleAng); //конвертация текущего угла вала в секундах в формат гр/мин/сек
	angle_to_EEPROMbuf (HandleAng, eeprom_tx_buffer); //перенос данных угла в буффер
	EEPROM_WriteBytes (MEMORY_PAGE_ANGLE_ROTATION, eeprom_tx_buffer, 8); //запись буффера с данными угла поворота в памяти					
}

//---------------------------------------------------------------------------------------------------//
void lbt_shaft_rotation (angular_data_t * HandleAng) 
{
	SetAngle_in_Seconds (HandleAng); //перевод угловых данных шага в секунды
	need_step = HandleAng->StepAngleInSec/step_unit; //количество необходимых шагов
	step_angle (BACKWARD, need_step); //поворот против часовой стрелке
	
	if (HandleAng->ShaftAngleInSec >= HandleAng->StepAngleInSec) //если угол положения вала больше угла шага
	{	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec - HandleAng->StepAngleInSec; }
	else 
	{
		if (HandleAng->ShaftAngleInSec < HandleAng->StepAngleInSec) //если угол положения вала меньше угла шага
		{	HandleAng->ShaftAngleInSec = CIRCLE_IN_SEC - (HandleAng->StepAngleInSec - HandleAng->ShaftAngleInSec); }	//перевод в формат 359гр. ххмин.
	}
	GetAngleShaft_from_Seconds(HandleAng); //конвертация текущего угла вала в секундах в формат гр/мин/сек
	angle_to_EEPROMbuf (HandleAng, eeprom_tx_buffer); //перенос данных угла в буффер
	EEPROM_WriteBytes (MEMORY_PAGE_ANGLE_ROTATION, eeprom_tx_buffer, 8); //запись буффера с данными угла поворота в памяти					
}

//---------------------------------------------------------------------------------------------------//
void one_full_turn (void) 
{
	step_angle (FORWARD, STEPS_IN_REV);
}

//---------------------------------------------------------------------------------------------------//
void lbt_rotate_to_zero (angular_data_t * HandleAng) 
{
	need_step = ((HandleAng->ShaftAngleInSec)/step_unit);
	step_angle (BACKWARD, need_step); //поворот против часовой стрелке
	
	HandleAng->ShaftAngleInSec = 0;
	GetAngleShaft_from_Seconds(HandleAng); //конвертация текущего угла вала в секундах в формат гр/мин/сек
	angle_to_EEPROMbuf (HandleAng, eeprom_tx_buffer); //перенос данных угла в буффер
	EEPROM_WriteBytes (MEMORY_PAGE_ANGLE_ROTATION, eeprom_tx_buffer, 8); //запись буффера с данными угла поворота в памяти
}

//---------------------------------------------------------------------------------------------------//
void rbt_rotate_to_zero (angular_data_t * HandleAng) 
{
	need_step = ((CIRCLE_IN_SEC - HandleAng->ShaftAngleInSec)/step_unit);
	step_angle (FORWARD, need_step); //поворот против часовой стрелке
	
	HandleAng->ShaftAngleInSec = 0;
	GetAngleShaft_from_Seconds(HandleAng); //конвертация текущего угла вала в секундах в формат гр/мин/сек
	angle_to_EEPROMbuf (HandleAng, eeprom_tx_buffer); //перенос данных угла в буффер
	EEPROM_WriteBytes (MEMORY_PAGE_ANGLE_ROTATION, eeprom_tx_buffer, 8); //запись буффера с данными угла поворота в памяти
}

