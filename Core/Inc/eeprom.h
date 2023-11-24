
#ifndef __DRIVE_H__
#define __DRIVE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Defines ------------------------------------------------------------------*/

/* Prototypes ------------------------------------------------------------------*/
void EEPROM_WriteBytes	(uint16_t, uint8_t*, uint16_t);
void EEPROM_ReadBytes	(uint16_t, uint8_t*, uint16_t);

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

