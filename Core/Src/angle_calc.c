
// Includes ---------------------------------------------------------------------------------------//
#include "angle_calc.h"

// Functions --------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//
void angle_to_EEPROMbuf (angular_data_t * handle, uint8_t * EEPROM_buffer)
{
	//передача угловых данных установки поворота вала из EEPROM
	*(EEPROM_buffer+0) = (uint8_t)handle->set_second; 				
	*(EEPROM_buffer+1) = (uint8_t)handle->set_minute; 				
	*(EEPROM_buffer+2) = (uint8_t)(handle->set_degree>>8); //сначала старшая часть градуса
	*(EEPROM_buffer+3) = (uint8_t)(handle->set_degree); 	//младшая часть градуса
	
	//передача угловых данных текущего положения вала из EEPROM
	*(EEPROM_buffer+4) 	= (uint8_t)	handle->shaft_second; 				
	*(EEPROM_buffer+5) = (uint8_t)	handle->shaft_minute; 				
	*(EEPROM_buffer+6) = (uint8_t)(handle->shaft_degree>>8);
	*(EEPROM_buffer+7) = (uint8_t)(handle->shaft_degree);
	
}

//------------------------------------------------------------------------------------------------//
void angle_from_EEPROMbuf (angular_data_t * handle, uint8_t * EEPROM_buffer)
{
	handle->set_second 		= 	(uint8_t)(*(EEPROM_buffer+0));
	handle->set_minute 		= 	(uint8_t)(*(EEPROM_buffer+1));
	handle->set_degree 		= 	(uint16_t)(((*(EEPROM_buffer+2)) << 8) | (*(EEPROM_buffer+3)));
	handle->shaft_second 	= 	(uint8_t)(*(EEPROM_buffer+4));
	handle->shaft_minute 	= 	(uint8_t)(*(EEPROM_buffer+5));
	handle->shaft_degree 	= 	(uint16_t)(((*(EEPROM_buffer+6)) << 8) | (*(EEPROM_buffer+7)));
}

//------------------------------------------------------------------------------------------------//
void init_angular_data (angular_data_t * measure_handle, uint8_t * EEPROM_buffer)
{
	angle_from_EEPROMbuf (measure_handle, EEPROM_buffer);
}


//------------------------------------------------------------------------------------------------//
void SetAngle_in_Seconds (angular_data_t * handle)
{
	handle->StepAngleInSec=0;
	handle->StepAngleInSec += handle->set_second;
	handle->StepAngleInSec += handle->set_minute*60;
	handle->StepAngleInSec += handle->set_degree*60*60;
}

//------------------------------------------------------------------------------------------------//
void GetSetAngle_from_Seconds (angular_data_t * handle)
{
	uint32_t tmp = 0;
	handle->set_degree = handle->StepAngleInSec/(3600);
	tmp = handle->StepAngleInSec % 3600; //остаток с минутами и секундами
	handle->set_minute = tmp/60;
	handle->set_second = tmp % 60;
}

//------------------------------------------------------------------------------------------------//
void ShaftAngle_in_Seconds (angular_data_t * handle)
{
	handle->ShaftAngleInSec = 0;
	handle->ShaftAngleInSec += handle->shaft_second;
	handle->ShaftAngleInSec += handle->shaft_minute*60;
	handle->ShaftAngleInSec += handle->shaft_degree*60*60;
}

//------------------------------------------------------------------------------------------------//
void GetAngleShaft_from_Seconds (angular_data_t * handle)
{
	uint32_t tmp = 0;
	handle->shaft_degree = handle->ShaftAngleInSec/(3600);
	tmp = handle->ShaftAngleInSec % 3600; //остаток с минутами и секундами
	handle->shaft_minute = tmp/60;
	handle->shaft_second = tmp % 60;
}

//------------------------------------------------------------------------------------------------//
void AngleShaftReset (angular_data_t * handle)
{
	handle->shaft_second = 0; 
	handle->shaft_minute = 0; 
	handle->shaft_degree = 0;
	handle->ShaftAngleInSec = 0; //текущее положение вала - нулевое
}

//------------------------------------------------------------------------------------------------//
void GetAngleReset (angular_data_t * handle)
{
	handle->set_second = 0;
	handle->set_minute = 1; 
	handle->set_degree = 0;
	GetSetAngle_from_Seconds (handle);
}

