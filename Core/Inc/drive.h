
#ifndef __DRIVE_H__
#define __DRIVE_H__

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------//
#include "main.h"

// Defines ---------------------------------------------------------------------//
#define 	MEMORY_PAGE_ANGLE_ROTATION 	0x0001
#define 	STEP18_IN_SEC								6480
#define 	CIRCLE_IN_SEC								1296000	

#define 	STEPS_IN_REV								20000
#define 	STEP_DIV 										100
#define 	CIRCLE_IN_STEP							STEPS_IN_REV/STEP_DIV	

// Prototypes ------------------------------------------------------------------//
void rotate_one_step (uint8_t );
void step_angle (uint8_t , uint32_t );
void set_angle (angular_data_t * , encoder_data_t *);			
void enc_shaft_rotation (angular_data_t *, encoder_data_t * );
void right_shaft_rotation (angular_data_t * ) ;
void left_shaft_rotation (angular_data_t * ) ;
void left_rotate_to_zero (angular_data_t * ) ;
void right_rotate_to_zero (angular_data_t * ) ;
void one_full_turn (void) ;
// Variables ------------------------------------------------------------------//
extern __IO uint32_t need_step;
#ifdef __cplusplus
}
#endif

#endif 

