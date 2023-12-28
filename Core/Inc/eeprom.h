
#ifndef __EEPROM_H__
#define __EEPROM_H__

#ifdef __cplusplus
extern "C" {
#endif

// Includes ---------------------------------------------------------------------//
#include "main.h"

// Defines ---------------------------------------------------------------------//
#define 	MEMORY_PAGE_ANGLE_ROTATION 	0x0001
#define 	MEMORY_PAGE_TEETH_ANGLE 		0x0010

// Prototypes -----------------------------------------------------------------//
void EEPROM_WriteBytes	(uint16_t, uint8_t*, uint16_t);
void EEPROM_ReadBytes	(uint16_t, uint8_t*, uint16_t);

// Variables ------------------------------------------------------------------//
extern uint8_t eeprom_rx_buffer[];
extern uint8_t eeprom_tx_buffer[];
#ifdef __cplusplus
}
#endif

#endif 

