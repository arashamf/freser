
#ifndef __ANGLE_CALC_H__
#define __ANGLE_CALC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "typedef.h" 

/* Defines ------------------------------------------------------------------*/

/* Prototypes ------------------------------------------------------------------*/
void init_angular_data (angular_data_t * );
void angle_to_EEPROMbuf (angular_data_t * , uint8_t *);
void angle_from_EEPROMbuf (angular_data_t * , uint8_t *);
void angle_in_seconds (angular_data_t *);
#ifdef __cplusplus
}
#endif

#endif 

