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
void SetupRotationMode (encoder_data_t *, angular_data_t *, uint8_t * );
void SetupMillingMode (milling_data_t *, encoder_data_t *, angular_data_t *, uint8_t *);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//---------------------------------------------------------------------------------=-------------------//

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
	
	ssd1306_Init();
	ssd1306_Clear();
	
	EEPROM_ReadBytes (MEMORY_PAGE_ANGLE_ROTATION, eeprom_rx_buffer, EEPROM_NUMBER_BYTES); //получение угловых данных установки поворота из EEPROM
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
		SetAngleReset (&curr_rotation); //сброс всех настроек
		AngleShaftReset (&curr_rotation);
		MilAngleTeethReset (&milling_mode);
	}
	default_screen_mode1 (&curr_rotation); //заставка по умолчанию
	DRIVE_ENABLE(OFF); //отключение привода
	STEP(OFF); //	
	status_flag.tool_mode = MODE_DEFAULT; //флаг = режим 1
	
	delay_us (20000);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		if ((key_code = scan_keys()) != NO_KEY) //если была нажата кнопка
		{
			if (status_flag.tool_mode == MODE_DEFAULT) //если активен режим 1
			{	
				switch (key_code) //обработка кода нажатой кнопки
				{					
					case KEY_LEFT:	//нажатие левой кнопки
						default_screen_mode1 (&curr_rotation); //главное меню режима 1 
						break;
					
					case KEY_CENTER_SHORT: //короткое нажатие центр. кнопки - переход в подрежим возврата вала в нулевую позицию
						return_mode_screen (&curr_rotation); //заставка режима возврата вала
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
								
								/*	case KEY_CENTER_SHORT:
										one_full_turn(); //оборот на 360 градусов
										break;*/
								
									case KEY_RIGHT: 
										right_rotate_to_zero (&curr_rotation); //поворот по часовой позиции в нулевую позицию
										break;	

									default:
										key_code = NO_KEY; //выход из подрежима 
										break;									
								}
								default_screen_mode1 (&curr_rotation);	//главное меню режима 1 
								break;
							}
						}
						break;
						
					case KEY_CENTER_LONG:	//длинное нажатие центр. кнопки - текущее положение вала устанавливается нулевым
						AngleShaftReset (&curr_rotation); //сброс текущего положения вала						
						angle_to_EEPROMbuf (&curr_rotation, eeprom_tx_buffer); //cохранение в буфере EEPROM текущих данных угла вала
						EEPROM_WriteBytes (MEMORY_PAGE_ANGLE_ROTATION, eeprom_tx_buffer, (EEPROM_NUMBER_BYTES-6)); //запись 8 байт
						default_screen_mode1 (&curr_rotation); //главное меню режима 1 
						break;
						
						
					case KEY_RIGHT: //нажатие правой кнопки
						default_screen_mode1 (&curr_rotation); //главное меню режима 1 
						break;
					
					case KEY_ENC_SHORT: //короткое нажатие кнопки энкодера - подрежим установки шага хода вала
						SetupRotationMode (&encoder_data, &curr_rotation, eeprom_tx_buffer); //подрежим ввода нового значения шага хода вала
						break;
					
					case KEY_ENC_LONG: //длинное нажатие кнопки энкодера - сброс установленного шага хода вала и ввод нового значения шага 
						SetAngleReset (&curr_rotation); //сброс установленного угла поворота до минимального значения
						angle_to_EEPROMbuf (&curr_rotation, eeprom_tx_buffer);  //сохранение в EEPROM	
						EEPROM_WriteBytes (MEMORY_PAGE_ANGLE_ROTATION, eeprom_tx_buffer, (EEPROM_NUMBER_BYTES-6)); //8 байт							
						SetupRotationMode (&encoder_data, &curr_rotation, eeprom_tx_buffer); //подрежим ввода нового значения шага хода вала
						break;
				
					case KEY_MODE: 
						status_flag.tool_mode = MODE_MILLING; //режим - фрезеровка
						default_screen_mode2 (&milling_mode);	//заставка дисплея - режим 2 по умолчанию
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
							left_teeth_rotation (&milling_mode, &curr_rotation); //поворот фрезы против часовой стрелке
							angle_to_EEPROMbuf (&curr_rotation, eeprom_tx_buffer);  //сохранение угловых данных положения вала
							remain_teeth_to_EEPROMbuf (&milling_mode, eeprom_tx_buffer); //сохранение оставшегося количества зубьев
							EEPROM_WriteBytes (MEMORY_PAGE_ANGLE_ROTATION, eeprom_tx_buffer, (EEPROM_NUMBER_BYTES-5)); //запись 9 байт
							default_screen_mode2 (&milling_mode); //заставка дисплея - режим 2 по умолчанию
							break;
					
						case KEY_CENTER_SHORT: //переход в подрежим
							return_mode_screen (&curr_rotation); //заставка дисплея - режима возврата вала
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
											SetupMillingMode (&milling_mode, &encoder_data, &curr_rotation, eeprom_tx_buffer); //подрежим установки количества зубьев детали
											break;							
								
										case KEY_RIGHT: 
											right_rotate_to_zero (&curr_rotation); //поворот по часовой позиции в нулевую позицию
											RemainTeethGearReset(&milling_mode); //сброс угловых настроек шестерёнки
											SetupMillingMode (&milling_mode, &encoder_data, &curr_rotation, eeprom_tx_buffer); //подрежим установки количества зубьев детали
											break;	

										default:
											key_code = NO_KEY;
											break;									
									}
									default_screen_mode2 (&milling_mode);	//главное меню режима фрезеровки
									break;
								}
							}
							break;
						
						case KEY_CENTER_LONG:	
							default_screen_mode2 (&milling_mode); //главное меню режима фрезеровки
							break;
											
						case KEY_RIGHT: //правая кнопка панели
							right_teeth_rotation	(&milling_mode, &curr_rotation); //поворот фрезы по часовой стрелке
							angle_to_EEPROMbuf (&curr_rotation, eeprom_tx_buffer);  //сохранение угловых данных положения вала
							remain_teeth_to_EEPROMbuf (&milling_mode, eeprom_tx_buffer); //сохранение оставшегося количества зубьев
							EEPROM_WriteBytes (MEMORY_PAGE_ANGLE_ROTATION, eeprom_tx_buffer, (EEPROM_NUMBER_BYTES-5));  //запись 9 байт в EEPROM
							default_screen_mode2 	(&milling_mode); //заставка дисплея - режим 2 по умолчанию
							break;
					
						case KEY_ENC_SHORT: //короткое нажатие энкодера	- резерв в режиме фрезеровки
							default_screen_mode2 (&milling_mode); //главное меню режима фрезеровки
							break;
					
						case KEY_ENC_LONG: 		//длинное нажатие энкодера - сброс всех настроек режима фрезеровки			
							MilAngleTeethReset (&milling_mode); //сброс всех установок режима фрезеровки
							SetupMillingMode (&milling_mode, &encoder_data, &curr_rotation, eeprom_tx_buffer); //подрежим установки количества зубьев детали
							default_screen_mode2 (&milling_mode); //главное меню режима фрезеровки
							break;
				
						case KEY_MODE: 
							status_flag.tool_mode = MODE_DEFAULT; //если активен первый режим = MODE_DEFAULT; //установка режима по умолчанию
							GetAngleShaft_from_Seconds (&curr_rotation); //получение текущего положения вала в формате гр/мин/с
							encoder_reset (&encoder_data); 	//сброс показаний энкодера (если энкодер крутили во втором режиме)
							default_screen_mode1 (&curr_rotation); //заставка дисплея - режим 1
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
//---------------------------функция ввода угловых настроечных данных режима 1---------------------------//
void SetupRotationMode (encoder_data_t * handle_enc, angular_data_t * handle_ang, uint8_t * TXbuffer)
{
	setangle_mode_screen (handle_ang); //меню заставки режима установки шага
	handle_enc->prevCounter_SetAngle = handle_enc->prevCounter_ShaftRotation; //сброс измений показаний энкодера 
	while(1) //установка угловых данных шага угла
	{
		if ((key_code = scan_keys()) != KEY_ENC_SHORT) //пока не будет повторного нажатия кнопки энкодера
		{	set_angle(handle_ang, handle_enc);	}		//проверка показаний энкодера								
		else
		{	
			angle_to_EEPROMbuf (handle_ang, TXbuffer); //перенос данных угла в буффер
			EEPROM_WriteBytes (MEMORY_PAGE_ANGLE_ROTATION, TXbuffer, (EEPROM_NUMBER_BYTES-10)); //запись 4 байт			
			default_screen_mode1 (handle_ang); //заставка дисплея - режим 1 по умолчанию
			handle_enc->prevCounter_ShaftRotation = handle_enc->prevCounter_SetAngle; //сброс измениний показаний энкодера 
			break;	
		}
	}
}

//---------------------------функция ввода настроечных данных фрезеровки в режиме 2---------------------------//
void SetupMillingMode (milling_data_t * handle_mil, encoder_data_t * handle_enc, angular_data_t * handle_ang, uint8_t * TXbuffer)
{
	encoder_reset (handle_enc); 	//сброс показаний энкодера
	setteeth_mode_screen (handle_mil); //заставка дисплея - режим ввода настроечных данных фрезеровки
	while(1) //режим установки количества зубов шестерёнки
	{
		if ((key_code = scan_keys()) != KEY_ENC_SHORT) //пока нет короткого нажатия кнопки энкодера
		{	set_teeth_gear(handle_mil, handle_enc);	} //опрос энкодера - установка количества зубьев шестерёнки
		else
		{	
			GetMilAngleTeeth (handle_mil); //расчёт угла поворота после ввода количества зубов
			angle_to_EEPROMbuf (handle_ang, TXbuffer); //сохранение текущего положения вала в EEPROM буффере
			teeth_angle_to_EEPROMbuf (handle_mil, TXbuffer); //сохранение данных фрезеровки в EEPROM буффере
			EEPROM_WriteBytes (MEMORY_PAGE_ANGLE_ROTATION, TXbuffer, EEPROM_NUMBER_BYTES ); //отправка в EEPROM 14 байт
			handle_enc->prevCounter_ShaftRotation = handle_enc->prevCounter_SetAngle; //сброс показаний энкодера
			break;
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
