
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
void angle_to_EEPROMbuf (angular_data_t * , uint8_t *);
void angle_from_EEPROMbuf (angular_data_t * , uint8_t *);
void init_angular_data (angular_data_t *, uint8_t * );
void SetAngle_in_Seconds (angular_data_t *);
void ShaftAngle_in_Seconds (angular_data_t * );
void GetAngle_from_Seconds (angular_data_t * handle);
#ifdef __cplusplus
}
#endif

#endif 

