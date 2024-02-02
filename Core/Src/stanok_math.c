
// Includes ------------------------------------------------------------------//
#include "stanok_math.h"
#include "typedef.h"
#include <math.h>

// Functions -----------------------------------------------------------------//

// Variables -----------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------//
uint32_t calc_steps (uint32_t Second, float step_unit, float ostatok)
{
		uint32_t need_step = 0;
		float buffer = 0;
	
		ostatok += ((remainderf(Second, step_unit))/step_unit); //расчёт остатка
		buffer = rintf((Second)/step_unit); //округление до целого полученного значения типа float
		need_step = (REDUCER*buffer);	
		if (ostatok > 1.0) //если накопилась погрешность больше 1
		{	
			ostatok -= 1.0;	
			need_step += REDUCER;
		}
		else
		{
			if (ostatok < (-1.0))  //если накопилась погрешность меньше 1
			{	
				ostatok += 1.0;	
				need_step -= REDUCER;
			}
		}
		return need_step;
}



