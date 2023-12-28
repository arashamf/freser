
// Includes ---------------------------------------------------------------------------------------//
#include "angle_calc.h"

// Functions --------------------------------------------------------------------------------------//
//------------------------------------------------------------------------------------------------//
void angle_to_EEPROMbuf (angular_data_t * handle, uint8_t * EEPROM_buffer)
{	
	//�������� ������� ������ ��������� �������� ���� �� EEPROM
	*(EEPROM_buffer+0) = (uint8_t)(handle->StepAngleInSec >> 24); 
	*(EEPROM_buffer+1) = (uint8_t)(handle->StepAngleInSec >> 16); 
	*(EEPROM_buffer+2) = (uint8_t)(handle->StepAngleInSec >> 8); 
	*(EEPROM_buffer+3) = (uint8_t)(handle->StepAngleInSec >> 0); 
	//�������� ������� ������ �������� ��������� ���� �� EEPROM
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

//-----------------------------------------------------------------------------------------------------//
void init_angular_data (angular_data_t * measure_handle, uint8_t * EEPROM_buffer)
{
	angle_from_EEPROMbuf (measure_handle, EEPROM_buffer);
}


//------------------------������� ������� ������ �� ������� ��/���/� � �������------------------------//
void SetAngle_in_Seconds (angular_data_t * handle)
{
	handle->StepAngleInSec=0;
	handle->StepAngleInSec += handle->set_second;
	handle->StepAngleInSec += handle->set_minute*60;
	handle->StepAngleInSec += handle->set_degree*60*60;
}

//-------------------------������� ������� ������ �� ������ � ������ ��/���/�-------------------------//
void GetSetAngle_from_Seconds (angular_data_t * handle)
{
	uint32_t tmp = 0;
	handle->set_degree = handle->StepAngleInSec/(3600);
	tmp = handle->StepAngleInSec % 3600; //������� � �������� � ���������
	handle->set_minute = tmp/60;
	handle->set_second = tmp % 60;
}

//----------------------------------------------------------------------------------------------------//
void ShaftAngle_in_Seconds (angular_data_t * handle)
{
	handle->ShaftAngleInSec = 0;
	handle->ShaftAngleInSec += handle->shaft_second;
	handle->ShaftAngleInSec += handle->shaft_minute*60;
	handle->ShaftAngleInSec += handle->shaft_degree*60*60;
}

//----------------------------------------------------------------------------------------------------//
void GetAngleShaft_from_Seconds (angular_data_t * handle)
{
	uint32_t tmp = 0;
	handle->shaft_degree = handle->ShaftAngleInSec/(3600);
	tmp = handle->ShaftAngleInSec % 3600; //������� � �������� � ���������
	handle->shaft_minute = tmp/60;
	handle->shaft_second = tmp % 60;
}

//------------------------------------------------------------------------------------------------//
void AngleShaftReset (angular_data_t * handle)
{
	handle->shaft_second = 0; 
	handle->shaft_minute = 0; 
	handle->shaft_degree = 0;
	handle->ShaftAngleInSec = 0; //������� ��������� ���� - �������
}

//------------------------------------------------------------------------------------------------//
void SetAngleReset (angular_data_t * handle)
{
	handle->set_second = 0;
	handle->set_minute = 1; 
	handle->set_degree = 0;
	SetAngle_in_Seconds (handle);
}

//------------------------------------------------------------------------------------------------//
void MilAngleTeeth_in_Seconds (milling_data_t * handle)
{
	handle->AngleTeethInSec = 0;
	handle->AngleTeethInSec += handle->step_shaft_second;
	handle->AngleTeethInSec += handle->step_shaft_minute*60;
	handle->AngleTeethInSec += handle->step_shaft_degree*60*60;
}

//------------------------------------------------------------------------------------------------//
void MilAngleTeeth_from_Seconds (milling_data_t * handle)
{
	uint32_t tmp = 0;
	handle->step_shaft_degree = handle->AngleTeethInSec/(3600);
	tmp = handle->AngleTeethInSec % 3600; //������� � �������� � ���������
	handle->step_shaft_minute = tmp/60;
	handle->step_shaft_second = tmp % 60;
}

//------------------------------------------------------------------------------------------------//
void GetMilAngleTeeth (milling_data_t * handle)
{	
	uint32_t tmp = 0;
	handle->remain_teeth_gear = handle->teeth_gear_numbers; //���������� ���������� ���������� �����	
	handle->AngleTeethInSec = CIRCLE_IN_SEC/handle->teeth_gear_numbers; //���� ����� �������
	handle->step_shaft_degree = handle->AngleTeethInSec/(3600); //���� � ��������
	tmp = handle->AngleTeethInSec % 3600; //������� � �������� � ���������
	handle->step_shaft_minute = tmp/60;
	handle->step_shaft_second = tmp % 60;
}

//------------------------------------------------------------------------------------------------//
void MilAngleTeethReset (milling_data_t * handle)
{	
	handle->teeth_gear_numbers 	= 0;
	handle->remain_teeth_gear 	= 0;
	handle->step_shaft_degree 	= 0; 
	handle->step_shaft_minute 	= 0; 
	handle->step_shaft_second 	= 0;
	handle->AngleTeethInSec 		= 0;
}

//------------------------------------------------------------------------------------------------//
void teeth_angle_to_EEPROMbuf (milling_data_t * handle, uint8_t * EEPROM_buffer)
{
	//�������� ������� ������ ��������� �������� ���� �� EEPROM
	*(EEPROM_buffer+8) = (int8_t)(handle->teeth_gear_numbers>>8);
	*(EEPROM_buffer+9) = (int8_t)(handle->teeth_gear_numbers); 
	*(EEPROM_buffer+10) = (uint8_t)(handle->remain_teeth_gear>>8);
	*(EEPROM_buffer+11) = (uint8_t)(handle->remain_teeth_gear);
	*(EEPROM_buffer+12) = (uint8_t)(handle->AngleTeethInSec >> 24); 	
	*(EEPROM_buffer+13) = (uint8_t)(handle->AngleTeethInSec >> 16); 				
	*(EEPROM_buffer+14) = (uint8_t)(handle->AngleTeethInSec >> 8); //������� ������� ����� �������
	*(EEPROM_buffer+15) = (uint8_t)(handle->AngleTeethInSec); 	//������� ����� �������		
}

//------------------------------------------------------------------------------------------------//
void teeth_angle_from_EEPROMbuf (milling_data_t * handle, uint8_t * EEPROM_buffer)
{
	handle->teeth_gear_numbers = (int16_t)(((*(EEPROM_buffer+8)) << 8) | (*(EEPROM_buffer+9)));
	handle->remain_teeth_gear = (uint16_t)(((*(EEPROM_buffer+10)) << 8) | (*(EEPROM_buffer+11)));
	handle->AngleTeethInSec = (uint32_t)(((*(EEPROM_buffer+12)) << 24) | ((*(EEPROM_buffer+13)) << 16)
	| ((*(EEPROM_buffer+14)) << 8) | ((*(EEPROM_buffer+15)) << 0));
}

//------------------------------------------------------------------------------------------------//
void init_teeth_angle_data (milling_data_t * handle, uint8_t * EEPROM_buffer)
{
	teeth_angle_from_EEPROMbuf (handle, EEPROM_buffer);
}
