
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
__IO float step_unit = ((float)STEP18_IN_SEC/(float)STEP_DIV); //���������� ������ � ����� ���������(1,8��/100=6480/100=64,8)

//----------------------------------------------------------------------------------------------------//
void rotate_step (uint8_t micro_step)
{
	for (uint8_t count = 0; count < micro_step; count++) //���������� ���������� (���������)
	{
			STEP(ON);
			delay_us (4);
			STEP(OFF);
			delay_us (4); //
	}
}

//----------------------------------------------------------------------------------------------------//
void step_angle (uint8_t dir, uint32_t need_step)
{
	uint32_t quant = 0;
	DIR_DRIVE (dir);
	delay_us (6);	
	if ((quant = (uint32_t)need_step/STEP_DIV) > 0) //���������� ����� ����� (1,8 ��.)
	{
		for (uint32_t count = 0; count < quant; count++) //
		{
			DRIVE_ENABLE(ON);
			delay_us (5);
			rotate_step (STEP_DIV); //������� �� ���� ��� (1,8 ��., 100 ����������)
			DRIVE_ENABLE(OFF);
			delay_us (1500); //�������� 1500 ���
		}
	}
		
	if ((quant = need_step%STEP_DIV) > 0) //������� ���������� (<1,8 ��)
	{
		DRIVE_ENABLE(ON);
		delay_us (5);
		rotate_step (quant);
		DRIVE_ENABLE(OFF);
		delay_us (3);
	}
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
				HandleAng->StepAngleInSec += (delta*SECOND_PER_MINUTE);
				if (HandleAng->StepAngleInSec < SECOND_PER_MINUTE)
				{ HandleAng->StepAngleInSec = SECOND_PER_MINUTE; }
				else
				{
					if (HandleAng->StepAngleInSec > (CIRCLE_IN_SEC-1))
					{ HandleAng->StepAngleInSec = (CIRCLE_IN_SEC-1); }
				}
				GetSetAngle_from_Seconds (HandleAng); //������� ���� ���� ���� ���� �� ������ � ������ ��/���/�
				setangle_mode_screen (HandleAng); //����� ���������� �� �������
			}
		}
	}				
}

//----------------------------------------------------------------------------------------------------//
void set_teeth_gear (milling_data_t * HandleMil, encoder_data_t * HandleEncData) 
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
				HandleMil->teeth_gear_numbers += delta;
				if (HandleMil->teeth_gear_numbers > 0xFE) //���������� ������ �� ������ 254
				{	HandleMil->teeth_gear_numbers = 0x02;	}	//����� �� ����������� ��������
				else
				{
					if (HandleMil->teeth_gear_numbers < 0x02) //� �� ������ 2
					{	HandleMil->teeth_gear_numbers = 0xFE;	} //����� �� ������������ ��������
				}	
				setteeth_mode_screen (HandleMil); //�������� ������� - ����� ����� ����������� ������ ����������
			}
		}
	}
}

//---------------------------------------------------------------------------------------------------//
void encoder_reset (encoder_data_t * HandleEncData) 
{
	int32_t encCounter = 0; //���������� ��� �������� ������ ��������
	encCounter = LL_TIM_GetCounter(TIM3); //���������� �������� ��������� ��������
	HandleEncData->currCounter_SetAngle = (32767 - ((encCounter-1) & 0xFFFF))/2; //�������������� ����������� ��������� �������� � ������ �� -10 �� 10
	HandleEncData->prevCounter_SetAngle = HandleEncData->currCounter_SetAngle; //���������� ���������������� �������� ��������� �������� � ��������� ��������� ���� ��������
	HandleEncData->prevCounter_ShaftRotation = HandleEncData->currCounter_SetAngle; //���������� ���������������� �������� ��������� �������� � ��������� ������ ��������� ����
}

//---------------------------------------------------------------------------------------------------//
void enc_shaft_rotation (angular_data_t * HandleAng, encoder_data_t * HandleEncData) 
{
	uint32_t need_step = 0; //���������� ����������� ����������(���������)
	int32_t currCounter=0;
	int32_t delta = 0;
	uint8_t dir = 2;
	
	currCounter= LL_TIM_GetCounter(TIM3); //������� ��������� ��������
	HandleEncData->currCounter_ShaftRotation= (32767 - ((currCounter-1) & 0xFFFF))/2; //������� �� 2, ���� ������� (������ = 2 ��������)
	
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
					while (delta > 0) //���� ������ ������ ����
					{
						//need_step = (uint32_t)(REDUCER*(HandleAng->StepAngleInSec/step_unit));
						need_step = calc_steps (HandleAng->StepAngleInSec, step_unit);
						step_angle (FORWARD, need_step); //������� �� ������� �������
						delta--;  //���������� ������
					}											
					HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec + HandleAng->StepAngleInSec;
					if (HandleAng->ShaftAngleInSec > (CIRCLE_IN_SEC-1)) //���� ���� ��������� ���� 360 ��. ��� ������
					{	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec - CIRCLE_IN_SEC;	} 
					dir = FORWARD;
				}
				else
				{
					if (delta < 0) //���� ������� ���������� ������ ������� �������
					{	
						while (delta < 0) //���� ������ ������ ����
						{
							//need_step = (uint32_t)(REDUCER*(HandleAng->StepAngleInSec/step_unit));
							need_step = calc_steps (HandleAng->StepAngleInSec, step_unit);
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
							dir = BACKWARD;
					}
				}
				angle_to_EEPROMbuf (HandleAng, eeprom_tx_buffer); //������� ������ ���� � ������
				EEPROM_WriteBytes (MEMORY_PAGE_ANGLE_ROTATION, eeprom_tx_buffer, (EEPROM_NUMBER_BYTES-6)); //������ ������� � ������� ���� �������� � ������					
				GetAngleShaft_from_Seconds(HandleAng); //����������� �������� ���� ���� � �������� � ������ ��/���/���
				shaft_rotation_screen (HandleAng, need_step, dir);
			}
			else
			{	delta = 0;	}
		}
	}
}


//---------------------------------------------------------------------------------------------------//
void right_shaft_rotation (angular_data_t * HandleAng) 
{
	uint32_t need_step = 0; //���������� ����������� ����������(���������)
	SetAngle_in_Seconds (HandleAng); //������� ������� ������ ���� � �������
//	need_step = HandleAng->StepAngleInSec/step_unit; //������� ������� ���������� ��� ����� ���������
	need_step = (uint32_t)(REDUCER*((float)HandleAng->StepAngleInSec/step_unit)); //���������� ����������� ����������
	step_angle (FORWARD, need_step); //������� �� ������� �������
	
	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec + HandleAng->StepAngleInSec;
	if (HandleAng->ShaftAngleInSec > (CIRCLE_IN_SEC-1)) //���� ���� ��������� ���� 360 ��. ��� ������
	{	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec - CIRCLE_IN_SEC;	} 
	
	GetAngleShaft_from_Seconds(HandleAng); //����������� �������� ���� ���� � �������� � ������ ��/���/���
	angle_to_EEPROMbuf (HandleAng, eeprom_tx_buffer); //������� ������ ���� � ������
	EEPROM_WriteBytes (MEMORY_PAGE_ANGLE_ROTATION, eeprom_tx_buffer, 8); //������ ������� � ������� ���� �������� � ������					
}

//---------------------------------------------------------------------------------------------------//
void left_shaft_rotation (angular_data_t * HandleAng) 
{
	uint32_t need_step = 0; //���������� ����������� ����������(���������)
	SetAngle_in_Seconds (HandleAng); //������� ������� ������ ���� � �������
//	need_step = HandleAng->StepAngleInSec/step_unit; //������� ������� ���������� ��� ����� ���������
	need_step = (uint32_t)(REDUCER*((float)HandleAng->StepAngleInSec/step_unit)); //���������� ����������� ����������
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
	//step_angle (FORWARD, STEPS_IN_REV); //������ ������ �� 360 ��������
	step_angle (FORWARD, (STEPS_IN_REV*REDUCER));  //������ ������ �� 360�� � ������ �������� ��������� (360��*40)
}

//---------------------------------------------------------------------------------------------------//
void left_rotate_to_zero (angular_data_t * HandleAng) 
{
	uint32_t need_step = 0; //���������� ����������� ����������(���������)
	
	//need_step = HandleAng->ShaftAngleInSec/step_unit; //������� ������� ���������� ��� ����� ���������
	need_step = (uint32_t)(REDUCER*((float)HandleAng->ShaftAngleInSec/step_unit));
	step_angle (BACKWARD, need_step); //������� ������ ������� �������
	
	HandleAng->ShaftAngleInSec = 0;
	GetAngleShaft_from_Seconds(HandleAng); //����������� �������� ���� ���� � �������� � ������ ��/���/���
	angle_to_EEPROMbuf (HandleAng, eeprom_tx_buffer); //������� ������ ���� � ������
	EEPROM_WriteBytes (MEMORY_PAGE_ANGLE_ROTATION, eeprom_tx_buffer, 8); //������ ������� � ������� ���� �������� � ������
}

//---------------------------------------------------------------------------------------------------//
void right_rotate_to_zero (angular_data_t * HandleAng) 
{
	uint32_t need_step = 0; //���������� ����������� ����������(���������)
	
	//need_step = ((CIRCLE_IN_SEC - HandleAng->ShaftAngleInSec)/step_unit); //������� ������� ���������� ��� ����� ���������
	need_step = (uint32_t)(REDUCER*((float)(CIRCLE_IN_SEC - HandleAng->ShaftAngleInSec)/step_unit));
	step_angle (FORWARD, need_step); //������� �� ������� �������
	
	HandleAng->ShaftAngleInSec = 0; //����� �������� ���� ���� � ������� �������
	GetAngleShaft_from_Seconds(HandleAng); //����������� �������� ���� ���� � �������� � ������ ��/���/���
	angle_to_EEPROMbuf (HandleAng, eeprom_tx_buffer); //������� ������ ���� � ������
	EEPROM_WriteBytes (MEMORY_PAGE_ANGLE_ROTATION, eeprom_tx_buffer, 8); //������ ������� � ������� ���� �������� � ������
}

//---------------------------------------------------------------------------------------------------//
void right_teeth_rotation (milling_data_t * HandleMil, angular_data_t * HandleAng) 
{
	uint32_t need_step = 0; //���������� ����������� ����������(���������)
	
	if (HandleMil->remain_teeth_gear > 0)
	{
		HandleMil->remain_teeth_gear--; //���������� �� 1 ����������� ���������� ������
	//	need_step = ((HandleMil->AngleTeethInSec)/step_unit); //������� �� ���� ���� ���� ������ ������� �������
		need_step = (uint32_t)(REDUCER*((float)HandleMil->AngleTeethInSec/step_unit)); //������� �� ���� ���� ���� ������ ������� ������� � ������ ���������
		step_angle (FORWARD, need_step); //������� �� ������� �������
		
		//�������� ������� ������ �������� ��������� ����
		HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec + HandleMil->AngleTeethInSec;
		if (HandleAng->ShaftAngleInSec > (CIRCLE_IN_SEC-1)) //���� ���� ��������� ���� 360 ��. ��� ������
		{	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec - CIRCLE_IN_SEC;	} //
	}
}

//---------------------------------------------------------------------------------------------------//
void left_teeth_rotation (milling_data_t * HandleMil, angular_data_t * HandleAng) 
{
	uint32_t need_step = 0; //���������� ����������� ����������(���������)
	
	if (HandleMil->remain_teeth_gear > 0)
	{
		HandleMil->remain_teeth_gear--; //���������� �� 1 ����������� ���������� ������
	//	need_step = ((HandleMil->AngleTeethInSec)/step_unit); //������� �� ���� ���� ���� ������ ������� �������
		need_step = (uint32_t)(REDUCER*((float)HandleMil->AngleTeethInSec/step_unit)); //������� �� ���� ���� ���� ������ ������� ������� � ������ ���������
		step_angle (BACKWARD, need_step); //������� ������ ������� �������
		
		if (HandleAng->ShaftAngleInSec >= HandleMil->AngleTeethInSec) //���� ���� ��������� ���� ������ ���� ����
		{	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec - HandleMil->AngleTeethInSec; }
		else 
		{
			if (HandleAng->ShaftAngleInSec < HandleMil->AngleTeethInSec) //���� ���� ��������� ���� ������ ���� ����
			{	HandleAng->ShaftAngleInSec = CIRCLE_IN_SEC - (HandleMil->AngleTeethInSec - HandleAng->ShaftAngleInSec); }	//������� � ������ 359��. �����.
		}
	}
}

//---------------------------------------------------------------------------------------------------//