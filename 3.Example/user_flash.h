/*!
 * Copyright (c) 2023, ErBW_s
 * All rights reserved.
 * 
 * @author  Baohan
 */

#ifndef MM32F527X_E9P_USER_FLASH_H
#define MM32F527X_E9P_USER_FLASH_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include "zf_driver_flash.h"

extern uint16_t flashBufIndex;       // Flash buffer array index(0-255)
extern uint8_t flashSecIndex;        // Flash section index(63-)
extern uint8_t flashPageIndex;       // Flash page index(3-0)

void SaveToFlash(const int32_t *value);
void SaveToFlashWithConversion(const double *value);
void ReadFlash(int32_t *value);
void ReadFlashWithConversion(double *value);
void FlashOperationEnd();

#ifdef __cplusplus
}
#endif

#endif
