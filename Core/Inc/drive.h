
#ifndef __DRIVE_H__
#define __DRIVE_H__

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------//
#include "main.h"

// Defines ---------------------------------------------------------------------//
#define MEMORY_PAGE_ANGLE_ROTATION 0x0001
#define STEP_UNIT 0.324 //ход шаговика в секундах с учётом делителя 20000

// Prototypes ------------------------------------------------------------------//
void one_step (uint8_t );
void step_angle (uint8_t , uint32_t );
void set_angle (angular_data_t * , encoder_data_t *);			
void enc_shaft_rotation (angular_data_t *, encoder_data_t * );
void rbt_shaft_rotation (angular_data_t * ) ;
void lbt_shaft_rotation (angular_data_t * ) ;
// Variables ------------------------------------------------------------------//
extern __IO uint32_t need_step;
#ifdef __cplusplus
}
#endif

#endif 

