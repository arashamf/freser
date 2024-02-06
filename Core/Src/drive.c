
// Includes ------------------------------------------------------------------//
#include "drive.h"
#include "gpio.h"
#include "tim.h"
#include "typedef.h"
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
void step_angle (uint8_t dir, uint32_t need_step)
{
	uint32_t quant = 0;
	DIR_DRIVE (dir);
	delay_us (6);	
	if ((quant = (uint32_t)need_step/STEP_DIV) > 0) //количество шагов (1,8 гр.)
	{
		for (uint32_t count = 0; count < quant; count++) //
		{
			DRIVE_ENABLE(ON);
			delay_us (5);
			rotate_step (STEP_DIV); //поворот на один шаг (1,8 гр., 100 микрошагов)
			DRIVE_ENABLE(OFF);
			delay_us (1500); //задержка 1500 мкс
		}
	}		
	if ((quant = need_step%STEP_DIV) > 0) //остаток - микрошаги (<1,8 гр)
	{
		DRIVE_ENABLE(ON);
		delay_us (5);
		rotate_step (quant);
		DRIVE_ENABLE(OFF);
		delay_us (5);
	}
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
				if (HandleAng->StepAngleInSec < SECOND_PER_MINUTE)
				{ HandleAng->StepAngleInSec = SECOND_PER_MINUTE; }
				else
				{
					if (HandleAng->StepAngleInSec > (CIRCLE_IN_SEC-1))
					{ HandleAng->StepAngleInSec = (CIRCLE_IN_SEC-1); }
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
void enc_shaft_rotation (angular_data_t * HandleAng, encoder_data_t * HandleEncData) 
{
	uint32_t need_step = 0; //количество вычисленных микрошагов(импульсов)
	int32_t currCounter=0;
	int32_t delta = 0;
	
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
					if (HandleEncData->flag_DirShaftRotation == 0)
					{
						while (delta > 0) //пока дельта больше нуля
						{
							need_step = calc_steps_mode1 (HandleAng, step_unit);
							step_angle (FORWARD, need_step); //поворот по часовой стрелке
							delta--;  //уменьшение дельты
						}											
						HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec + HandleAng->StepAngleInSec;
						if (HandleAng->ShaftAngleInSec > (CIRCLE_IN_SEC-1)) //если угол положения вала 360 гр. или больше
						{	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec - CIRCLE_IN_SEC;	} 
							
						GetAngleShaft_from_Seconds(HandleAng); //конвертация текущего угла вала в секундах в формат гр/мин/сек
						
					/*	snprintf (LCD_buff, sizeof(LCD_buff), "%03d* %02d' %02d\" ->", HandleAng->shaft_degree, HandleAng->shaft_minute, HandleAng->shaft_second);	
						ssd1306_PutData (kord_X, kord_Y, LCD_buff, DISP_CLEAR);	
						snprintf (LCD_buff, sizeof(LCD_buff), "step=%d", need_step);
						ssd1306_PutData (kord_X, kord_Y+1, LCD_buff, DISP_NOT_CLEAR);	*/
					}
				}
				else
				{
					if (delta < 0) //если энкодер повернулся против часовой стрелке
					{	
						if (HandleEncData->flag_DirShaftRotation == 0) 
						{
							while (delta < 0) //пока дельта меньше нуля
							{
								need_step = calc_steps_mode1 (HandleAng, step_unit);
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
							
						/*	snprintf (LCD_buff, sizeof(LCD_buff), "%03d* %02d' %02d\" <-", HandleAng->shaft_degree, HandleAng->shaft_minute, HandleAng->shaft_second);	
							ssd1306_PutData (kord_X, kord_Y, LCD_buff, DISP_CLEAR);	
							snprintf (LCD_buff, sizeof(LCD_buff), "step=%d", need_step); 
							ssd1306_PutData (kord_X, kord_Y+1, LCD_buff, DISP_NOT_CLEAR);	*/
						}
					}
				}
				angle_to_EEPROMbuf (HandleAng, eeprom_tx_buffer); //перенос данных угла в буффер
				EEPROM_WriteBytes (EEPROM_MEMORY_PAGE, eeprom_tx_buffer, (EEPROM_NUMBER_BYTES-6)); //запись буффера с данными угла поворота в памяти					
				default_screen_mode1 (HandleAng);
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
	float error;
	need_step = steps_for_back_to_zero (HandleAng->ShaftAngleInSec, step_unit);
	step_angle (BACKWARD, need_step); //поворот против часовой стрелке
	
	HandleAng->ShaftAngleInSec = 0;
	GetAngleShaft_from_Seconds(HandleAng); //конвертация текущего угла вала в секундах в формат гр/мин/сек
	angle_to_EEPROMbuf (HandleAng, eeprom_tx_buffer); //перенос данных угла в буффер
	EEPROM_WriteBytes (EEPROM_MEMORY_PAGE, eeprom_tx_buffer, 8); //запись буффера с данными угла поворота в памяти
}

//-----------------------------возврат вала в нулевую позицию по часовой стрелке-----------------------------//
void right_rotate_to_zero (angular_data_t * HandleAng) 
{
	uint32_t need_step = 0; //количество вычисленных микрошагов(импульсов)
	float error;
	need_step = steps_for_back_to_zero ((CIRCLE_IN_SEC - HandleAng->ShaftAngleInSec), step_unit);
	step_angle (FORWARD, need_step); //поворот по часовой стрелке
	
	HandleAng->ShaftAngleInSec = 0; //сброс текущего угла вала в нулевую позицию
	GetAngleShaft_from_Seconds(HandleAng); //конвертация текущего угла вала в секундах в формат гр/мин/сек
	angle_to_EEPROMbuf (HandleAng, eeprom_tx_buffer); //перенос данных угла в буффер
	EEPROM_WriteBytes (EEPROM_MEMORY_PAGE, eeprom_tx_buffer, 8); //запись буффера с данными угла поворота в памяти
}

//---------------------------------поворот вала на один зуб по часовой стрелке---------------------------------//
void right_teeth_rotation (milling_data_t * HandleMil, angular_data_t * HandleAng) 
{
	uint32_t need_step = 0; //количество вычисленных микрошагов(импульсов)
	
	if (HandleMil->remain_teeth_gear > 0)
	{
		HandleMil->remain_teeth_gear--; //уменьшение на 1 оставшегося количества зубьев
		need_step = calc_steps_milling (HandleMil, step_unit); //поворот на один угол зуба против часовой стрелке с учётом редуктора
		step_angle (FORWARD, need_step); //поворот по часовой стрелке
		
		HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec + HandleMil->AngleTeethInSec; //увеличиваем угол текущего положения вала
		if (HandleAng->ShaftAngleInSec > (CIRCLE_IN_SEC-1)) //если угол положения вала 360 гр. или больше
		{	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec - CIRCLE_IN_SEC;	} //
	}
	/*snprintf (LCD_buff, sizeof(LCD_buff), "set=%03d@ rem=%03d@", HandleMil->teeth_gear_numbers, HandleMil->remain_teeth_gear);
	ssd1306_PutData (kord_X, kord_Y, LCD_buff, DISP_CLEAR);	
	snprintf (LCD_buff, sizeof(LCD_buff), "%03u %u ->", need_step , HandleMil->AngleTeethInSec);
	ssd1306_PutData (kord_X, kord_Y+1, LCD_buff, DISP_NOT_CLEAR);*/
}

//-------------------------------поворот вала на один зуб против часовой стрелке-------------------------------//
void left_teeth_rotation (milling_data_t * HandleMil, angular_data_t * HandleAng) 
{
	uint32_t need_step = 0; //количество вычисленных микрошагов(импульсов)
	
	if (HandleMil->remain_teeth_gear > 0)
	{
		HandleMil->remain_teeth_gear--; //уменьшение на 1 оставшегося количества зубьев
		need_step = calc_steps_milling (HandleMil, step_unit); //поворот на один угол зуба против часовой стрелке с учётом редуктора
		step_angle (BACKWARD, need_step); //поворот против часовой стрелке
		
		if (HandleAng->ShaftAngleInSec >= HandleMil->AngleTeethInSec) //если угол положения вала больше угла шага
		{	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec - HandleMil->AngleTeethInSec; } //уменьшаем угол текущего положения вала
		else 
		{
			if (HandleAng->ShaftAngleInSec < HandleMil->AngleTeethInSec) //если угол положения вала меньше угла шага
			{	HandleAng->ShaftAngleInSec = CIRCLE_IN_SEC - (HandleMil->AngleTeethInSec - HandleAng->ShaftAngleInSec); }	//перевод в формат 359гр. ххмин.
		}
	}
/*	snprintf (LCD_buff, sizeof(LCD_buff), "set=%03d@ rem=%03d@", HandleMil->teeth_gear_numbers, HandleMil->remain_teeth_gear);
	ssd1306_PutData (kord_X, kord_Y, LCD_buff, DISP_CLEAR);
	
	snprintf (LCD_buff, sizeof(LCD_buff), "%03u %u<-", need_step , HandleMil->AngleTeethInSec);
	ssd1306_PutData (kord_X, kord_Y+1, LCD_buff, DISP_NOT_CLEAR);*/
}

//---------------------------------------------------------------------------------------------------//