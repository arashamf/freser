/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.c
  * @brief   This file provides code for the configuration
  *          of the TIM instances.
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
#include "tim.h"

/* USER CODE BEGIN 0 */
#include "systick.h"
#include "typedef.h"
#include "ssd1306.h"

static xTIMER xTimerList[MAX_xTIMERS];
static portTickType xTimeNow;
extern void vTimerDisplayCallback(xTimerHandle xTimer);
static xTimerHandle xTimerDisplay;
static void tim_delay_init (void);
static void timer_bounce_init (void);

uint8_t end_bounce = 0;
extern angular_data_t curr_rotation;
/* USER CODE END 0 */

/* TIM3 init function */
void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  LL_TIM_InitTypeDef TIM_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);

  LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
  /**TIM3 GPIO Configuration
  PA6   ------> TIM3_CH1
  PA7   ------> TIM3_CH2
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_7;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  LL_TIM_SetEncoderMode(TIM3, LL_TIM_ENCODERMODE_X2_TI1);
  LL_TIM_IC_SetActiveInput(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_DIRECTTI);
  LL_TIM_IC_SetPrescaler(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV2);
  LL_TIM_IC_SetFilter(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV1_N2);
  LL_TIM_IC_SetPolarity(TIM3, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_RISING);
  LL_TIM_IC_SetActiveInput(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_ACTIVEINPUT_DIRECTTI);
  LL_TIM_IC_SetPrescaler(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_ICPSC_DIV2);
  LL_TIM_IC_SetFilter(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_IC_FILTER_FDIV1_N4);
  LL_TIM_IC_SetPolarity(TIM3, LL_TIM_CHANNEL_CH2, LL_TIM_IC_POLARITY_RISING);
  TIM_InitStruct.Prescaler = 0;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 65535;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  LL_TIM_Init(TIM3, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM3);
  LL_TIM_SetTriggerOutput(TIM3, LL_TIM_TRGO_RESET);
  LL_TIM_DisableMasterSlaveMode(TIM3);
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/* USER CODE BEGIN 1 */
//-----------------------------------------------------------------------------------------------------//
void encoder_init(void) 
{
    
  LL_TIM_SetCounter(TIM3, 32767); // начальное значение счетчика:
	
	LL_TIM_CC_EnableChannel(TIM3,LL_TIM_CHANNEL_CH1); //Enable the encoder interface channels 
	LL_TIM_CC_EnableChannel(TIM3,LL_TIM_CHANNEL_CH2);

  LL_TIM_EnableCounter(TIM3);     // включение таймера
}

//-----------------------------------------------------------------------------------------------------//
void tim_delay_init (void)
{
	LL_TIM_InitTypeDef TIM_InitStruct = {0};

  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM14);   // Peripheral clock enable 

  TIM_InitStruct.Prescaler = (uint16_t)((CPU_CLOCK_VALUE/1000000)-1); //предделитель 48ћ√ц/48=1ћ√ц
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 0xFF;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  LL_TIM_Init(TIM14, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM14);
}

//-----------------------------------------------------------------------------------------------------//
void delay_us(uint16_t delay)
{
  LL_TIM_SetAutoReload(TIM14, delay); //
	LL_TIM_ClearFlag_UPDATE(TIM14); //сброс флага обновлени€ таймера
	LL_TIM_SetCounter(TIM14, 0); //сброс счЄтного регистра
	LL_TIM_EnableCounter(TIM14); //включение таймера
	while (LL_TIM_IsActiveFlag_UPDATE(TIM14) == 0) {} //ожидание установки флага обновлени€ таймера 
	LL_TIM_DisableCounter(TIM14); //выключение таймера		
}

//-----------------------------------------------------------------------------------------------------//
void timer_bounce_init (void)
{
	LL_TIM_InitTypeDef TIM_InitStruct = {0};

  LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_TIM16);   // Peripheral clock enable 

  TIM_InitStruct.Prescaler = (uint16_t)((CPU_CLOCK_VALUE/2000)-1); //предделитель 48ћ√ц/24000=2 √ц
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 0xFFFF;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  TIM_InitStruct.RepetitionCounter = 0;
  LL_TIM_Init(TIM16, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM16);	
	
	LL_TIM_ClearFlag_UPDATE(TIM16); //сброс флага обновлени€ таймера
	LL_TIM_EnableIT_UPDATE(TIM16);
	NVIC_SetPriority(TIM16_IRQn, 0);
  NVIC_EnableIRQ(TIM16_IRQn);
}

//-----------------------------------------------------------------------------------------------------//
void repeat_time (uint16_t delay)
{
  LL_TIM_SetAutoReload(TIM16, 2*delay); //
	LL_TIM_SetCounter(TIM16, 0); //сброс счЄтного регистра
	LL_TIM_ClearFlag_UPDATE(TIM16); //сброс флага обновлени€ таймера
	LL_TIM_EnableCounter(TIM16); //включение таймера	
}

//-----------------------------------------------------------------------------------------------------//
void TIM16_IRQHandler(void)
{
	if (LL_TIM_IsActiveFlag_UPDATE(TIM16) == SET)
	{	
		LL_TIM_ClearFlag_UPDATE (TIM16); //сброс флага обновлени€ таймера
		LL_TIM_DisableCounter(TIM16); //выключение таймера
		end_bounce = SET; //установка флага окончани€ ожидани€ прекращени€ дребезга
	}
}

//-----------------------------------------------------------------------------------------------------//
void xTimer_Init(uint32_t (*GetSysTick)(void))
{
	xTimeNow = GetSysTick; //получение текущего системного времени
}

//-----------------------------------------------------------------------------------------------------//
xTimerHandle xTimer_Create(uint32_t xTimerPeriodInTicks, FunctionalState AutoReload, 
tmrTIMER_CALLBACK CallbackFunction, FunctionalState NewState)
{
	xTimerHandle NewTimer = NULL;
	uint16_t i;
	
	for (i = 0; i < MAX_xTIMERS; i++) 
	{
		if (xTimerList[i].CallbackFunction == NULL) 
		{
			xTimerList[i].periodic = xTimerPeriodInTicks;
			xTimerList[i].AutoReload = AutoReload;
			xTimerList[i].CallbackFunction = CallbackFunction;
			
			if (NewState != DISABLE) 
			{
				xTimerList[i].expiry = xTimeNow() + xTimerPeriodInTicks; 
				xTimerList[i].State = __ACTIVE;
			} 
			else 
			{	xTimerList[i].State = __IDLE;	}		
			NewTimer = (xTimerHandle)(i + 1);
			break;
    }
  }
	return NewTimer;
}

//-----------------------------------------------------------------------------------------------------//
void xTimer_SetPeriod(xTimerHandle xTimer, uint32_t xTimerPeriodInTicks) 
{
	if ( xTimer != NULL ) 
	{	xTimerList[(uint32_t)xTimer-1].periodic = xTimerPeriodInTicks;	}
}

//-----------------------------------------------------------------------------------------------------//
void xTimer_Reload(xTimerHandle xTimer) 
{
	if ( xTimer != NULL ) 
	{
		xTimerList[(uint32_t)xTimer-1].expiry = xTimeNow() + xTimerList[(uint32_t)xTimer-1].periodic;
		xTimerList[(uint32_t)xTimer-1].State = __ACTIVE;
	}
}

//-----------------------------------------------------------------------------------------------------//
void xTimer_Delete(xTimerHandle xTimer)
{
	if ( xTimer != NULL ) 
	{
		xTimerList[(uint32_t)xTimer-1].CallbackFunction = NULL;
		xTimerList[(uint32_t)xTimer-1].State = __IDLE;
		xTimer = NULL;
	}		
}

//-----------------------------------------------------------------------------------------------------//
void xTimer_Task(uint32_t portTick)
{
	uint16_t i;
	
	for (i = 0; i < MAX_xTIMERS; i++) 
	{
		switch (xTimerList[i].State) 
		{
			case __ACTIVE:
				if ( portTick >= xTimerList[i].expiry ) 
				{				
					if ( xTimerList[i].AutoReload != DISABLE ) 
					{	xTimerList[i].expiry = portTick + xTimerList[i].periodic;	} 
					else 
					{	xTimerList[i].State = __IDLE;	}			
					xTimerList[i].CallbackFunction((xTimerHandle)(i + 1));
				}					
				break;
				
			default:
				break;
		}
	}	
}


//-----------------------------------------------------------------------------------------------------//
void TimerDisplayStart (void)
{
	xTimerDisplay = xTimer_Create(3000, ENABLE, &vTimerDisplayCallback, ENABLE);
}

//-----------------------------------------------------------------------------------------------------//
void vTimerDisplayCallback (xTimerHandle xTimer)
{
//	display_default_screen (&curr_rotation);
}

//-----------------------------------------------------------------------------------------------------//
void TimerDisplayDelete (void)
{
	xTimer_Delete(xTimerDisplay);
}

//-----------------------------------------------------------------------------------------------------//
void timers_ini (void)
{
	SysTick_Init (xTimer_Task);
	xTimer_Init(&Get_SysTick); //&Get_SysTick - указатель на ф-ю получени€ системного времени
	
	tim_delay_init(); 		//инициализаци€ TIM14 дл€ микросекундных задержек
	timer_bounce_init();	//инициализаци€ TIM16	дл€ отчЄта задержек дребезга кнопок
 	
	//таймер таймаута при отсутствии получени€ сообщений по UART от GPS приемника
	//xTimerDisplay = xTimer_Create(5000, ENABLE, &vTimerDisplayCallback, ENABLE); //перезагрузка и инициализаци€ GPS-модул€ через 5с 
						
}
/* USER CODE END 1 */
