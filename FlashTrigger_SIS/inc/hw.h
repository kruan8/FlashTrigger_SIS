/*
 * hw.h
 *
 *  Created on: 5. 11. 2020
 *  Author:     Priesol Vladimir
 */

#ifndef HW_H_
#define HW_H_

#include "stm32l0xx.h"
#include "common_L0.h"
#include <stdbool.h>

void HW_Init(void);
bool HW_IsMaster(void);
void HW_LedBlink(uint16_t nDuration_ms);
void HW_FlashBlink(void);
void HW_LedOffDiming(void);
uint32_t HW_IsButtonPressed_ms(void);
bool HW_IsInputActive(void);
void HW_StandbyMode(void);
void HW_DeviceOff(void);
void HW_SetOffInterval(uint32_t nInterval_ms);
uint32_t HW_GetOffTime(void);

#endif /* HW_H_ */
