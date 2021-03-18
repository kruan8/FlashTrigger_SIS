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

#define FLASH_DURATION_MS         6     // delka impulsu na tyristoru spinani blesku

uint16_t g_nLedInterval;                // citac delky svitu LED
uint16_t g_nFlashInterval_ms;           // citac delky impulsu na tyristoru
uint32_t g_nOffInterval;                // citani intervalu do vypnuti

volatile bool g_bButtonPressed = true;
volatile uint32_t g_nButtonStateDuration;

static void _SysTickCallback(void);

void HW_Init(void)
{
  Timer_Init();

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
  if (g_bButtonPressed)
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

  PWR->CR |= PWR_CR_CWUF;  // Clear Wakeup flag
  PWR->CR |= PWR_CR_CSBF;  // clear Standby flag
  PWR->CR |= PWR_CR_PDDS;  // Select STANDBY mode
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk; // Set SLEEPDEEP bit of Cortex-M0 System Control Register

//  __WFI;  // Request Wait For Interrupt
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

  // obsluha tlacitka (odstraneni zakmitu)
#define DBNC_CNT  5   // e.g. 20ms with 5ms tick
  if ((Timer_GetTicks_ms() & 0b11) == 0) // kazde 4 ms
  {

    static uint8_t old_state = 0;
    static uint8_t key_cnt = 0;

//    uint8_t state = (!(BUTTON_GPIO_PORT->IDR & BUTTON_PIN));
    uint8_t state = (GET_PORT(HW_BUTTON)->IDR & GET_PIN(HW_BUTTON));
    if (state != old_state)
    {
      key_cnt = DBNC_CNT;
      g_nButtonStateDuration = 0;
    }
    else if (key_cnt == 0)
    {
      g_bButtonPressed = state;
      g_nButtonStateDuration += 4;
    }
    else
    {
      key_cnt--;
    }

    old_state = state;
  }
}
