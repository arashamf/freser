
// Includes ------------------------------------------------------------------//
#include "stanok_math.h"
#include "typedef.h"
#include <math.h>

// Functions -----------------------------------------------------------------//

// Variables -----------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------//
uint32_t calc_steps_mode1 (angular_data_t * handle, float step_unit)
{
		uint32_t need_step = 0;
		float buffer = 0;
	
		handle->mode1_error += ((remainderf(handle->StepAngleInSec, step_unit))/step_unit); //расчёт остатка
		buffer = rintf((handle->StepAngleInSec)/step_unit); //округление до целого полученного значения типа float
		need_step = (uint32_t)(REDUCER*buffer);	
		if (handle->mode1_error > 1.0) //если накопилась погрешность больше 1
		{	
			handle->mode1_error -= 1.0;	
			need_step += REDUCER;
		}
		else
		{
			if (handle->mode1_error < (-1.0))  //если накопилась погрешность меньше 1
			{	
				handle->mode1_error += 1.0;	
				need_step -= REDUCER;
			}
		}
		return need_step;
}

//---------------------------------------------------------------------------------------------------//
uint32_t calc_steps_milling (milling_data_t * handle, float step_unit)
{
		uint32_t need_step = 0;
		float buffer = 0;
	
		handle->milling_error += ((remainderf(handle->AngleTeethInSec, step_unit))/step_unit); //расчёт остатка
		buffer = rintf((handle->AngleTeethInSec)/step_unit); //округление до целого полученного значения типа float
		need_step = (uint32_t)(REDUCER*buffer);	
		if (handle->milling_error > 1.0) //если накопилась погрешность больше 1
		{	
			handle->milling_error -= 1.0;	
			need_step += REDUCER;
		}
		else
		{
			if (handle->milling_error < (-1.0))  //если накопилась погрешность меньше 1
			{	
				handle->milling_error += 1.0;	
				need_step -= REDUCER;
			}
		}
		return need_step;
}

//---------------------------------------------------------------------------------------------------//
uint32_t steps_for_back_to_zero (uint32_t Second, float step_unit)
{
		uint32_t need_step = 0;

		need_step = (uint32_t)(REDUCER*(Second/step_unit));	
		return need_step;
}

//---------------------------------------------------------------------------------------------------//
uint32_t calc_passed_angle  (uint32_t receive_step, float step_unit)
{
		float buffer;
		uint32_t second = 0;
	
		buffer = (uint32_t )(rintf((receive_step/REDUCER) * step_unit));	
		second = (uint32_t)buffer;
		return second;
}


