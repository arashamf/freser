
/* Includes ------------------------------------------------------------------*/
#include "drive.h"
#include "gpio.h"
#include "tim.h"

// Functions -----------------------------------------------------------------//
void one_step (uint8_t dir)
{
	DRIVE_ENABLE(ON);
	delay_us (5);
	DIR_DRIVE (dir);
	delay_us (5);
	STEP(ON);
	delay_us (3);
	STEP(OFF);
	delay_us (3);
	DRIVE_ENABLE(OFF);
	delay_us (2);
}

//----------------------------------------------------------------------------//
void step_angle (uint8_t dir, uint32_t need_step)
{
	DRIVE_ENABLE(ON);
	delay_us (6);
	DIR_DRIVE (dir);
	delay_us (6);
	for (uint32_t count = 0; count < need_step; count++)
	{
		STEP(ON);
		delay_us (3);
		STEP(OFF);
		delay_us (3);
	}
	DRIVE_ENABLE(OFF);
	delay_us (2);
	if (dir == FORWARD)
		DIR_DRIVE (BACKWARD);
	else
		DIR_DRIVE (FORWARD);
}

//----------------------------------------------------------------------------//
