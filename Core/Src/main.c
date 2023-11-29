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
#define MEMORY_PAGE_SHAFT_POSITION 0x0002
#define STEP_UNIT 0.324
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
char LCD_buff[20];

angular_data_t angular_rotation;
angular_data_t shaft_angle;

__IO int32_t prevCounter = 0;
__IO int32_t currCounter = 0;

__IO uint16_t key_code = NO_KEY;
__IO uint32_t need_step = 0;

uint8_t LCD_X = LCD_DEFAULT_X_SIZE; 
uint8_t LCD_Y = LCD_DEFAULT_Y_SIZE;

uint8_t eeprom_rx_buffer[20] = {0};
uint8_t eeprom_tx_buffer[10];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void set_angle(angular_data_t * handle);												
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
	tim_delay_init ();
	HAL_Delay(100);
	
	ssd1306_Init();
	ssd1306_Clear();
	
	EEPROM_ReadBytes (MEMORY_PAGE_ANGLE_ROTATION, eeprom_rx_buffer, 8); //��������� ������� ������ ��������� �������� �� EEPROM
	init_angular_data (&angular_rotation); //������������� ������� ������ ��������� ��������
	angle_from_EEPROMbuf (&angular_rotation, eeprom_rx_buffer); //���������� ������� ������ ��������� ��������
	
	EEPROM_ReadBytes (MEMORY_PAGE_SHAFT_POSITION, eeprom_rx_buffer, 8);
	init_angular_data (&shaft_angle);
	angle_from_EEPROMbuf (&shaft_angle, eeprom_rx_buffer);
	
	snprintf (LCD_buff, sizeof(LCD_buff), "%03d* %02d' %02d\"", angular_rotation.degree, 
	angular_rotation.minute, angular_rotation.second);
	ssd1306_PutData (LCD_X, LCD_Y, LCD_buff, 1);

	DRIVE_ENABLE(OFF);
	STEP(OFF);
	
	HAL_Delay (100);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		for (uint8_t count = 0; count <0xFF; count++)
		{
			if ((key_code = scan_keys()) != NO_KEY)
			{		
				switch (key_code)
				{
					case NO_KEY:
						break;
					
					case KEY_LEFT:						
						angle_in_seconds (&angular_rotation);
						need_step = angular_rotation.AngleInSecond/STEP_UNIT; //���������� ����������� �����
						step_angle (BACKWARD, need_step);
					
						snprintf(LCD_buff, sizeof(LCD_buff), "back step=");
						ssd1306_PutData (LCD_X, LCD_Y, LCD_buff, 1);
						snprintf(LCD_buff, sizeof(LCD_buff), "%d", need_step);
						ssd1306_PutData (LCD_X, LCD_Y+1, LCD_buff, 0);
						angular_rotation.AngleInSecond = 0;
						break;
					
					case KEY_CENTER:
						break;
					
					case KEY_RIGHT:
						angle_in_seconds (&angular_rotation);
						need_step = angular_rotation.AngleInSecond/STEP_UNIT;
						step_angle (FORWARD, need_step);

						snprintf(LCD_buff, sizeof(LCD_buff), "forward step");
						ssd1306_PutData (LCD_X, LCD_Y, LCD_buff, 1);	
						snprintf(LCD_buff, sizeof(LCD_buff), "%d", need_step);
						ssd1306_PutData (LCD_X, LCD_Y+1, LCD_buff, 0);	
						angular_rotation.AngleInSecond = 0;
						break;
					
					case KEY_ENC:
						snprintf (LCD_buff, sizeof(LCD_buff), "%03d* %02d' %02d\" edit", angular_rotation.degree, angular_rotation.minute, angular_rotation.second);
						ssd1306_PutData (LCD_X, LCD_Y, LCD_buff, 1);
						while(1)
						{
							if ((key_code = scan_keys()) != KEY_ENC)
							{	set_angle(&angular_rotation);	} //������������ ��������� ��������
							else
							{	
								angle_to_EEPROMbuf (&angular_rotation, eeprom_tx_buffer); //������� ������ ���� � ������
								EEPROM_WriteBytes (MEMORY_PAGE_ANGLE_ROTATION, eeprom_tx_buffer, 8); //������ ������� � ������� ���� �������� � ������
								snprintf (LCD_buff, sizeof(LCD_buff), "%03d* %02d' %02d\"", angular_rotation.degree, 
								angular_rotation.minute, angular_rotation.second);
								ssd1306_PutData (LCD_X, LCD_Y, LCD_buff, 1);		
								break;	
							}
						}
						break;
					
					default:
						key_code = NO_KEY;
						break;					
				}
			}
		}
  }
  /* USER CODE END 3 */
}

//--------------------------------------------------------------------------------------------//
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
void set_angle(angular_data_t * handle) 
{
	currCounter = LL_TIM_GetCounter(TIM3); //������� ��������� ��������
	currCounter = (32767 - ((currCounter-1) & 0xFFFF))/2; //������� �� 2, ���� ������� (������ = 2 ��������)
	int32_t delta = 0;
	
	if(currCounter != prevCounter) 
	{
		delta = (currCounter-prevCounter); //������� ����� ������� � ���������� ���������� ��������
    prevCounter = currCounter; //���������� �������� ���������� ��������    
   // if((delta > -20) && (delta < 20)) // ������ �� �������� ��������� � ������������ �������� (������������ ����� ��������� ����� �����)
	
		if (delta != 0) //���� ���������� ��������� ��������
		{    
			handle->minute += delta; //���������� �������
			if (handle->minute < 0) //
			{			 
				if (handle->degree > 0)
				{ 
					handle->degree--; 
					handle->minute = 59;
				}
				else
				{	
					if (handle->degree == 0)
					{	handle->minute = 0;	}
				}
			}
			else
			{
				if (handle->minute > 59)
				{
					handle->degree++;
					if (handle->degree >= 360)
					{	handle->degree = 0;	}
					handle->minute=0;
				}				
			}
			snprintf (LCD_buff, sizeof(LCD_buff), "%03d* %02d' %02d\" edit", handle->degree, handle->minute, handle->second);
			ssd1306_PutData (LCD_X, LCD_Y, LCD_buff, 1);
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
