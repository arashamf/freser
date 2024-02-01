
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
void angle_from_EEPROMbuf (angular_data_t * , uint8_t * );
void SetAngle_in_Seconds (angular_data_t *);
void GetSetAngle_from_Seconds (angular_data_t * );
void ShaftAngle_in_Seconds (angular_data_t * );
void GetAngleShaft_from_Seconds (angular_data_t * );
void AngleShaftReset (angular_data_t * );
void SetAngleReset (angular_data_t * );
void MilAngleTeeth_in_Seconds (milling_data_t * );
void MilAngleTeeth_from_Seconds (milling_data_t * );
void GetMilAngleTeeth (milling_data_t * );
void MilAngleTeethReset (milling_data_t * );
void RemainTeethGearReset (milling_data_t * );
void teeth_angle_to_EEPROMbuf (milling_data_t * , uint8_t * , STATUS_FLAG_t * );
void teeth_angle_from_EEPROMbuf (milling_data_t * , uint8_t *, STATUS_FLAG_t * );
void remain_teeth_to_EEPROMbuf (milling_data_t * , uint8_t * );
#ifdef __cplusplus
}
#endif

#endif 

