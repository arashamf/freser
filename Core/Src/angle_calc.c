
// Includes ---------------------------------------------------------------------------------------//
#include "angle_calc.h"

// Functions --------------------------------------------------------------------------------------//

//------------------------------------------------------------------------------------------------//
void angle_to_EEPROMbuf (angular_data_t * handle, uint8_t * EEPROM_buffer)
{	
	//передача угловых данных установки поворота вала из EEPROM
	*(EEPROM_buffer+0) = (uint8_t)(handle->StepAngleInSec >> 24); 
	*(EEPROM_buffer+1) = (uint8_t)(handle->StepAngleInSec >> 16); 
	*(EEPROM_buffer+2) = (uint8_t)(handle->StepAngleInSec >> 8); 
	*(EEPROM_buffer+3) = (uint8_t)(handle->StepAngleInSec >> 0); 
	//передача угловых данных текущего положения вала из EEPROM
	*(EEPROM_buffer+4) = (uint8_t)(handle->ShaftAngleInSec >> 24); 
	*(EEPROM_buffer+5) = (uint8_t)(handle->ShaftAngleInSec >> 16); 
	*(EEPROM_buffer+6) = (uint8_t)(handle->ShaftAngleInSec >> 8); 
	*(EEPROM_buffer+7) = (uint8_t)(handle->ShaftAngleInSec >> 0);
}

//------------------------------------------------------------------------------------------------//
void angle_from_EEPROMbuf (angular_data_t * handle, uint8_t * EEPROM_buffer)
{
	handle->StepAngleInSec = (uint32_t)(((*(EEPROM_buffer+0))<<24) | ((*(EEPROM_buffer+1))<<16) | ((*(EEPROM_buffer+2))<<8) | ((*(EEPROM_buffer+3))<<0));
	handle->ShaftAngleInSec = (uint32_t)(((*(EEPROM_buffer+4))<<24) | ((*(EEPROM_buffer+5))<<16) | ((*(EEPROM_buffer+6))<<8) | ((*(EEPROM_buffer+7))<<0));
}

//------------------------перевод угла шага хода вала из формата гр/мин/с в секунды------------------------//
void SetAngle_in_Seconds (angular_data_t * handle)
{
	handle->StepAngleInSec=0;
	handle->StepAngleInSec += handle->set_second;
	handle->StepAngleInSec += handle->set_minute*SECOND_PER_MINUTE ;
	handle->StepAngleInSec += handle->set_degree*SECOND_PER_DEGREE;
}

//-------------------------перевод угла шага хода вала из секунд в формат гр/мин/с-------------------------//
void GetSetAngle_from_Seconds (angular_data_t * handle)
{
	uint32_t tmp = 0;
	handle->set_degree = handle->StepAngleInSec/ SECOND_PER_DEGREE; //количество градусов
	tmp = handle->StepAngleInSec % SECOND_PER_DEGREE; //остаток с минутами и секундами
	handle->set_minute = tmp/SECOND_PER_MINUTE;  //количество минут
	handle->set_second = tmp % SECOND_PER_MINUTE; //количество секунд
}

//-------------------перевод угла текущего положения вала из формата гр/мин/с в секунды-------------------//
void ShaftAngle_in_Seconds (angular_data_t * handle)
{
	handle->ShaftAngleInSec = 0;
	handle->ShaftAngleInSec += handle->shaft_second;
	handle->ShaftAngleInSec += handle->shaft_minute*SECOND_PER_MINUTE;
	handle->ShaftAngleInSec += handle->shaft_degree*SECOND_PER_DEGREE;
}

//-------------------перевод угла текущего положения вала из секунд в формат гр/мин/с-------------------//
void GetAngleShaft_from_Seconds (angular_data_t * handle)
{
	uint32_t tmp = 0;
	handle->shaft_degree = handle->ShaftAngleInSec/(SECOND_PER_DEGREE); //количество градусов
	tmp = handle->ShaftAngleInSec % SECOND_PER_DEGREE; //остаток с минутами и секундами
	handle->shaft_minute = tmp/SECOND_PER_MINUTE; //количество минут
	handle->shaft_second = tmp % SECOND_PER_MINUTE; //количество секунд
}

//-------------------------------обнуление угла текущего положения вала -------------------------------//
void AngleShaftReset (angular_data_t * handle)
{
	handle->shaft_second = 0; 
	handle->shaft_minute = 0; 
	handle->shaft_degree = 0;
	handle->ShaftAngleInSec = 0; //текущее положение вала - нулевое
	handle->mode1_error = 0;
}

//---------------------------сброс угла шага хода вала на минимальное значение---------------------------//
void SetAngleReset (angular_data_t * handle)
{
	handle->set_second = 0;
	handle->set_minute = 1; 
	handle->set_degree = 0;
	SetAngle_in_Seconds (handle);
}

//-----------------перевод угла поворота в режиме фрезеровки из формата гр/мин/с в секунды-----------------//
void MilAngleTeeth_in_Seconds (milling_data_t * handle)
{
	handle->AngleTeethInSec = 0;
	handle->AngleTeethInSec += handle->step_shaft_second;
	handle->AngleTeethInSec += handle->step_shaft_minute*SECOND_PER_MINUTE;
	handle->AngleTeethInSec += handle->step_shaft_degree*SECOND_PER_DEGREE;
}

//------------------перевод угла поворота в режиме фрезеровки из секунд в формат гр/мин/с------------------//
void MilAngleTeeth_from_Seconds (milling_data_t * handle)
{
	uint32_t tmp = 0;
	handle->step_shaft_degree = handle->AngleTeethInSec/SECOND_PER_DEGREE; //количество градусов
	tmp = handle->AngleTeethInSec % SECOND_PER_DEGREE; //остаток с минутами и секундами
	handle->step_shaft_minute = tmp/SECOND_PER_MINUTE; //количество минут
	handle->step_shaft_second = tmp % SECOND_PER_MINUTE; //количество секунд
}

//---------------------------расчёт угла поворота после ввода количества зубов---------------------------//
void GetMilAngleTeeth (milling_data_t * handle)
{	
	uint32_t tmp = 0;
	handle->AngleTeethInSec = CIRCLE_IN_SEC/handle->teeth_gear_numbers; //угол между зубьями
	MilAngleTeeth_from_Seconds (handle); //перевод угла поворота из секунд в формат гр/мин/с
}

//---------------------сброс угловых данных режима фрезеровки на минимальное значение---------------------//
void MilAngleTeethReset (milling_data_t * handle)
{	
	handle->teeth_gear_numbers 	= 2;
	handle->remain_teeth_gear 	= handle->teeth_gear_numbers;
	handle->step_shaft_degree 	= 0; 
	handle->step_shaft_minute 	= 0; 
	handle->step_shaft_second 	= 0;
	handle->AngleTeethInSec 		= 0;
	handle->milling_error = 0;
}

//------------------------------------------------------------------------------------------------//
void RemainTeethReset (milling_data_t * handle)
{
	handle->remain_teeth_gear = handle->teeth_gear_numbers;
	handle->milling_error = 0;
}

//------------------------------------------------------------------------------------------------//
void teeth_angle_to_EEPROMbuf (milling_data_t * handle, uint8_t * EEPROM_buffer, STATUS_FLAG_t * status_flag)
{
	//передача угловых данных установки поворота вала из EEPROM
	*(EEPROM_buffer+8) = (uint8_t)(handle->remain_teeth_gear);
	*(EEPROM_buffer+9) = (int8_t)(handle->teeth_gear_numbers); 
	*(EEPROM_buffer+10) = (uint8_t)(status_flag->flag); 		
}

//------------------------------------------------------------------------------------------------//
void teeth_angle_from_EEPROMbuf (milling_data_t * handle, uint8_t * EEPROM_buffer, STATUS_FLAG_t * status_flag)
{
	handle->remain_teeth_gear = (uint8_t)(*(EEPROM_buffer+8)); //сохранение оставшихся количества зубьев
	handle->teeth_gear_numbers = (uint8_t)(*(EEPROM_buffer+9)); //сохранение установленного количества зубьев
	status_flag->flag = (uint8_t)(*(EEPROM_buffer+10));
}

//------------------------------------------------------------------------------------------------//
void remain_teeth_to_EEPROMbuf (milling_data_t * handle, uint8_t * EEPROM_buffer)
{
	*(EEPROM_buffer+8) = (uint8_t)(handle->remain_teeth_gear); //сохранение оставшихся количества зубьев
}

//------------------------------------------------------------------------------------------------//
