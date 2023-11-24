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
#include "ssd1306.h"
#include "ssd1306_interface.h"
#include "button.h"
#include "drive.h"
#include "eeprom.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
char DBG_buffer [30];
char LCD_buff[20];

__IO uint16_t degree = 0; 
__IO uint8_t minute = 0; 
__IO uint8_t second = 0;

__IO int32_t prevCounter = 0;
__IO int32_t currCounter = 0;

__IO uint16_t key_code = NO_KEY;

uint8_t LCD_X = LCD_DEFAULT_X_SIZE; 
uint8_t LCD_Y = LCD_DEFAULT_Y_SIZE;

uint8_t rd_value[20] = {0};
uint8_t wr_value[20] = {0xCA,0xFE,0xDE,0xAD,0x10,
                        0x0F,0x0E,0x0D,0x0C,0x0B,
                        0x0A,0x09,0x08,0x07,0x06,
                        0x05,0x04,0x03,0x02,0x01};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void loop(void);
extern void EEPROM_WriteBytes	(uint16_t, uint8_t*, uint16_t);
extern void EEPROM_ReadBytes	(uint16_t, uint8_t*, uint16_t);
												
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
	
	SSD1306_Init();
	ssd1306_Clear();
	
	ssd1306_Goto(LCD_X, LCD_Y);
	sprintf (LCD_buff, "%02d, %02d, %02d",  degree, minute, second);
	ssd1306_PutString(LCD_buff);
	HAL_Delay (20);
	
	EEPROM_WriteBytes (0x0000, wr_value, 20);
	HAL_Delay (50);
	EEPROM_WriteBytes (0x0001, wr_value, 20);

	/*ssd1306_Goto(LCD_X, LCD_Y+1);
	sprintf (LCD_buff, "_");
	ssd1306_PutString(LCD_buff);*/
	//while(SSD1306_IsReady() == 0){}				
		
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
		//	loop() ;
			if ((key_code = scan_keys()) != NO_KEY)
			{		
				switch (key_code)
				{
					case NO_KEY:
						break;
					
					case KEY_LEFT:						
						one_step (BACKWARD);	
						ssd1306_Clear();	
						ssd1306_Goto(LCD_DEFAULT_X_SIZE, LCD_DEFAULT_Y_SIZE);
						snprintf(LCD_buff, sizeof(LCD_buff), "key=%03d", key_code);
						ssd1306_PutString(LCD_buff);
						break;
					
					case KEY_CENTER:
					//	rd_value[0]=rd_value[1]=rd_value[2]=0x1;
						//EEPROM_WriteBytes (0x00A7, wr_value, 10);
					//	EEPROM_WriteBytes (0x0008, wr_value, 10);
						//HAL_Delay (20);
						for (uint8_t count = 0x00; count<0x03; count++)
							EEPROM_ReadBytes (count, rd_value, 20);
						
						//ssd1306_Clear();	
						//ssd1306_Goto(LCD_DEFAULT_X_SIZE, LCD_DEFAULT_Y_SIZE);
						//snprintf(LCD_buff, sizeof(LCD_buff), "data=%x%x%x", rd_value[0], rd_value[1], rd_value[2]);
					//	ssd1306_PutString(LCD_buff);
						break;
					
					case KEY_RIGHT:
						one_step (FORWARD);
						ssd1306_Clear();	
						ssd1306_Goto(LCD_DEFAULT_X_SIZE, LCD_DEFAULT_Y_SIZE);
						snprintf(LCD_buff, sizeof(LCD_buff), "key=%03d", key_code);
						ssd1306_PutString(LCD_buff);
						break;
					
					case KEY_ENC:
						break;
					
					default:
						break;					
				}
			/*	ssd1306_Clear();	
				ssd1306_Goto(LCD_DEFAULT_X_SIZE, LCD_DEFAULT_Y_SIZE);
				snprintf(LCD_buff, sizeof(LCD_buff), "key=%03d", key_code);
				ssd1306_PutString(LCD_buff);
				key_code = NO_KEY;*/
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
void loop(void) 
{
	currCounter = LL_TIM_GetCounter(TIM3); //текущее показание энкодера
	currCounter = (32767 - ((currCounter-1) & 0x0FFFF))/2; //деление на 2, счёт щелчков (щелчок = 2 импульса)
	int32_t need_step = 0;
	int32_t delta = 0;
	
	if(currCounter != prevCounter) 
	{
		delta = (currCounter-prevCounter); //разница между текущим и предыдущим показанием энкодера
    prevCounter = currCounter; //сохранение текущего показанаия энкодера    
   // if((delta > -20) && (delta < 20)) // защита от дребезга контактов и переполнения счетчика (переполнение будет случаться очень редко)
	
		if (delta != 0)
		{    
		/*	ssd1306_Clear();	
			ssd1306_Goto(LCD_DEFAULT_X_SIZE, LCD_DEFAULT_Y_SIZE);
			snprintf(LCD_buff, sizeof(LCD_buff), "%06d %03d", currCounter, delta);
			ssd1306_PutString(LCD_buff);*/
			need_step = delta; //количество шагов
			if (delta > 0)
			{
				DRIVE_ENABLE(ON);
				delay_us (5);
				DIR_DRIVE (FORWARD);
				delay_us (5);
				while (need_step > 0)
				{
					STEP(ON);
					delay_us (3);
					STEP(OFF);
					delay_us (3);
					need_step--;
				}
				DRIVE_ENABLE(OFF);
				delay_us (2);
			}
			else
			{
				if (delta < 0)
				{
					DRIVE_ENABLE(ON);
					delay_us (5);
					DIR_DRIVE (BACKWARD);
					delay_us (5);
					while (need_step < 0)
					{
						STEP(ON);
						delay_us (3);
						STEP(OFF);
						delay_us (3);
						need_step++;
					}
					DRIVE_ENABLE(OFF);
					delay_us (2);
				}
			}
		}
	}
}

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
