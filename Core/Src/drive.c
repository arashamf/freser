
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

//-------------------------------------------------------------------------------------------------------------//
void milling_step (uint8_t dir, uint32_t need_step)
{
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
			delay_us (1500); //задержка 1500 мкс
		}
	}		
	if ((quant = need_step%STEP_DIV) > 0) //остаток - микрошаги (<1,8 гр)
	{
		DRIVE_ENABLE(ON);
		delay_us (5);
		rotate_step (quant);
		DRIVE_ENABLE(OFF);
	}
}
//--------------------------------------поворот вала на произвольный угол--------------------------------------//
uint32_t step_angle (uint8_t dir, uint32_t need_step,	angular_data_t * HandleAng, encoder_data_t * HandleEncData)
{
	uint32_t receive_step = 0; //количество фактически пройденных микрошагов (импульсов)
	uint32_t quant = 0; //количество целых шагов (1,8 гр.)
	__IO uint16_t key_code = NO_KEY;
	uint32_t PassedAngleInSec = 0; //фактически пройденный угол в секундах 
	
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
			if ((key_code = scan_keys()) == KEY_MODE_SHORT) //если произошло короткое нажатие кнопки выбора режима
			{	
				encoder_reset (HandleEncData); //сброс показаний энкодера
				HandleAng->SetShaftAngleInSec = HandleAng->ShaftAngleInSec; //сброс уставки хода вала
				return receive_step; //остановка вращения и возврат фактически пройденного угла	
			}		
			if (((count % 100) == 0) && (count != 0))
			//if ((count % 100) == 0) 
			{	
				if (read_enc_shaft_rotation (HandleAng, HandleEncData) == ON)
				{ continue;	}
			}
			delay_us (1499); 			
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

//-----------------------------------------------------------------------------------------------------//
uint8_t read_enc_shaft_rotation (angular_data_t * HandleAng, encoder_data_t * HandleEncData) 
{
	int32_t currCounter = 0; //текущее показание энкодера
	int32_t currDelta = 0; //разница между полученным показаниям энкодера и предыдущим
	int32_t changeAng = 0; //изменение угла уставки (щелчки энкодера * угол хода вала)
	
	currCounter= LL_TIM_GetCounter(TIM3); //получение текущего показания энкодера
	HandleEncData->currCounter_ShaftRotation= (32767 - ((currCounter-1) & 0xFFFF))/2; //деление на 2, счёт щелчков (щелчок = 2 импульса)
	
	if(HandleEncData->currCounter_ShaftRotation != HandleEncData->prevCounter_ShaftRotation) //если показания энкодера изменились
	{
		currDelta = (HandleEncData->currCounter_ShaftRotation - HandleEncData->prevCounter_ShaftRotation); //разница между полученным и предыдущим показанием энкодера
    HandleEncData->prevCounter_ShaftRotation = HandleEncData->currCounter_ShaftRotation; //сохранение текущего показания энкодера    
		
		if (currDelta != 0) //если изменилось положение энкодера
		{		
			HandleEncData->delta += currDelta; //сохранение изменения показаний энкодера (кол-во щелчков энкодера)
			if (currDelta > 0) //если повернулся по часовой стрелке
			{
				changeAng = HandleAng->StepAngleInSec * currDelta; //получение изменения угла уставки вала
				HandleAng->SetShaftAngleInSec = HandleAng->SetShaftAngleInSec + changeAng; //расчёт угла уставки вала
				if (HandleAng->SetShaftAngleInSec > (CIRCLE_IN_SEC-1)) //если угол положения вала 360 гр. или больше
				{	HandleAng->SetShaftAngleInSec = HandleAng->SetShaftAngleInSec - CIRCLE_IN_SEC;	} 
			}
			else
			{
				if (currDelta < 0) //если энкодер повернулся против часовой стрелке
				{
					changeAng = -1	* (HandleAng->StepAngleInSec) * currDelta; //получение изменения угла уставки вала
					
					if (HandleAng->SetShaftAngleInSec >= changeAng) //если текущее положение вала больше или равно углу уставки
					{	HandleAng->SetShaftAngleInSec = HandleAng->SetShaftAngleInSec - changeAng; }
					else 
					{
						if (HandleAng->SetShaftAngleInSec < changeAng) //если текущее положение вала меньше угла уставки
						{	HandleAng->SetShaftAngleInSec = (CIRCLE_IN_SEC - (changeAng - HandleAng->SetShaftAngleInSec)); }			 
					}
				}
			}
			GetSetShaftAngle_from_Seconds (HandleAng); //перевод угла уставки в формат гр./мин./с для отображения на дисплее
			default_screen_mode1 (HandleAng); //главное меню режима 1
			return ON;
		}
	}
	return OFF; 
}

//--------------------------------------сброс показаний энкодера--------------------------------------//
void encoder_reset (encoder_data_t * HandleEncData) 
{
	int32_t encCounter = 0; //переменная для хранения данных энкодера
	encCounter = LL_TIM_GetCounter(TIM3); //сохранение текущего показания энкодера
	HandleEncData->currCounter_SetAngle = (32767 - ((encCounter-1) & 0xFFFF))/2; //преобразование полученного показания энкодера в формат от -10 до 10
	HandleEncData->prevCounter_SetAngle = HandleEncData->currCounter_SetAngle; //сохранение преобразованного текущего показания энкодера в структуру установки шага поворота	
	HandleEncData->prevCounter_ShaftRotation = HandleEncData->currCounter_SetAngle; //сохранение преобразованного текущего показания энкодера в структуру данных положения вала
	HandleEncData->delta = 0; //показания от энкодера обнуляются
}

//----------------------------------------------------------------------------------------------------//
void step_by_step (angular_data_t * HandleAng, encoder_data_t * HandleEncData) 
{
	uint32_t need_step = 0;  //количество микрошагов(импульсов), которые необходимо пройти
	uint8_t dir; //направление вращения вала
	uint32_t PassedAngleInSec = 0; //фактически пройденный угол в секундах 
	uint32_t receive_step = 0; //количество фактически пройденных микрошагов(импульсов)
	
	if (HandleEncData->delta != 0) //если изменилось положение энкодера
	{
		if (HandleEncData->delta > 0) //если энкодер повернулся по часовой стрелке
		{
			need_step = calc_steps_mode1 (HandleAng, step_unit); //перевод угла поворота вала в количество микрошагов (импульсов) 
			receive_step += step_angle (FORWARD, need_step, HandleAng, HandleEncData); //поворот по часовой стрелке				
			if (HandleEncData->delta != 0) //если дельта не равна нулю
			{	HandleEncData->delta--; }	//уменьшение дельты
			
			PassedAngleInSec = calc_passed_angle (receive_step, step_unit); //расчёт фактического поворота вала
			HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec +	PassedAngleInSec; //расчёт текущего положения вала
			if (HandleAng->ShaftAngleInSec > (CIRCLE_IN_SEC-1)) //если угол положения вала 360 гр. или больше
			{	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec - CIRCLE_IN_SEC;	} 							
		}
		else
		{
			if (HandleEncData->delta < 0) //если энкодер повернулся против часовой стрелке
			{	
				need_step = calc_steps_mode1 (HandleAng, step_unit); //расчёт количества необходимых микрошагов
				receive_step += step_angle (BACKWARD, need_step, HandleAng, HandleEncData); //поворот против часовой стрелки
				if (HandleEncData->delta != 0)	//если дельта не равна нулю
				{	HandleEncData->delta++; }		//увеличение дельты
				PassedAngleInSec = calc_passed_angle (receive_step, step_unit);
				
				if (HandleAng->ShaftAngleInSec >= PassedAngleInSec) //если угол положения вала больше пройдённого угла 
				{	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec - PassedAngleInSec; }
				else 
				{
					if (HandleAng->ShaftAngleInSec < PassedAngleInSec) //если угол положения вала меньше угла шага
					{	HandleAng->ShaftAngleInSec = (CIRCLE_IN_SEC - (PassedAngleInSec - HandleAng->ShaftAngleInSec)); }	
				}						 
			}
		}
		if (HandleEncData->delta == 0) //если изменение показания энкодера полностью пройдено валом
		{
			GetAngleShaft_from_Seconds(HandleAng); //конвертация текущего угла вала в секундах в формат гр/мин/сек
			default_screen_mode1 (HandleAng); //главное меню режима 1
		}
		angle_to_EEPROMbuf (HandleAng, eeprom_tx_buffer); //перенос данных угла в буффер
		EEPROM_WriteBytes (EEPROM_MEMORY_PAGE, eeprom_tx_buffer, (EEPROM_NUMBER_BYTES-6)); //запись буффера с данными угла поворота в
	}
	else
	{	HandleAng->SetShaftAngleInSec = HandleAng->ShaftAngleInSec;	}  //уставка вала равна текущему положению вала
}

//---------------------------обработка показаний энкодера в режиме управлением вала----------------------------//
void enc_shaft_rotation (angular_data_t * HandleAng, encoder_data_t * HandleEncData) 
{
	uint32_t need_step = 0; //количество вычисленных микрошагов(импульсов)
	uint32_t receive_step = 0; //количество фактически пройденных микрошагов(импульсов)
	int32_t currCounter=0; //текущее показание энкодера
	uint32_t PassedAngleInSec = 0;
	
	currCounter= LL_TIM_GetCounter(TIM3); //текущее показание энкодера
	HandleEncData->currCounter_ShaftRotation= (32767 - ((currCounter-1) & 0xFFFF))/2; //деление на 2, счёт щелчков (щелчок = 2 импульса)
	
	if(HandleEncData->currCounter_ShaftRotation != HandleEncData->prevCounter_ShaftRotation) //если показания энкодера изменились
	{
		HandleEncData->delta = (HandleEncData->currCounter_ShaftRotation - HandleEncData->prevCounter_ShaftRotation); //разница между текущим и предыдущим показанием энкодера
    HandleEncData->prevCounter_ShaftRotation = HandleEncData->currCounter_ShaftRotation; //сохранение текущего показания энкодера    
		if((HandleEncData->delta > -20) && (HandleEncData->delta < 20)) // защита от дребезга контактов и переполнения счетчика 
		{
			if (HandleEncData->delta != 0) //если изменилось положение энкодера
			{ 
				SetAngle_in_Seconds (HandleAng); //перевод угловых данных шага в секунды		
				if (HandleEncData->delta > 0) //если энкодер повернулся по часовой стрелке
				{
					while (HandleEncData->delta > 0) //пока дельта больше нуля
					{
						need_step = calc_steps_mode1 (HandleAng, step_unit);
						receive_step += step_angle (FORWARD, need_step, HandleAng, HandleEncData); //поворот по часовой стрелке				
						
						if (HandleEncData->delta != 0) //если дельта не равна нулю
						{	HandleEncData->delta--; } 		//уменьшение дельты
						PassedAngleInSec = calc_passed_angle (receive_step, step_unit);
						
						HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec +	PassedAngleInSec;
						if (HandleAng->ShaftAngleInSec > (CIRCLE_IN_SEC-1)) //если угол положения вала 360 гр. или больше
						{	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec - CIRCLE_IN_SEC;	} 										
					}
				}
				else
				{
					if (HandleEncData->delta < 0) //если энкодер повернулся против часовой стрелке
					{	
						while (HandleEncData->delta < 0) //пока дельта меньше нуля
						{
							need_step = calc_steps_mode1 (HandleAng, step_unit); //расчёт количества необходимых микрошагов
							receive_step += step_angle (BACKWARD, need_step, HandleAng, HandleEncData); //поворот против часовой стрелки
							
							if (HandleEncData->delta != 0)	//если дельта не равна нулю
							{	HandleEncData->delta++; }		//увеличение дельты
							PassedAngleInSec = calc_passed_angle (receive_step, step_unit);
							
							if (HandleAng->ShaftAngleInSec >= PassedAngleInSec) //если угол положения вала больше пройдённого угла 
							{	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec - PassedAngleInSec; }
							else 
							{
								if (HandleAng->ShaftAngleInSec < PassedAngleInSec) //если угол положения вала меньше угла шага
								{	HandleAng->ShaftAngleInSec = (CIRCLE_IN_SEC - (PassedAngleInSec - HandleAng->ShaftAngleInSec)); }	//перевод в формат 359гр. ххмин.
							}						
						}
					}
				}
				GetAngleShaft_from_Seconds(HandleAng); //конвертация текущего угла вала в секундах в формат гр/мин/сек
				default_screen_mode1 (HandleAng); //главное меню режима 1 	
				angle_to_EEPROMbuf (HandleAng, eeprom_tx_buffer); //перенос данных угла в буффер
				EEPROM_WriteBytes (EEPROM_MEMORY_PAGE, eeprom_tx_buffer, (EEPROM_NUMBER_BYTES-6)); //запись буффера с данными угла поворота в памяти					
			}
			else
			{	HandleEncData->delta = 0;	}
		}
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
				HandleAng->StepAngleInSec += (delta*SECOND_PER_MINUTE); //прибавление минуты
				if (HandleAng->StepAngleInSec == 0) 							//если угол становится равным 0
				{ HandleAng->StepAngleInSec = (CIRCLE_IN_SEC); } 	//угол равен 360 гр
				else
				{
					if (HandleAng->StepAngleInSec > (CIRCLE_IN_SEC)) //если угол становится равным больше 359 гр 59 минут
					{ HandleAng->StepAngleInSec = SECOND_PER_MINUTE; } //угол равен 1 минуте
				}
				GetSetAngle_from_Seconds (HandleAng); //перевод угла шага хода вала из секунд в формат гр/мин/с
				setangle_mode_screen (HandleAng); //вывод информации на дисплей
			}
		}
		else
		{	delta = 0; }
	}				
}

//----------------------обработка показаний энкодера в режиме устаноки количества зубьев----------------------//
void set_teeth_gear (milling_data_t * HandleMil, encoder_data_t * HandleEncData) 
{
	int32_t currCounter=0;

	currCounter = LL_TIM_GetCounter(TIM3); //текущее показание энкодера
	HandleEncData->currCounter_SetAngle = (32767 - ((currCounter-1) & 0xFFFF))/2; //деление на 2, счёт щелчков (щелчок = 2 импульса)
	
	if(HandleEncData->currCounter_SetAngle != HandleEncData->prevCounter_SetAngle) //если текущее значение энкодера на равно предыдущему
	{
		HandleEncData->delta = (HandleEncData->currCounter_SetAngle - HandleEncData->prevCounter_SetAngle); //разница между текущим и предыдущим показанием энкодера
    HandleEncData->prevCounter_SetAngle = HandleEncData->currCounter_SetAngle; //сохранение текущего показанаия энкодера    
    if((HandleEncData->delta > -20) && (HandleEncData->delta < 20)) // защита от дребезга контактов и переполнения счетчика (переполнение будет случаться очень редко)
		{
			if (HandleEncData->delta != 0) //если изменилось положение энкодера
			{  
				HandleMil->teeth_gear_numbers += HandleEncData->delta;
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

//---------------------------------------------------------------------------------------------------//
void right_shaft_rotation (angular_data_t * HandleAng, encoder_data_t * HandleEncData) 
{
	uint32_t need_step = 0; //количество вычисленных микрошагов(импульсов)
	SetAngle_in_Seconds (HandleAng); //перевод угловых данных шага в секунды
	need_step = calc_steps_mode1 (HandleAng, step_unit);
	step_angle (FORWARD, need_step, HandleAng, HandleEncData); //поворот по часовой стрелке
	
	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec + HandleAng->StepAngleInSec;
	if (HandleAng->ShaftAngleInSec > (CIRCLE_IN_SEC-1)) //если угол положения вала 360 гр. или больше
	{	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec - CIRCLE_IN_SEC;	} 
	
	GetAngleShaft_from_Seconds(HandleAng); //конвертация текущего угла вала в секундах в формат гр/мин/сек
	angle_to_EEPROMbuf (HandleAng, eeprom_tx_buffer); //перенос данных угла в буффер
	EEPROM_WriteBytes (EEPROM_MEMORY_PAGE, eeprom_tx_buffer, 8); //запись буффера с данными угла поворота в памяти					
}

//---------------------------------------------------------------------------------------------------//
void left_shaft_rotation (angular_data_t * HandleAng, encoder_data_t * HandleEncData) 
{
	uint32_t need_step = 0; //количество вычисленных микрошагов(импульсов)
	SetAngle_in_Seconds (HandleAng); //перевод угловых данных шага в секунды
	need_step = calc_steps_mode1 (HandleAng, step_unit);
	step_angle (BACKWARD, need_step, HandleAng, HandleEncData); //поворот против часовой стрелке
	
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
void one_full_turn (angular_data_t * HandleAng, encoder_data_t * HandleEncData) 
{
	step_angle (FORWARD, (STEPS_IN_REV*REDUCER), HandleAng, HandleEncData);  //полный оборот на 360гр с учётом делителя редуктора (360гр*40)
}

//----------------------------возврат вала в нулевую позицию против часовой стрелке----------------------------//
void left_rotate_to_zero (angular_data_t * HandleAng, encoder_data_t * HandleEncData) 
{
	uint32_t need_step = 0; //количество вычисленных микрошагов(импульсов)
	uint32_t receive_step = 0; //количество фактически пройденных микрошагов(импульсов)
	uint32_t PassedAngleInSec = 0;
	
	need_step = steps_for_back_to_zero (HandleAng->ShaftAngleInSec, step_unit);
	receive_step = step_angle (BACKWARD, need_step, HandleAng, HandleEncData); //поворот против часовой стрелке
	//PassedAngleInSec = calc_passed_angle (receive_step, step_unit);
	
	HandleAng->ShaftAngleInSec = 0; //текущее положение вала нулевое
	//HandleAng->ShaftAngleInSec -= PassedAngleInSec;
	GetAngleShaft_from_Seconds(HandleAng); //конвертация текущего угла вала в секундах в формат гр/мин/сек
	angle_to_EEPROMbuf (HandleAng, eeprom_tx_buffer); //перенос данных угла в буффер
	EEPROM_WriteBytes (EEPROM_MEMORY_PAGE, eeprom_tx_buffer, 8); //запись буффера с данными угла поворота в памяти
}

//-----------------------------возврат вала в нулевую позицию по часовой стрелке-----------------------------//
void right_rotate_to_zero (angular_data_t * HandleAng, encoder_data_t * HandleEncData) 
{
	uint32_t need_step = 0; //количество вычисленных микрошагов(импульсов)
	uint32_t receive_step = 0; //количество фактически пройденных микрошагов(импульсов)
	uint32_t PassedAngleInSec = 0;
	
	need_step = steps_for_back_to_zero ((CIRCLE_IN_SEC - HandleAng->ShaftAngleInSec), step_unit);
	receive_step = step_angle (FORWARD, need_step, HandleAng, HandleEncData); //поворот по часовой стрелке
	//PassedAngleInSec = calc_passed_angle (receive_step, step_unit);
	
	HandleAng->ShaftAngleInSec = 0; //текущее положение вала нулевое
	//HandleAng->ShaftAngleInSec += PassedAngleInSec;
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
		milling_step (FORWARD, need_step); //поворот по часовой стрелке
		
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
		milling_step (BACKWARD, need_step); //поворот против часовой стрелке
		
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