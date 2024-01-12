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

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

angular_data_t curr_rotation = {0}; //структура с угловыми данными привода
encoder_data_t encoder_data = {0}; //структура с данными энкодера
milling_data_t milling_mode = {0};
STATUS_FLAG_t status_flag;

__IO uint16_t key_code = NO_KEY;
int32_t encCounter=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//---------------------------------------------------------------------------------=-------------------//
/*void vTimerDisplayCallback (xTimerHandle xTimer)
{
	display_default_screen (LCD_buff, &curr_rotation);
}*/

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
	
	EEPROM_ReadBytes (MEMORY_PAGE_ANGLE_ROTATION, eeprom_rx_buffer, 14); //получение угловых данных установки поворота из EEPROM
	if ((eeprom_rx_buffer[0] != 0xFF) && (eeprom_rx_buffer[1] != 0xFF) && (eeprom_rx_buffer[2] != 0xFF) && (eeprom_rx_buffer[3] != 0xFF) 
	&& (eeprom_rx_buffer[4] != 0xFF) && (eeprom_rx_buffer[5] != 0xFF) && (eeprom_rx_buffer[6] != 0xFF) && (eeprom_rx_buffer[7] != 0xFF))
	{	
		angle_from_EEPROMbuf (&curr_rotation, eeprom_rx_buffer); //инициализация и конвертация угловых данных установки поворота в структуру
		GetSetAngle_from_Seconds (&curr_rotation); 		//перевод угловых данных установки угла в формат гр/мин/с
		GetAngleShaft_from_Seconds (&curr_rotation);	//перевод угловых данных положения вала в формат гр/мин/с
		teeth_angle_from_EEPROMbuf (&milling_mode, eeprom_rx_buffer); //инициализация и конвертация угловых данных режима фрезировки
		MilAngleTeeth_from_Seconds (&milling_mode); //перевод угловых данных хода шага фрезировки в формат гр/мин/с
	}
	else
	{
		SetAngleReset (&curr_rotation);
		AngleShaftReset (&curr_rotation);
		MilAngleTeethReset (&milling_mode);
	}
	default_screen_mode1 (&curr_rotation); //заставка по умолчанию
	DRIVE_ENABLE(OFF); //отключение привода
	STEP(OFF); //
	
	status_flag.tool_mode = MODE_DEFAULT; //режим по умолчанию
	status_flag.right_flag = status_flag.left_flag = OFF;
	
	HAL_Delay (100);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		if ((key_code = scan_keys()) != NO_KEY) //если была нажата кнопка
		{
			if (status_flag.tool_mode == MODE_DEFAULT) //если активен первый режим
			{	
				switch (key_code) //обработка кода нажатой кнопки
				{					
					case KEY_LEFT:	//нажатие левой кнопки
					//	left_shaft_rotation (&curr_rotation) ; //установленный шаг против часовой стрелки 
						default_screen_mode1 (&curr_rotation); //заставка по умолчанию режима 1
						break;
					
					case KEY_CENTER_SHORT: //нажата центральная кнопка, переход в подрежим возврата вала в нулевую позицию
						while(1)
						{
							if ((key_code = scan_keys()) == NO_KEY) //пока не нажата какая-нибудь кнопка
							{	continue;	} //ожидание нажатия любой кнопки
							else
							{
								switch (key_code)
								{
									case KEY_LEFT:
										left_rotate_to_zero (&curr_rotation); //поворот против часовой позиции в нулевую позицию
										break;
								
									case KEY_CENTER_SHORT:
										one_full_turn(); //оборот на 360 градусов
										break;
								
									case KEY_RIGHT: 
										right_rotate_to_zero (&curr_rotation); //поворот по часовой позиции в нулевую позицию
										break;	

									default:
										key_code = NO_KEY; //выход из подрежима 
										break;									
								}
								default_screen_mode1 (&curr_rotation);	//заставка по умолчанию режима 1
								break;
							}
						}
						break;
						
					case KEY_CENTER_LONG:	
						AngleShaftReset (&curr_rotation); //сброс текущего положения вала						
						angle_to_EEPROMbuf (&curr_rotation, eeprom_tx_buffer); //cохранение в буфере EEPROM текущих данных угла вала
						EEPROM_WriteBytes (MEMORY_PAGE_ANGLE_ROTATION, eeprom_tx_buffer, 8); //запись буфера EEPROM
						default_screen_mode1 (&curr_rotation);
						break;
						
						
					case KEY_RIGHT: //нажатие правой кнопки
					//	right_shaft_rotation (&curr_rotation) ; //установленный шаг по часовой стрелки 
						default_screen_mode1 (&curr_rotation); //заставка по умолчанию режима 1
						break;
					
					case KEY_ENC_SHORT: //короткое нажатие кнопки энкодера - режим установки шага хода вала
						snprintf (LCD_buff, sizeof(LCD_buff), "%03d* %02d' %02d\" edit", curr_rotation.set_degree, 
						curr_rotation.set_minute, curr_rotation.set_second);
						ssd1306_PutData (kord_X, kord_Y+1, LCD_buff, DISP_CLEAR);
						encoder_data.prevCounter_SetAngle = encoder_data.prevCounter_ShaftRotation; //сброс измений показаний энкодера 
						while(1)
						{
							if ((key_code = scan_keys()) != KEY_ENC_SHORT) //пока не будет повторного нажатия кнопки энкодера
							{	set_angle(&curr_rotation, &encoder_data);	} //проверка показаний энкодера
							else
							{	
								angle_to_EEPROMbuf (&curr_rotation, eeprom_tx_buffer); 
								EEPROM_WriteBytes (MEMORY_PAGE_ANGLE_ROTATION, eeprom_tx_buffer, 8); 		
								default_screen_mode1 (&curr_rotation);
								encoder_data.prevCounter_ShaftRotation = encoder_data.prevCounter_SetAngle; //сброс измениний показаний энкодера 
								break;	
							}
						}
						break;
					
					case KEY_ENC_LONG: //длинное нажатие кнопки энкодера
						SetAngleReset (&curr_rotation); //сброс установленного угла поворота до минимального значения
						angle_to_EEPROMbuf (&curr_rotation, eeprom_tx_buffer); 
						EEPROM_WriteBytes (MEMORY_PAGE_ANGLE_ROTATION, eeprom_tx_buffer, 8); 		
						default_screen_mode1 (&curr_rotation);
						break;
				
					case KEY_MODE: 
						status_flag.tool_mode = MODE_MILLING; //режим - фрезеровка
						default_screen_mode2 (&milling_mode);	
						break;
			
					default:
						key_code = NO_KEY;
						break;	
				}
			}
			else
			{
				if (status_flag.tool_mode == MODE_MILLING) //если активен первый режим == MODE_MILLING) //режим фрезировка
				{
					switch (key_code)
					{	
						case KEY_LEFT: //нажатие левой кнопки
							if (status_flag.left_flag == ON)
							{
								left_teeth_rotation (&milling_mode, &curr_rotation); //поворот фрезы против часовой стрелке
								angle_to_EEPROMbuf (&curr_rotation, eeprom_tx_buffer);  //сохранение угловых данных положения вала
								remain_teeth_to_EEPROMbuf (&milling_mode, eeprom_tx_buffer); //сохранение оставшегося количества зубьев
								EEPROM_WriteBytes (MEMORY_PAGE_ANGLE_ROTATION, eeprom_tx_buffer, 9);
								default_screen_mode2 (&milling_mode);
							}
							else
							{
								if (status_flag.right_flag != ON)
								{	status_flag.left_flag = ON; }
							}
							break;
					
						case KEY_CENTER_SHORT: //переход в подрежим
							while(1)
							{
								if ((key_code = scan_keys()) == NO_KEY) //пока не нажата какая-нибудь кнопка
								{	continue;	} 	//крутимся в бесконечном цикле
								else
								{
									switch (key_code) //если кнопка была нажата
									{
										case KEY_LEFT:
											left_rotate_to_zero (&curr_rotation); //поворот против часовой позиции в нулевую позицию
											RemainTeethGearReset(&milling_mode); //сброс угловых настроек шестерёнки 
											status_flag.right_flag = status_flag.left_flag = OFF; //сброс флагов поворота
											break;							
								
										case KEY_RIGHT: 
											right_rotate_to_zero (&curr_rotation); //поворот по часовой позиции в нулевую позицию
											RemainTeethGearReset(&milling_mode); //сброс угловых настроек шестерёнки
											status_flag.right_flag = status_flag.left_flag = OFF; //сброс флагов поворота
											break;	

										default:
											key_code = NO_KEY;
											break;									
									}
									default_screen_mode2 (&milling_mode);	//заставка дисплея - режим 2
									break;
								}
							}
							break;
						
						case KEY_CENTER_LONG:	
							MilAngleTeethReset (&milling_mode); //сброс всех сохраненных данных фрезеровки
							default_screen_mode2 (&milling_mode);
							break;
											
						case KEY_RIGHT: //правая кнопка панели
							if (status_flag.right_flag == ON)
							{
								right_teeth_rotation	(&milling_mode, &curr_rotation); //поворот фрезы по часовой стрелке
								angle_to_EEPROMbuf (&curr_rotation, eeprom_tx_buffer);  //сохранение угловых данных положения вала
								remain_teeth_to_EEPROMbuf (&milling_mode, eeprom_tx_buffer); //сохранение оставшегося количества зубьев
								EEPROM_WriteBytes (MEMORY_PAGE_ANGLE_ROTATION, eeprom_tx_buffer, 9);
								default_screen_mode2 	(&milling_mode);
							}
							else
							{
								if (status_flag.left_flag != ON)
								{	status_flag.right_flag = ON; }
							}
							break;
					
						case KEY_ENC_SHORT: //короткое нажатие энкодера
							encoder_reset (&encoder_data); 	//сброс показаний энкодера						
							snprintf (LCD_buff, sizeof(LCD_buff), "set_number_teeth");
							ssd1306_PutData (kord_X, kord_Y, LCD_buff, DISP_CLEAR);	
							snprintf (LCD_buff, sizeof(LCD_buff), "%03d@ edit", milling_mode.teeth_gear_numbers);
							ssd1306_PutData (kord_X, kord_Y+1, LCD_buff, DISP_NOT_CLEAR);
						
							while(1) //режим установки количества зубов шестерёнки
							{
								if ((key_code = scan_keys()) != KEY_ENC_SHORT) //пока нет короткого нажатия кнопки энкодера
								{	set_teeth_gear(&milling_mode, &encoder_data);	} //опрос энкодера - установка количества зубьев шестерёнки
								else
								{	
									GetMilAngleTeeth (&milling_mode); //расчёт угла поворота после ввода количества зубов
									angle_to_EEPROMbuf (&curr_rotation, eeprom_tx_buffer); //сохранение текущего положения вала
									teeth_angle_to_EEPROMbuf (&milling_mode, eeprom_tx_buffer); //сохранение угловых данных режима фрезировки
									EEPROM_WriteBytes (MEMORY_PAGE_ANGLE_ROTATION, eeprom_tx_buffer, 14); //сохранение в EEPROM
									default_screen_mode2 (&milling_mode);
									encoder_data.prevCounter_ShaftRotation = encoder_data.prevCounter_SetAngle; //сброс показаний энкодера
									break;	
								}
							}
							break;
					
						case KEY_ENC_LONG: 		//длинное нажатие энкодера					
							default_screen_mode2 (&milling_mode);
							break;
				
						case KEY_MODE: 
							status_flag.tool_mode = MODE_DEFAULT; //если активен первый режим = MODE_DEFAULT; //установка режима по умолчанию
							GetAngleShaft_from_Seconds (&curr_rotation); //получение текущего положения вала в формате гр/мин/с
							default_screen_mode1 (&curr_rotation); //заставка дисплея - режим 1
							encoder_reset (&encoder_data); 	//сброс показаний энкодера (если энкодер крутили во втором режиме)
							break;
				
						default:
							key_code = NO_KEY;
							break;	
					}
				}				
			}			
		}
		
		if (status_flag.tool_mode == MODE_DEFAULT) //если активен первый режим
		{	enc_shaft_rotation (&curr_rotation, &encoder_data);	} //отслеживания положения энкодера и поворот	вала в случае изменения его изменения
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
