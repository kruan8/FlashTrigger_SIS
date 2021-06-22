/*
 * hw.c
 *
 *  Created on: 5. 11. 2020
 *  Author:     Priesol Vladimir
 */

#include "hw.h"
#include "timer.h"
#include "clock.h"

#include "stm32l0xx_ll_adc.h"
#include "stm32l0xx_ll_bus.h"
#include "stm32l0xx_ll_gpio.h"
#include "stm32l0xx_ll_pwr.h"
#include "stm32l0xx_ll_cortex.h"

#define HW_BUTTON                 PA0

// contact from camera
#define HW_INPUT                  PA10

#define HW_SLAVE                  PA9

// LED
#define HW_LED                    PA4
#define HW_LED_OFF                GPIO_RESETPIN(HW_LED)
#define HW_LED_ON                 GPIO_SETPIN(HW_LED)

// flash output
#define HW_FLASH                  PC14
#define HW_FLASH_ON               GPIO_SETPIN(HW_FLASH)
#define HW_FLASH_OFF              GPIO_RESETPIN(HW_FLASH)

// switch deboucing
#define HW_CHECK_MSEC             1 // Read hardware every 1 msec - called from SysTimer
#define HW_PRESS_MSEC            10 // Stable time before registering pressed
#define HW_RELEASE_MSEC          20 // Stable time before registering released

#define FLASH_DURATION_MS         6     // delka impulsu na tyristoru spinani blesku

uint16_t g_nLedInterval;                // counter delky svitu LED
uint16_t g_nFlashInterval_ms;           // counter delky impulsu na tyristoru
uint32_t g_nOffInterval;                // counter intervalu do vypnuti

static volatile bool g_bDebouncedKeyPress = false;    // This holds the debounced state of the key.
static volatile uint32_t g_nButtonStateDuration;      // active switch state duration (ms)

volatile bool g_bButtonPressed = true;


static void _SysTickCallback(void);
void _DebounceSwitch(bool bSwitchState);

void HW_Init(void)
{
  // pri behu na MSI 2,1 MHz je spotreba 261 uA
  // pri behu na HSI 16MHz je spotreba cca MCU cca 1,5 mA
  SetHSI16();
//  SetMSI(msi_4Mhz);

  SystemCoreClockUpdate();

  Timer_Init();

  SystemCoreClockUpdate();

  GPIO_ConfigPin(HW_LED, mode_output, outtype_pushpull, pushpull_no, speed_low);
  HW_LED_OFF;

  GPIO_ConfigPin(HW_FLASH, mode_output, outtype_pushpull, pushpull_no, speed_low);
  HW_FLASH_OFF;

  GPIO_ConfigPin(HW_BUTTON, mode_input, outtype_pushpull, pushpull_up, speed_low);

  GPIO_ConfigPin(HW_INPUT, mode_input, outtype_pushpull, pushpull_up, speed_medium);

  GPIO_ConfigPin(HW_SLAVE, mode_input, outtype_pushpull, pushpull_up, speed_low);

  Timer_SetSysTickCallback(_SysTickCallback);
}

bool HW_IsMaster(void)
{
  return (GET_PORT(HW_SLAVE)->IDR & GET_PIN(HW_SLAVE)) ? false : true;
}

void HW_LedBlink(uint16_t nDuration_ms)
{
  g_nLedInterval = nDuration_ms;
  HW_LED_ON;
}

void HW_FlashBlink(void)
{
  g_nFlashInterval_ms = FLASH_DURATION_MS;
  HW_FLASH_ON;
}

// pomale zhasinani LED (dynamicka zmena PWN LED)
void HW_LedOffDiming(void)
{
  uint16_t led_range = 900;  // delka zhasinani
  uint16_t led_rate = led_range;

  while (led_rate)
  {
    for (uint16_t i = 0; i < led_rate; i++)
    {
      HW_LED_ON;
    }

    for (uint16_t i = led_range; i > led_rate; i--)
    {
      HW_LED_OFF;
    }

    led_rate--;
  }
}

// pokud je tlacitko stisknuto, vraci dobu trvani stisknuti v ms, jinak 0
uint32_t HW_IsButtonPressed_ms(void)
{
  if (g_bDebouncedKeyPress)
  {
    return g_nButtonStateDuration;
  }

  return 0;
}

bool HW_IsInputActive(void)
{
  return (GET_PORT(HW_INPUT)->IDR & GET_PIN(HW_INPUT)) ? false : true;
}

// prechod Standby modu
void HW_StandbyMode(void)
{
  // to standby
  RCC->APB1ENR |= RCC_APB1ENR_PWREN;  // PWR enable
  PWR->CSR |= PWR_CSR_EWUP1;          // Enable WKUP pin 1

  LL_PWR_ClearFlag_WU();
  PWR->CR |= PWR_CR_CSBF;  // clear Standby flag
  PWR->CR |= PWR_CR_PDDS;  // Select STANDBY mode
  LL_LPM_EnableDeepSleep();

  __ASM volatile ("wfi");

  while(1);
}

// vypnuti
void HW_DeviceOff(void)
{
  HW_LedOffDiming();
  HW_StandbyMode();
}

void HW_SetOffInterval(uint32_t nInterval_ms)
{
  g_nOffInterval = nInterval_ms;
}

uint32_t HW_GetOffTime(void)
{
  return g_nOffInterval;
}

static void _SysTickCallback(void)
{
  // odmereni delky sviceni LED
  if (g_nLedInterval)
  {
    g_nLedInterval--;
    if (g_nLedInterval == 0)
    {
      HW_LED_OFF;
    }
  }

  // odmereni delky impulsu na tyristoru
  if (g_nFlashInterval_ms)
  {
    g_nFlashInterval_ms--;
    if (g_nFlashInterval_ms == 0)
    {
      HW_FLASH_OFF;
    }
  }

  // odmereni OFF intervalu
  if (g_nOffInterval)
  {
    g_nOffInterval--;
  }

  // switch deboucing
  _DebounceSwitch(GET_PORT(HW_BUTTON)->IDR & GET_PIN(HW_BUTTON));
}

// Service routine called every CHECK_MSEC to
// debounce both edges
void _DebounceSwitch(bool bSwitchState)
{
  static uint32_t Count = HW_PRESS_MSEC / HW_CHECK_MSEC;

  if (bSwitchState == g_bDebouncedKeyPress)
  {
    // Set the timer which will allow a change from the current state.
    if (g_bDebouncedKeyPress)
    {
      g_nButtonStateDuration++;
      Count = HW_RELEASE_MSEC / HW_CHECK_MSEC;
    }
    else
    {
      Count = HW_PRESS_MSEC / HW_CHECK_MSEC;
    }
  }
  else
  {
    // Key has changed - wait for new state to become stable.
    g_nButtonStateDuration = 0;
    if (--Count == 0)
    {
      // Timer expired - accept the change.
      g_bDebouncedKeyPress = bSwitchState;

      // And reset the timer.
      if (g_bDebouncedKeyPress)
      {
        Count = HW_RELEASE_MSEC / HW_CHECK_MSEC;
      }
      else
      {
        Count = HW_PRESS_MSEC / HW_CHECK_MSEC;
      }
    }
  }
}
