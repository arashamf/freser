
/* Includes ------------------------------------------------------------------*/
#include "drive.h"
#include "gpio.h"
#include "tim.h"

/* Functions -----------------------------------------------------------------*/
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