
#ifndef __EEPROM_H__
#define __EEPROM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Defines ------------------------------------------------------------------*/

/* Prototypes ------------------------------------------------------------------*/
void EEPROM_WriteBytes	(uint16_t, uint8_t*, uint16_t);
void EEPROM_ReadBytes	(uint16_t, uint8_t*, uint16_t);
void angle_to_EEPROMbuf (uint16_t , uint8_t , uint8_t , uint8_t *);
void angle_from_EEPROMbuf (uint16_t , uint8_t , uint8_t , uint8_t *);
#ifdef __cplusplus
}
#endif

#endif 

