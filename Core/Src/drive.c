
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
	
	if ((quant = (uint16_t)need_step/STEP_DIV) > 0) //���������� ����� ����� �� 1,8 �������
	{
		for (uint16_t count = 0; count < quant; count++)
		{
			DRIVE_ENABLE(ON);
			delay_us (6);
			rotate_one_step (STEP_DIV); //������� �� ���� ���  �� 1,8 �������
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
	
	currCounter = LL_TIM_GetCounter(TIM3); //������� ��������� ��������
	HandleEncData->currCounter_SetAngle = (32767 - ((currCounter-1) & 0xFFFF))/2; //������� �� 2, ���� ������� (������ = 2 ��������)
	
	if(HandleEncData->currCounter_SetAngle != HandleEncData->prevCounter_SetAngle) //���� ������� �������� �������� �� ����� �����������
	{
		delta = (HandleEncData->currCounter_SetAngle - HandleEncData->prevCounter_SetAngle); //������� ����� ������� � ���������� ���������� ��������
    HandleEncData->prevCounter_SetAngle = HandleEncData->currCounter_SetAngle; //���������� �������� ���������� ��������    
    if((delta > -20) && (delta < 20)) // ������ �� �������� ��������� � ������������ �������� (������������ ����� ��������� ����� �����)
		{
			if (delta != 0) //���� ���������� ��������� ��������
			{  
				snprintf (LCD_buff, sizeof(LCD_buff), "delta=%d", delta);
				ssd1306_PutData (kord_X, kord_Y, LCD_buff, DISP_CLEAR); //1 ������ �������
				
				HandleAng->set_minute += delta; //���������� �������
				if (HandleAng->set_minute <= 0) //���� �������� ����� ������ 0
				{			 
					if ((HandleAng->set_degree > 0) ) //���� �������� �������� ������ 0
					{ 
						HandleAng->set_degree--; //��������� ������ �� �������
						HandleAng->set_minute = 59;
					}
					else
					{	
						if (HandleAng->set_degree == 0)
						{
							if (HandleAng->set_minute <= 0) //���� �������� �������� ����� 0
							{	HandleAng->set_minute = 1;	} //��������� 1 � �������� �����
						}
					}
				}
				else
				{
					if (HandleAng->set_minute > 59) //���� ����� ������ 59
					{
						HandleAng->set_degree++; 				//���������� �������� �������� �� �������
						if (HandleAng->set_degree > 359) 
						{	HandleAng->set_degree = 0;	}
						HandleAng->set_minute = 0;
					}	
				}				
			}
		}
		snprintf (LCD_buff, sizeof(LCD_buff), "%03d* %02d' %02d\" edit", HandleAng->set_degree, 
		HandleAng->set_minute, HandleAng->set_second);
		ssd1306_PutData (kord_X, kord_Y+1, LCD_buff, DISP_NOT_CLEAR); //2 ������ �������
	}
}

//---------------------------------------------------------------------------------------------------//
void enc_shaft_rotation (angular_data_t * HandleAng, encoder_data_t * HandleEncData) 
{
	int32_t currCounter=0;
	currCounter= LL_TIM_GetCounter(TIM3); //������� ��������� ��������
	HandleEncData->currCounter_ShaftRotation= (32767 - ((currCounter-1) & 0xFFFF))/2; //������� �� 2, ���� ������� (������ = 2 ��������)
	int32_t delta = 0;
	
	if(HandleEncData->currCounter_ShaftRotation != HandleEncData->prevCounter_ShaftRotation) //���� ��������� �������� ����������
	{
		delta = (HandleEncData->currCounter_ShaftRotation - HandleEncData->prevCounter_ShaftRotation); //������� ����� ������� � ���������� ���������� ��������
    HandleEncData->prevCounter_ShaftRotation = HandleEncData->currCounter_ShaftRotation; //���������� �������� ��������� ��������    
		if((delta > -20) && (delta < 20)) // ������ �� �������� ��������� � ������������ �������� 
		{
			if (delta != 0) //���� ���������� ��������� ��������
			{ 
				SetAngle_in_Seconds (HandleAng); //������� ������� ������ ���� � �������
				
				if (delta > 0) //���� ������� ���������� �� ������� �������
				{
					HandleEncData->flag_DirShaftRotation++; //���������� �������� ����� �������������
					if (HandleEncData->flag_DirShaftRotation>1) 
					{ HandleEncData->flag_DirShaftRotation = 1;	}

					if (HandleEncData->flag_DirShaftRotation > 0) //���� �������� ����� ������������� �������������
					{
						while (delta > 0) //���� ������ ������ ����
						{
							need_step = HandleAng->StepAngleInSec/step_unit; //���������� ����������� �����
							step_angle (FORWARD, need_step); //������� �� ������� �������
							delta--;  //���������� ������
						}						
						
						HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec + HandleAng->StepAngleInSec;
						if (HandleAng->ShaftAngleInSec > (CIRCLE_IN_SEC-1)) //���� ���� ��������� ���� 360 ��. ��� ������
						{	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec - CIRCLE_IN_SEC;	} 
						
						GetAngleShaft_from_Seconds(HandleAng); //����������� �������� ���� ���� � �������� � ������ ��/���/���
						
						snprintf (LCD_buff, sizeof(LCD_buff), "%03d* %02d' %02d\" f", HandleAng->shaft_degree, 
						HandleAng->shaft_minute, HandleAng->shaft_second);	
						ssd1306_PutData (kord_X, kord_Y, LCD_buff, DISP_CLEAR);	
						snprintf (LCD_buff, sizeof(LCD_buff), "step=%d", need_step);
						ssd1306_PutData (kord_X, kord_Y+1, LCD_buff, DISP_NOT_CLEAR);	
					}
				}
				else
				{
					if (delta < 0) //���� ������� ���������� ������ ������� �������
					{
						HandleEncData->flag_DirShaftRotation--; //���������� �������� ����� �������������
						if (HandleEncData->flag_DirShaftRotation<(-1))
						{ HandleEncData->flag_DirShaftRotation = -1;	}
		
						if (HandleEncData->flag_DirShaftRotation < 0) //���� �������� ����� ������������� �������������
						{
							while (delta < 0) //���� ������ ������ ����
							{
								need_step = HandleAng->StepAngleInSec/step_unit; //���������� ����������� �����
								step_angle (BACKWARD, need_step); //������� ������ ������� �������
								delta++; //���������� ������
							}							
							if (HandleAng->ShaftAngleInSec >= HandleAng->StepAngleInSec) //���� ���� ��������� ���� ������ ���� ����
							{	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec - HandleAng->StepAngleInSec; }
							else 
							{
								if (HandleAng->ShaftAngleInSec < HandleAng->StepAngleInSec) //���� ���� ��������� ���� ������ ���� ����
								{	HandleAng->ShaftAngleInSec = CIRCLE_IN_SEC - (HandleAng->StepAngleInSec - HandleAng->ShaftAngleInSec); }	//������� � ������ 359��. �����.
							}
							GetAngleShaft_from_Seconds(HandleAng); //����������� �������� ���� ���� � �������� � ������ ��/���/���
							snprintf (LCD_buff, sizeof(LCD_buff), "%03d* %02d' %02d\" b", HandleAng->shaft_degree, 
							HandleAng->shaft_minute, HandleAng->shaft_second);	
							ssd1306_PutData (kord_X, kord_Y, LCD_buff, DISP_CLEAR);	
							snprintf (LCD_buff, sizeof(LCD_buff), "step=%d", need_step);
							ssd1306_PutData (kord_X, kord_Y+1, LCD_buff, DISP_NOT_CLEAR);	
						}
					}
				}
				angle_to_EEPROMbuf (HandleAng, eeprom_tx_buffer); //������� ������ ���� � ������
				EEPROM_WriteBytes (MEMORY_PAGE_ANGLE_ROTATION, eeprom_tx_buffer, 8); //������ ������� � ������� ���� �������� � ������					
			}
			else
			{	delta = 0;	}
		}
	}
}


//---------------------------------------------------------------------------------------------------//
void rbt_shaft_rotation (angular_data_t * HandleAng) 
{
	SetAngle_in_Seconds (HandleAng); //������� ������� ������ ���� � �������
	need_step = HandleAng->StepAngleInSec/step_unit; //���������� ����������� �����
	step_angle (FORWARD, need_step); //������� �� ������� �������
	
	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec + HandleAng->StepAngleInSec;
	if (HandleAng->ShaftAngleInSec > (CIRCLE_IN_SEC-1)) //���� ���� ��������� ���� 360 ��. ��� ������
	{	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec - CIRCLE_IN_SEC;	} 
	
	GetAngleShaft_from_Seconds(HandleAng); //����������� �������� ���� ���� � �������� � ������ ��/���/���
	angle_to_EEPROMbuf (HandleAng, eeprom_tx_buffer); //������� ������ ���� � ������
	EEPROM_WriteBytes (MEMORY_PAGE_ANGLE_ROTATION, eeprom_tx_buffer, 8); //������ ������� � ������� ���� �������� � ������					
}

//---------------------------------------------------------------------------------------------------//
void lbt_shaft_rotation (angular_data_t * HandleAng) 
{
	SetAngle_in_Seconds (HandleAng); //������� ������� ������ ���� � �������
	need_step = HandleAng->StepAngleInSec/step_unit; //���������� ����������� �����
	step_angle (BACKWARD, need_step); //������� ������ ������� �������
	
	if (HandleAng->ShaftAngleInSec >= HandleAng->StepAngleInSec) //���� ���� ��������� ���� ������ ���� ����
	{	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec - HandleAng->StepAngleInSec; }
	else 
	{
		if (HandleAng->ShaftAngleInSec < HandleAng->StepAngleInSec) //���� ���� ��������� ���� ������ ���� ����
		{	HandleAng->ShaftAngleInSec = CIRCLE_IN_SEC - (HandleAng->StepAngleInSec - HandleAng->ShaftAngleInSec); }	//������� � ������ 359��. �����.
	}
	GetAngleShaft_from_Seconds(HandleAng); //����������� �������� ���� ���� � �������� � ������ ��/���/���
	angle_to_EEPROMbuf (HandleAng, eeprom_tx_buffer); //������� ������ ���� � ������
	EEPROM_WriteBytes (MEMORY_PAGE_ANGLE_ROTATION, eeprom_tx_buffer, 8); //������ ������� � ������� ���� �������� � ������					
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
	step_angle (BACKWARD, need_step); //������� ������ ������� �������
	
	HandleAng->ShaftAngleInSec = 0;
	GetAngleShaft_from_Seconds(HandleAng); //����������� �������� ���� ���� � �������� � ������ ��/���/���
	angle_to_EEPROMbuf (HandleAng, eeprom_tx_buffer); //������� ������ ���� � ������
	EEPROM_WriteBytes (MEMORY_PAGE_ANGLE_ROTATION, eeprom_tx_buffer, 8); //������ ������� � ������� ���� �������� � ������
}

//---------------------------------------------------------------------------------------------------//
void rbt_rotate_to_zero (angular_data_t * HandleAng) 
{
	need_step = ((CIRCLE_IN_SEC - HandleAng->ShaftAngleInSec)/step_unit);
	step_angle (FORWARD, need_step); //������� ������ ������� �������
	
	HandleAng->ShaftAngleInSec = 0;
	GetAngleShaft_from_Seconds(HandleAng); //����������� �������� ���� ���� � �������� � ������ ��/���/���
	angle_to_EEPROMbuf (HandleAng, eeprom_tx_buffer); //������� ������ ���� � ������
	EEPROM_WriteBytes (MEMORY_PAGE_ANGLE_ROTATION, eeprom_tx_buffer, 8); //������ ������� � ������� ���� �������� � ������
}

