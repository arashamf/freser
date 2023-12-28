
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
void GetSetAngle_from_Seconds (angular_data_t * );
void ShaftAngle_in_Seconds (angular_data_t * );
void GetAngleShaft_from_Seconds (angular_data_t * );
void AngleShaftReset (angular_data_t * handle);
void SetAngleReset (angular_data_t * handle);
void MilAngleTeeth_in_Seconds (milling_data_t * );
void MilAngleTeeth_from_Seconds (milling_data_t * );
void GetMilAngleTeeth (milling_data_t * handle);
void MilAngleTeethReset (milling_data_t * handle);
void teeth_angle_to_EEPROMbuf (milling_data_t *, uint8_t * );
void init_teeth_angle_data (milling_data_t * , uint8_t * );
#ifdef __cplusplus
}
#endif

#endif 

