/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "typedef.h"
#include "ssd1306.h"
#include "button.h"
#include "drive.h"
#include "eeprom.h"
#include "angle_calc.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MEMORY_PAGE_ANGLE_ROTATION 0x0001
#define STEP_UNIT 0.324 //��� �������� � �������� � ������ �������� 20000
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
char LCD_buff[20];

angular_data_t curr_rotation = {0}; //��������� � �������� ������� �������
encoder_data_t encoder_data = {0}; //��������� � ������� ��������

__IO uint16_t key_code = NO_KEY;
__IO uint32_t need_step = 0;

uint8_t LCD_X = LCD_DEFAULT_X_SIZE; //����� ������� �� �����������
uint8_t LCD_Y = LCD_DEFAULT_Y_SIZE; //����� ������ ������� 

uint8_t eeprom_rx_buffer[20] = {0};
uint8_t eeprom_tx_buffer[10];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void set_angle (angular_data_t * , encoder_data_t *);			
void shaft_rotation (angular_data_t *, encoder_data_t * );
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM3_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
	encoder_init();
	timers_ini ();
	HAL_Delay(100);
	
	ssd1306_Init();
	ssd1306_Clear();
	
	EEPROM_ReadBytes (MEMORY_PAGE_ANGLE_ROTATION, eeprom_rx_buffer, 8); //��������� ������� ������ ��������� �������� �� EEPROM
	if ((eeprom_rx_buffer[0] != 0xFF) && (eeprom_rx_buffer[1] != 0xFF) && (eeprom_rx_buffer[2] != 0xFF)
			&& (eeprom_rx_buffer[3] != 0xFF) && (eeprom_rx_buffer[4] != 0xFF) && (eeprom_rx_buffer[5] != 0xFF)
			&& (eeprom_rx_buffer[6] != 0xFF) && (eeprom_rx_buffer[7] != 0xFF))
	{	init_angular_data (&curr_rotation, eeprom_rx_buffer); }//������������� � ����������� ������� ������ ��������� �������� � ���������
	ShaftAngle_in_Seconds (&curr_rotation); //������� ������� ������ ��������� ���� � �������
	
	snprintf (LCD_buff, sizeof(LCD_buff), "%03d* %02d' %02d\"", curr_rotation.shaft_degree, 
	curr_rotation.shaft_minute, curr_rotation.shaft_second);
	ssd1306_PutData (LCD_X, LCD_Y, LCD_buff, DISP_CLEAR);
	
	snprintf (LCD_buff, sizeof(LCD_buff), "%03d* %02d' %02d\"", curr_rotation.set_degree, 
	curr_rotation.set_minute, curr_rotation.set_second);
	ssd1306_PutData (LCD_X, LCD_Y+1, LCD_buff, DISP_NOT_CLEAR);
	
	DRIVE_ENABLE(OFF); //���������� �������
	STEP(OFF); //
	
	HAL_Delay (100);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
		shaft_rotation (&curr_rotation, &encoder_data); //��������� ������ �������� (���������� ��������� ����)
		
		if ((key_code = scan_keys()) != NO_KEY)
		{		
			switch (key_code)
			{
				case NO_KEY:
					break;
					
				case KEY_LEFT:		//������� ���� ������ ������� �������									
					snprintf(LCD_buff, sizeof(LCD_buff), "back step=%3d", need_step);
					ssd1306_PutData (LCD_X, LCD_Y, LCD_buff, DISP_CLEAR);
					
					snprintf (LCD_buff, sizeof(LCD_buff), "%03d* %02d' %02d\"", curr_rotation.set_degree, 
					curr_rotation.set_minute, curr_rotation.set_second);
					ssd1306_PutData (LCD_X, LCD_Y+1, LCD_buff, DISP_NOT_CLEAR);		

					TimerDisplayStart ();
					break;
					
				case KEY_CENTER_SHORT: //
					display_default_screen (LCD_buff, &curr_rotation);	
					break;
						
				case KEY_CENTER_LONG:	//������� ��������� ���� - �������				
					snprintf (LCD_buff, sizeof(LCD_buff), "key_enc_long");
					ssd1306_PutData (LCD_X, LCD_Y, LCD_buff, DISP_CLEAR);
					
					snprintf (LCD_buff, sizeof(LCD_buff), "%03d* %02d' %02d\"", curr_rotation.set_degree, 
					curr_rotation.set_minute, curr_rotation.set_second);
					ssd1306_PutData (LCD_X, LCD_Y+1, LCD_buff, DISP_NOT_CLEAR);
					
					curr_rotation.shaft_second = 0; //������� ��������� ���� - �������
					curr_rotation.shaft_minute = 0; 
					curr_rotation.shaft_degree = 0;
					
					angle_to_EEPROMbuf (&curr_rotation, eeprom_tx_buffer); //������� ������ ���� � ������
					EEPROM_WriteBytes (MEMORY_PAGE_ANGLE_ROTATION, eeprom_tx_buffer, 8);
					break;
						
						
				case KEY_RIGHT:  //������� ���� �� ������� �������			
					snprintf(LCD_buff, sizeof(LCD_buff), "forward_step=%3d", need_step);
					ssd1306_PutData (LCD_X, LCD_Y, LCD_buff, DISP_CLEAR);
					
					snprintf (LCD_buff, sizeof(LCD_buff), "%03d* %02d' %02d\"", curr_rotation.set_degree, 
					curr_rotation.set_minute, curr_rotation.set_second);
					ssd1306_PutData (LCD_X, LCD_Y+1, LCD_buff, DISP_NOT_CLEAR);	

					TimerDisplayStart ();
					break;
					
				case KEY_ENC_SHORT: //��������� ���� ��������
					snprintf (LCD_buff, sizeof(LCD_buff), "%03d* %02d' %02d\" edit", curr_rotation.set_degree, 
					curr_rotation.set_minute, curr_rotation.set_second);
					ssd1306_PutData (LCD_X, LCD_Y+1, LCD_buff, DISP_CLEAR);
					while(1)
					{
						if ((key_code = scan_keys()) != KEY_ENC_SHORT) //�������� ���������� ������� ������
						{	set_angle(&curr_rotation, &encoder_data);	} //��������� ������ �������� (��������� ���� ��������)
						else
						{	
							angle_to_EEPROMbuf (&curr_rotation, eeprom_tx_buffer); //������� ������ ���� � ������
							EEPROM_WriteBytes (MEMORY_PAGE_ANGLE_ROTATION, eeprom_tx_buffer, 8); //������ ������� � ������� ���� �������� � ������					
							display_default_screen (LCD_buff, &curr_rotation);							
							break;	
						}
					}
					break;
					
				case KEY_ENC_LONG: //����� ���� ��������� ���� �� ������������ ��������
					curr_rotation.set_second = 0;
					curr_rotation.set_minute = 1; 
					curr_rotation.set_degree = 0;
				
					snprintf (LCD_buff, sizeof(LCD_buff), "key_center_long");
					ssd1306_PutData (LCD_X, LCD_Y, LCD_buff, DISP_CLEAR);
					
					snprintf (LCD_buff, sizeof(LCD_buff), "%03d* %02d' %02d\" edit", curr_rotation.set_degree, 
					curr_rotation.set_minute, curr_rotation.set_second);
					ssd1306_PutData (LCD_X, LCD_Y+1, LCD_buff, DISP_NOT_CLEAR);
					break;
					
				default:
					key_code = NO_KEY;
					break;					
			}
		}
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
//--------------------------------------------------------------------------------------------//
void set_angle (angular_data_t * HandleAng, encoder_data_t * HandleEncData) 
{
	int32_t currCounter=0;
	int32_t delta = 0;
	currCounter = LL_TIM_GetCounter(TIM3); //������� ��������� ��������
	HandleEncData->currCounter_SetAngle = (32767 - ((currCounter-1) & 0xFFFF))/2; //������� �� 2, ���� ������� (������ = 2 ��������)
	
	if(HandleEncData->currCounter_SetAngle != HandleEncData->prevCounter_SetAngle) 
	{
		delta = (HandleEncData->currCounter_SetAngle - HandleEncData->prevCounter_SetAngle); //������� ����� ������� � ���������� ���������� ��������
    HandleEncData->prevCounter_SetAngle = HandleEncData->currCounter_SetAngle; //���������� �������� ���������� ��������    
    if((delta > -20) && (delta < 20)) // ������ �� �������� ��������� � ������������ �������� (������������ ����� ��������� ����� �����)
		{
			if (delta != 0) //���� ���������� ��������� ��������
			{  
				snprintf (LCD_buff, sizeof(LCD_buff), "delta=%d", delta);
				ssd1306_PutData (LCD_X, LCD_Y, LCD_buff, DISP_CLEAR); //1 ������ �������
				
				HandleAng->set_minute += delta; //���������� �������
				if (HandleAng->set_minute < 0) //���� �������� ����� ������ 0
				{			 
					if (HandleAng->set_degree > 0) //���� �������� �������� ������ 0
					{ 
						HandleAng->set_degree--; //��������� ������ �� �������
						HandleAng->set_minute = 59;
					}
					else
					{	
						if (HandleAng->set_degree == 0) //���� �������� �������� ����� 0
						{	HandleAng->set_minute = 1;	} //��������� 1 � �������� �����
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
		ssd1306_PutData (LCD_X, LCD_Y+1, LCD_buff, DISP_NOT_CLEAR); //2 ������ �������
	}
}

//--------------------------------------------------------------------------------------------//
void shaft_rotation (angular_data_t * HandleAng, encoder_data_t * HandleEncData) 
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
						snprintf (LCD_buff, sizeof(LCD_buff),  "delta=%d", delta);
						ssd1306_PutData (LCD_X, LCD_Y, LCD_buff, DISP_CLEAR);

						while (delta > 0) //���� ������ ������ ����
						{
							need_step = HandleAng->StepAngleInSec/STEP_UNIT; //���������� ����������� �����
							step_angle (FORWARD, need_step); //������� �� ������� �������
							delta--;  //���������� ������
						}
						
						snprintf (LCD_buff, sizeof(LCD_buff),  "forwardstep=%3d", need_step);
						ssd1306_PutData (LCD_X, LCD_Y+1, LCD_buff, DISP_NOT_CLEAR);
						HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec + HandleAng->StepAngleInSec;
						if (HandleAng->ShaftAngleInSec > (MAX_SEC-1)) //���� ���� ��������� ���� 360 ��. ��� ������
						{	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec - MAX_SEC;	} 
						GetAngle_from_Seconds (HandleAng); //����������� �������� ���� ���� � �������� � ������ ��/���/���
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
							snprintf (LCD_buff, sizeof(LCD_buff),  "delta=%d", delta);
							ssd1306_PutData (LCD_X, LCD_Y, LCD_buff, DISP_CLEAR); //1 ������ �������
							while (delta < 0) //���� ������ ������ ����
							{
								need_step = HandleAng->StepAngleInSec/STEP_UNIT; //���������� ����������� �����
								step_angle (BACKWARD, need_step); //������� ������ ������� �������
								delta++; //���������� ������
							}							
							snprintf (LCD_buff, sizeof(LCD_buff),  "backstep=%3d", need_step);
							ssd1306_PutData (LCD_X, LCD_Y+1, LCD_buff, DISP_NOT_CLEAR);	//2 ������ �������
							
							if (HandleAng->ShaftAngleInSec > HandleAng->StepAngleInSec) //���� ���� ��������� ���� ������ ���� ����
							{	HandleAng->ShaftAngleInSec = HandleAng->ShaftAngleInSec - HandleAng->StepAngleInSec; }
							else 
							{
								if (HandleAng->ShaftAngleInSec < HandleAng->StepAngleInSec) //���� ���� ��������� ���� ������ ���� ����
								{	HandleAng->ShaftAngleInSec =  MAX_SEC - (HandleAng->StepAngleInSec - HandleAng->ShaftAngleInSec); }	
								else
								{
									if (HandleAng->ShaftAngleInSec == HandleAng->StepAngleInSec) //���� ���� ��������� ���� ����� ���� ����
									{	HandleAng->ShaftAngleInSec = HandleAng->StepAngleInSec; }
								}
							}
							GetAngle_from_Seconds (HandleAng); //����������� �������� ���� ���� � �������� � ������ ��/���/���
						}
					}
				}
			}
			else
			{	delta = 0;	}
		}
	}
}

//--------------------------------------------------------------------------------------------//
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
