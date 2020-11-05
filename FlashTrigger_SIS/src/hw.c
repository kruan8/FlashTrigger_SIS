/*
 * hw.c
 *
 *  Created on: 5. 11. 2020
 *  Author:     Priesol Vladimir
 */

#include "hw.h"
#include "timer.h"

#include "stm32l0xx_ll_adc.h"
#include "stm32l0xx_ll_bus.h"
#include "stm32l0xx_ll_gpio.h"

#define HW_BUTTON                 PA0

// contact from camera
#define HW_INPUT                  PA0

// LED
#define HW_LED                    PA4
#define HW_LED_ON                 GPIO_RESETPIN(HW_LED)
#define HW_LED_OFF                GPIO_SETPIN(HW_LED)

// flash output
#define HW_FLASH                  PC14
#define HW_FLASH_ON               GPIO_SETPIN(HW_FLASH)
#define HW_FLASH_OFF              GPIO_RESETPIN(HW_FLASH)


static void _SysTickCallback(void);

void HW_Init(void)
{
  Timer_Init();

  GPIO_ClockEnable(HW_LED);
  HW_LED_OFF;
  GPIO_ConfigPin(HW_LED, mode_output, outtype_pushpull, pushpull_no, speed_low);

  GPIO_ClockEnable(HW_FLASH);
  HW_FLASH_OFF;
  GPIO_ConfigPin(HW_FLASH, mode_output, outtype_pushpull, pushpull_no, speed_low);

  GPIO_ClockEnable(HW_BUTTON);
  GPIO_ConfigPin(HW_BUTTON, mode_input, outtype_pushpull, pushpull_up, speed_low);

  GPIO_ClockEnable(HW_INPUT);
  GPIO_ConfigPin(HW_INPUT, mode_input, outtype_pushpull, pushpull_up, speed_medium);

  Timer_SetSysTickCallback(_SysTickCallback);
}

static void _SysTickCallback(void)
{

}
