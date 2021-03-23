/*
 * app.c
 *
 *  Created on: 6. 11. 2020
 *      Author: Priesol Vladimir
 */

#include "app.h"
#include "timer.h"
#include "si4463.h"
#include "hw.h"

#include <string.h>

#define APP_CHECK_INTERVAL_MS               2500              // interval for CHECK packet sending
#define APP_FLASH_TRANSMIT_COUNT             3

#define APP_OFF_INTERVAL_MS                 (1000 * 60 * 5)   // off interval po zapnuti (bez signalu) - 5 minut
#define APP_OFF_INTERVAL_RCV_MS             (1000 * 60 * 10)  // off interval po prijeti signalu - 10 minut
#define APP_BUTTON_INTERVAL_FOR_FLASH_MS    1500              // interval for Flash
#define APP_FLASH_LED_INTERVAL_MS           400               // LED on by flashing

const uint8_t g_CheckStamp = { 0xAA };
const uint8_t g_FlashStamp = { 0x55 };

bool g_bMaster = 0;


void _MasterExec(bool bManualFlash);
void _SlaveExec(bool bManualFlash);

void App_Init(void)
{
  HW_Init();

  // zjistime, jestli jsme MASTER nebo SLAVE
  g_bMaster = HW_IsMaster();

  // start blik
  HW_LedBlink(200);

  bool bResult = SI4463_Init();
  if (!bResult)
  {
    while (1);
  }

  HW_SetOffInterval(APP_OFF_INTERVAL_MS);
  while (HW_IsButtonPressed_ms());

  if (!g_bMaster)
  {
    SI4463_StartRxData();
  }
}

void App_Exec(void)
{
  static bool bManualFlash = false;

  if (g_bMaster)
  {
    _MasterExec(bManualFlash);
  }
  else
  {
    _SlaveExec(bManualFlash);
  }

  // kontrola casovace vypnuti (do Standby modu)
  if (HW_GetOffTime() == 0)
  {
    HW_StandbyMode();
  }

  bManualFlash = false;

  // kontrola stisku tlacitka (vypnuti / flash)
  uint32_t nButtonPressDuration_ms = 0;
  uint32_t nMaxDuration = 0;

  // wait for button release
  while ((nButtonPressDuration_ms = HW_IsButtonPressed_ms()) > 0)
  {
    nMaxDuration = nButtonPressDuration_ms;
  }

  if (nMaxDuration > APP_BUTTON_INTERVAL_FOR_FLASH_MS)
  {
    bManualFlash = true;
  }
  else if (nMaxDuration > 0)
  {
    HW_DeviceOff();
  }
}

void _MasterExec(bool bManualFlash)
{
  static uint32_t nNextCheckTime = APP_CHECK_INTERVAL_MS;

  if (HW_IsInputActive() || bManualFlash)
  {
    HW_LedBlink(APP_FLASH_LED_INTERVAL_MS);
    for (uint8_t i = 0; i < APP_FLASH_TRANSMIT_COUNT; ++i)
    {
      SI4463_SendData((uint8_t*)&g_FlashStamp, sizeof(g_FlashStamp));
    }

    HW_SetOffInterval(APP_OFF_INTERVAL_RCV_MS);
    nNextCheckTime = Timer_GetTicks_ms() + APP_CHECK_INTERVAL_MS;

    // wait for flash impulse ending
    while (HW_IsInputActive());
  }

  if (Timer_GetTicks_ms() > nNextCheckTime)
  {
    nNextCheckTime = Timer_GetTicks_ms() + APP_CHECK_INTERVAL_MS;
    HW_LedBlink(100);

    // send CHECK
    SI4463_SendData((uint8_t*)&g_CheckStamp, sizeof(g_CheckStamp));
  }
}

void _SlaveExec(bool bManualFlash)
{
  uint8_t buffer[10];

  if (bManualFlash)
  {
    HW_FlashBlink();
    HW_LedBlink(APP_FLASH_LED_INTERVAL_MS);
    HW_SetOffInterval(APP_OFF_INTERVAL_RCV_MS);
  }

  if (!SI4463_IsRxReady())
  {
    return;
  }

  uint8_t nSize = SI4463_GetRxData(buffer, sizeof(buffer));
  SI4463_StartRxData();
  if (nSize == 0)
  {
    return;
  }

  if (buffer[0] == g_CheckStamp)
  {
    HW_LedBlink(100);
  }

  if (buffer[0] == g_FlashStamp)
  {
    HW_FlashBlink();
    HW_LedBlink(APP_FLASH_LED_INTERVAL_MS);
    HW_SetOffInterval(APP_OFF_INTERVAL_RCV_MS);
  }

}
