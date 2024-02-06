
#ifndef __DRIVE_H__
#define __DRIVE_H__

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------//
#include "main.h"

// Defines ---------------------------------------------------------------------//


// Prototypes ------------------------------------------------------------------//
void step_angle (uint8_t , uint32_t );
void set_angle (angular_data_t * , encoder_data_t *);		
void set_teeth_gear (milling_data_t * , encoder_data_t * );
void enc_shaft_rotation (angular_data_t *, encoder_data_t * );
void encoder_reset (encoder_data_t * );
void right_shaft_rotation (angular_data_t * ) ;
void left_shaft_rotation (angular_data_t * ) ;
void left_rotate_to_zero (angular_data_t * ) ;
void right_rotate_to_zero (angular_data_t * ) ;
void one_full_turn (void) ;
void right_teeth_rotation (milling_data_t * , angular_data_t * ) ;
void left_teeth_rotation (milling_data_t * , angular_data_t * );
// Variables ------------------------------------------------------------------//
extern __IO uint32_t need_step;
#ifdef __cplusplus
}
#endif

#endif 

