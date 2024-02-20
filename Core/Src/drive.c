
// Includes ------------------------------------------------------------------//
#include "drive.h"
#include "gpio.h"
#include "tim.h"
#include "typedef.h"
#include "button.h"
#include "ssd1306.h"
#include "eeprom.h"
#include "angle_calc.h"
#include "stanok_math.h"

// Functions -----------------------------------------------------------------//
void rotate_step (uint8_t );

// Variables -----------------------------------------------------------------//
__IO float step_unit = ((float)STEP18_IN_SEC/(float)STEP_DIV); //количество секунд в одном микрошаге(1,8гр/100=6480/100=64,8)

//---------------------------------------поворот вала на один микрошаг---------------------------------------//
void rotate_step (uint8_t micro_step)
{
	for (uint8_t count = 0; count < micro_step; count++) //количество микрошагов (импульсов)
	{
			STEP(ON);
			delay_us (4); //4 мкс
			STEP(OFF);
			delay_us (4); //4 мкс
	}
}

//--------------------------------------поворот вала на произвольный угол--------------------------------------//
uint32_t step_angle (uint8_t dir, uint32_t need_step)
{
	uint32_t receive_step = 0; //количество фактически пройденных микрошагов (импульсов)
	uint32_t quant = 0; //количество целых шагов (1,8 гр.)
	__IO uint16_t key_code = NO_KEY;
	
	DIR_DRIVE (dir); //направление вращения
	delay_us (6);	
	
	if ((quant = (uint32_t)need_step/STEP_DIV) > 0) //количество шагов (1,8 гр.)
	{
		for (uint32_t count = 0; count < quant; count++) //проход целых шагов (1,8 гр.)
		{
			DRIVE_ENABLE(ON);
			delay_us (5);
			rotate_step (STEP_DIV); //поворот на один шаг (1,8 гр., 100 микрошагов)
			DRIVE_ENABLE(OFF);
			receive_step += STEP_DIV; //увеличение пройденного угла на 1 целый шаг
			if ((key_code = scan_keys()) == KEY_MODE_SHORT)
			{	
				return receive_step;	
			}		
			delay_us (1500); //задержка 1500 мкс
		}
	}		
	if ((quant = need_step%STEP_DIV) > 0) //остаток - микрошаги (<1,8 гр)
	{
		DRIVE_ENABLE(ON);
		delay_us (5);
		rotate_step (quant);
		DRIVE_ENABLE(OFF);
		receive_step += quant;
	}
	return receive_step;
}

//----------------------------------------------------------------------------------------------------//
void step_by_step (angular_data_t * HandleAng, uint32_t need_step, uint8_t dir) 
{
	uint32_t PassedAngleInSec = 0;
	uint32_t receive_step = 0; //количество фактически пройденных микрошагов(импульсов)
	while (need_step > 0) //пока дельта больше нуля
	{
		receive_step += step_angle (dir, need_step); //поворот по часовой стрелке
		need_step -= receive_step;  
	}						
	PassedAngleInSec = calc_passed_angle (receive_step, step_unit);
	if (dir == FORWARD) //если направление вращения по часовой стрелке
	{
		HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec +	PassedAngleInSec;
		if (HandleAng->ShaftAngleInSec > (CIRCLE_IN_SEC-1)) //если угол положения вала 360 гр. или больше
		{	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec - CIRCLE_IN_SEC;	} 							
	}
	else
	{
		if (dir == BACKWARD) //если направление вращения против часовой стрелке
		{
			if (HandleAng->ShaftAngleInSec >= PassedAngleInSec) //если угол положения вала больше пройдённого угла 
			{	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec - PassedAngleInSec; }
			else 
			{
				if (HandleAng->ShaftAngleInSec < PassedAngleInSec) //если угол положения вала меньше угла шага
				{	HandleAng->ShaftAngleInSec = (CIRCLE_IN_SEC - (PassedAngleInSec - HandleAng->ShaftAngleInSec)); }	//перевод в формат 359гр. ххмин.
			}			
		}
	}			
	GetAngleShaft_from_Seconds(HandleAng); //конвертация текущего угла вала в секундах в формат гр/мин/сек
	angle_to_EEPROMbuf (HandleAng, eeprom_tx_buffer); //перенос данных угла в буффер
	EEPROM_WriteBytes (EEPROM_MEMORY_PAGE, eeprom_tx_buffer, (EEPROM_NUMBER_BYTES-6)); //запись буффера с данными угла поворота в памяти					
	default_screen_mode1 (HandleAng); //главное меню режима 1 
}

//------------------------обработка показаний энкодера в режиме установки шага хода вала------------------------//
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
				HandleAng->StepAngleInSec += (delta*SECOND_PER_MINUTE);
				if (HandleAng->StepAngleInSec == 0)
				{ HandleAng->StepAngleInSec = (CIRCLE_IN_SEC); }
				else
				{
					if (HandleAng->StepAngleInSec > (CIRCLE_IN_SEC)) //если установленный угол больше 359 гр 59 минут
					{ HandleAng->StepAngleInSec = SECOND_PER_MINUTE; }
				}
				GetSetAngle_from_Seconds (HandleAng); //перевод угла шага хода вала из секунд в формат гр/мин/с
				setangle_mode_screen (HandleAng); //вывод информации на дисплей
			}
		}
	}				
}

//----------------------обработка показаний энкодера в режиме устаноки количества зубьев----------------------//
void set_teeth_gear (milling_data_t * HandleMil, encoder_data_t * HandleEncData) 
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
				HandleMil->teeth_gear_numbers += delta;
				if (HandleMil->teeth_gear_numbers > 0xFE) //количество зубьев не больше 254
				{	HandleMil->teeth_gear_numbers = 0x02;	}	//сброс на минимальное значение
				else
				{
					if (HandleMil->teeth_gear_numbers < 0x02) //и не меньше 2
					{	HandleMil->teeth_gear_numbers = 0xFE;	} //сброс на максимальное значение
				}	
				setteeth_mode_screen (HandleMil); //заставка дисплея - режим ввода настроечных данных фрезеровки
			}
		}
	}
}

//---------------------------обработка показаний энкодера в режиме управлением вала----------------------------//
/*void enc_shaft_rotation (angular_data_t * HandleAng, encoder_data_t * HandleEncData) 
{
	uint32_t need_step = 0; //количество вычисленных микрошагов(импульсов)
	uint32_t receive_step = 0; //количество фактически пройденных микрошагов(импульсов)
	int32_t currCounter=0; //текущее показание энкодера
	int32_t delta = 0; //изменение положения энкодера
	uint32_t PassedAngleInSec = 0;
	
	currCounter= LL_TIM_GetCounter(TIM3); //текущее показание энкодера
	HandleEncData->currCounter_ShaftRotation= (32767 - ((currCounter-1) & 0xFFFF))/2; //деление на 2, счёт щелчков (щелчок = 2 импульса)
	
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
					//if (HandleEncData->flag_DirShaftRotation == 0)
					{
						while (delta > 0) //пока дельта больше нуля
						{
							need_step = calc_steps_mode1 (HandleAng, step_unit);
							receive_step += step_angle (FORWARD, need_step); //поворот по часовой стрелке
							delta--;  //уменьшение дельты
						}						
						PassedAngleInSec = calc_passed_angle (receive_step, step_unit);
						HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec +	PassedAngleInSec;
						if (HandleAng->ShaftAngleInSec > (CIRCLE_IN_SEC-1)) //если угол положения вала 360 гр. или больше
						{	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec - CIRCLE_IN_SEC;	} 							
						GetAngleShaft_from_Seconds(HandleAng); //конвертация текущего угла вала в секундах в формат гр/мин/сек
					}
				}
				else
				{
					if (delta < 0) //если энкодер повернулся против часовой стрелке
					{	
					//	if (HandleEncData->flag_DirShaftRotation == 0) 
						{
							while (delta < 0) //пока дельта меньше нуля
							{
								need_step = calc_steps_mode1 (HandleAng, step_unit); //расчёт количества необходимых микрошагов
								receive_step += step_angle (BACKWARD, need_step); //поворот против часовой стрелки
								delta++; //увеличение дельты
							}	
							PassedAngleInSec = calc_passed_angle (receive_step, step_unit);
							if (HandleAng->ShaftAngleInSec >= PassedAngleInSec) //если угол положения вала больше пройдённого угла 
							{	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec - PassedAngleInSec; }
							else 
							{
								if (HandleAng->ShaftAngleInSec < PassedAngleInSec) //если угол положения вала меньше угла шага
								{	HandleAng->ShaftAngleInSec = (CIRCLE_IN_SEC - (PassedAngleInSec - HandleAng->ShaftAngleInSec)); }	//перевод в формат 359гр. ххмин.
							}						
							GetAngleShaft_from_Seconds(HandleAng); //конвертация текущего угла вала в секундах в формат гр/мин/сек
						}
					}
				}
				angle_to_EEPROMbuf (HandleAng, eeprom_tx_buffer); //перенос данных угла в буффер
				EEPROM_WriteBytes (EEPROM_MEMORY_PAGE, eeprom_tx_buffer, (EEPROM_NUMBER_BYTES-6)); //запись буффера с данными угла поворота в памяти					
				default_screen_mode1 (HandleAng); //главное меню режима 1 
			}
			else
			{	delta = 0;	}
		}
	}
}*/

void enc_shaft_rotation (angular_data_t * HandleAng, encoder_data_t * HandleEncData, uint32_t need_step, uint8_t dir) 
{
	int32_t currCounter=0; //текущее показание энкодера
	int32_t delta = 0; //изменение положения энкодера
	uint32_t PassedAngleInSec = 0;
	
	currCounter= LL_TIM_GetCounter(TIM3); //текущее показание энкодера
	HandleEncData->currCounter_ShaftRotation= (32767 - ((currCounter-1) & 0xFFFF))/2; //деление на 2, счёт щелчков (щелчок = 2 импульса)
	
	if(HandleEncData->currCounter_ShaftRotation != HandleEncData->prevCounter_ShaftRotation) //если показания энкодера изменились
	{
		delta = (HandleEncData->currCounter_ShaftRotation - HandleEncData->prevCounter_ShaftRotation); //разница между текущим и предыдущим показанием энкодера
    HandleEncData->prevCounter_ShaftRotation = HandleEncData->currCounter_ShaftRotation; //сохранение текущего показания энкодера    
		if((delta > -20) && (delta < 20)) // защита от дребезга контактов и переполнения счетчика 
		{
			if (delta != 0) //если изменилось положение энкодера
			{ 
				if (delta > 0) //если энкодер повернулся по часовой стрелке
				{
					while (delta > 0) //пока дельта больше нуля
					{
						need_step = calc_steps_mode1 (HandleAng, step_unit);
						delta--;  //уменьшение дельты
					}						
				}
				else
				{
					if (delta < 0) //если энкодер повернулся против часовой стрелке
					{	
						while (delta < 0) //пока дельта меньше нуля
						{
							need_step = calc_steps_mode1 (HandleAng, step_unit); //расчёт количества необходимых микрошагов
							delta++; //увеличение дельты
						}	
					}
				}
			}
			else
			{	delta = 0;	}
		}
	}
}

//------------------------------------------сброс показаний энкодера------------------------------------------//
void encoder_reset (encoder_data_t * HandleEncData) 
{
	int32_t encCounter = 0; //переменная для хранения данных энкодера
	encCounter = LL_TIM_GetCounter(TIM3); //сохранение текущего показания энкодера
	HandleEncData->currCounter_SetAngle = (32767 - ((encCounter-1) & 0xFFFF))/2; //преобразование полученного показания энкодера в формат от -10 до 10
	HandleEncData->prevCounter_SetAngle = HandleEncData->currCounter_SetAngle; //сохранение преобразованного текущего показания энкодера в структуру установки шага поворота
	HandleEncData->prevCounter_ShaftRotation = HandleEncData->currCounter_SetAngle; //сохранение преобразованного текущего показания энкодера в структуру данных положения вала
}

//---------------------------------------------------------------------------------------------------//
void right_shaft_rotation (angular_data_t * HandleAng) 
{
	uint32_t need_step = 0; //количество вычисленных микрошагов(импульсов)
	SetAngle_in_Seconds (HandleAng); //перевод угловых данных шага в секунды
	need_step = calc_steps_mode1 (HandleAng, step_unit);
	step_angle (FORWARD, need_step); //поворот по часовой стрелке
	
	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec + HandleAng->StepAngleInSec;
	if (HandleAng->ShaftAngleInSec > (CIRCLE_IN_SEC-1)) //если угол положения вала 360 гр. или больше
	{	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec - CIRCLE_IN_SEC;	} 
	
	GetAngleShaft_from_Seconds(HandleAng); //конвертация текущего угла вала в секундах в формат гр/мин/сек
	angle_to_EEPROMbuf (HandleAng, eeprom_tx_buffer); //перенос данных угла в буффер
	EEPROM_WriteBytes (EEPROM_MEMORY_PAGE, eeprom_tx_buffer, 8); //запись буффера с данными угла поворота в памяти					
}

//---------------------------------------------------------------------------------------------------//
void left_shaft_rotation (angular_data_t * HandleAng) 
{
	uint32_t need_step = 0; //количество вычисленных микрошагов(импульсов)
	SetAngle_in_Seconds (HandleAng); //перевод угловых данных шага в секунды
	need_step = calc_steps_mode1 (HandleAng, step_unit);
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
	EEPROM_WriteBytes (EEPROM_MEMORY_PAGE, eeprom_tx_buffer, 8); //запись буффера с данными угла поворота в памяти					
}

//---------------------------------------------------------------------------------------------------//
void one_full_turn (void) 
{
	step_angle (FORWARD, (STEPS_IN_REV*REDUCER));  //полный оборот на 360гр с учётом делителя редуктора (360гр*40)
}

//----------------------------возврат вала в нулевую позицию против часовой стрелке----------------------------//
void left_rotate_to_zero (angular_data_t * HandleAng) 
{
	uint32_t need_step = 0; //количество вычисленных микрошагов(импульсов)
	uint32_t receive_step = 0; //количество фактически пройденных микрошагов(импульсов)
	uint32_t PassedAngleInSec = 0;
	
	need_step = steps_for_back_to_zero (HandleAng->ShaftAngleInSec, step_unit);
	receive_step = step_angle (BACKWARD, need_step); //поворот против часовой стрелке
	PassedAngleInSec = calc_passed_angle (receive_step, step_unit);
	
	//HandleAng->ShaftAngleInSec = 0;
	HandleAng->ShaftAngleInSec -= PassedAngleInSec;
	GetAngleShaft_from_Seconds(HandleAng); //конвертация текущего угла вала в секундах в формат гр/мин/сек
	angle_to_EEPROMbuf (HandleAng, eeprom_tx_buffer); //перенос данных угла в буффер
	EEPROM_WriteBytes (EEPROM_MEMORY_PAGE, eeprom_tx_buffer, 8); //запись буффера с данными угла поворота в памяти
}

//-----------------------------возврат вала в нулевую позицию по часовой стрелке-----------------------------//
void right_rotate_to_zero (angular_data_t * HandleAng) 
{
	uint32_t need_step = 0; //количество вычисленных микрошагов(импульсов)
	uint32_t receive_step = 0; //количество фактически пройденных микрошагов(импульсов)
	uint32_t PassedAngleInSec = 0;
	
	need_step = steps_for_back_to_zero ((CIRCLE_IN_SEC - HandleAng->ShaftAngleInSec), step_unit);
	receive_step = step_angle (FORWARD, need_step); //поворот по часовой стрелке
	PassedAngleInSec = calc_passed_angle (receive_step, step_unit);
	
	//HandleAng->ShaftAngleInSec = 0; //сброс текущего угла вала в нулевую позицию
	HandleAng->ShaftAngleInSec += PassedAngleInSec;
	GetAngleShaft_from_Seconds(HandleAng); //конвертация текущего угла вала в секундах в формат гр/мин/сек
	angle_to_EEPROMbuf (HandleAng, eeprom_tx_buffer); //перенос данных угла в буффер
	EEPROM_WriteBytes (EEPROM_MEMORY_PAGE, eeprom_tx_buffer, 8); //запись буффера с данными угла поворота в памяти
}

//---------------------------------поворот вала на один зуб по часовой стрелке---------------------------------//
void right_teeth_rotation (milling_data_t * HandleMil, angular_data_t * HandleAng) 
{
	uint32_t need_step = 0; //количество вычисленных микрошагов(импульсов)
	uint32_t receive_step = 0; //количество фактически пройденных микрошагов(импульсов)
	
	if (HandleMil->remain_teeth_gear > 0)
	{
		HandleMil->remain_teeth_gear--; //уменьшение на 1 оставшегося количества зубьев
		need_step = calc_steps_milling (HandleMil, step_unit); //поворот на один угол зуба против часовой стрелке с учётом редуктора
		receive_step = step_angle (FORWARD, need_step); //поворот по часовой стрелке
		
		HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec + HandleMil->AngleTeethInSec; //увеличиваем угол текущего положения вала
		if (HandleAng->ShaftAngleInSec > (CIRCLE_IN_SEC-1)) //если угол положения вала 360 гр. или больше
		{	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec - CIRCLE_IN_SEC;	} //
	}
}

//-------------------------------поворот вала на один зуб против часовой стрелке-------------------------------//
void left_teeth_rotation (milling_data_t * HandleMil, angular_data_t * HandleAng) 
{
	uint32_t need_step = 0; //количество вычисленных микрошагов(импульсов)
	uint32_t receive_step = 0; //количество фактически пройденных микрошагов(импульсов)
	
	if (HandleMil->remain_teeth_gear > 0)
	{
		HandleMil->remain_teeth_gear--; //уменьшение на 1 оставшегося количества зубьев
		need_step = calc_steps_milling (HandleMil, step_unit); //поворот на один угол зуба против часовой стрелке с учётом редуктора
		receive_step = step_angle (BACKWARD, need_step); //поворот против часовой стрелке
		
		if (HandleAng->ShaftAngleInSec >= HandleMil->AngleTeethInSec) //если угол положения вала больше угла шага
		{	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec - HandleMil->AngleTeethInSec; } //уменьшаем угол текущего положения вала
		else 
		{
			if (HandleAng->ShaftAngleInSec < HandleMil->AngleTeethInSec) //если угол положения вала меньше угла шага
			{	HandleAng->ShaftAngleInSec = CIRCLE_IN_SEC - (HandleMil->AngleTeethInSec - HandleAng->ShaftAngleInSec); }	//перевод в формат 359гр. ххмин.
		}
	}
}

//---------------------------------------------------------------------------------------------------//