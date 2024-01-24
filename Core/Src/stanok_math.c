
// Includes ------------------------------------------------------------------//
#include "stanok_math.h"
#include "typedef.h"
#include <math.h>

// Functions -----------------------------------------------------------------//

// Variables -----------------------------------------------------------------//
float ostatok = 0;
//---------------------------------------------------------------------------------------------------//
uint32_t calc_steps (uint32_t Second, float step_unit)
{
		uint32_t need_step = 0;
		float buffer = 0;
	
		ostatok += ((remainderf(Second, step_unit))/step_unit); //расчёт остатка
		if (ostatok > 1.0)
		{	ostatok -= 1.0;	}
		else
		{
			if (ostatok < (-1.0))
			{	ostatok += 1.0;	}
		}
		buffer = rintf((Second)/step_unit); //округление до целого полученного значения типа float
		need_step = (REDUCER*buffer);	
		return need_step;
}



