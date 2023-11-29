
// Includes ---------------------------------------------------------------------------------------//
#include "angle_calc.h"

// Functions --------------------------------------------------------------------------------------//
void init_angular_data (angular_data_t * measure_handle)
{
	measure_handle->degree = 0;
	measure_handle->minute = 0;
	measure_handle->second = 0;
}

//------------------------------------------------------------------------------------------------//
void angle_to_EEPROMbuf (angular_data_t * handle, uint8_t * EEPROM_buffer)
{
	*(EEPROM_buffer+0) = (uint8_t)(handle->second >> 8); //сначала старшая часть секунды
	*(EEPROM_buffer+1) = (uint8_t)handle->second; //младшая часть секунды
	*(EEPROM_buffer+2) = (uint8_t)(handle->minute >> 8); //сначала старшая часть минуты
	*(EEPROM_buffer+3) = (uint8_t)handle->minute; //младшая часть минуты
	*(EEPROM_buffer+4) = (uint8_t)(handle->degree>>24);
	*(EEPROM_buffer+5) = (uint8_t)(handle->degree>>16);
	*(EEPROM_buffer+6) = (uint8_t)(handle->degree>>8);
	*(EEPROM_buffer+7) = (uint8_t)(handle->degree);
}

//------------------------------------------------------------------------------------------------//
void angle_from_EEPROMbuf (angular_data_t * handle, uint8_t * EEPROM_buffer)
{
	handle->second |= (((*(EEPROM_buffer+0)) << 8) | (*(EEPROM_buffer+1)));
	handle->minute |= (((*(EEPROM_buffer+2)) << 8) | (*(EEPROM_buffer+3)));
	handle->degree |= (((*(EEPROM_buffer+4)) << 24) | ((*(EEPROM_buffer+5)) << 16) | ((*(EEPROM_buffer+6)) << 8) | (*(EEPROM_buffer+7)));
}

//------------------------------------------------------------------------------------------------//
void angle_in_seconds (angular_data_t * handle)
{
	handle->AngleInSecond=0;
	handle->AngleInSecond += handle->second;
	handle->AngleInSecond += handle->minute*60;
	handle->AngleInSecond += handle->degree*60*60;
}

//------------------------------------------------------------------------------------------------//

