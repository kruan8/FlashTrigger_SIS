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

#define OFF_INTERVAL_MS              (1000*60*10)  // 10 minutes
#define CHECK_INTERVAL_MS            2500
#define FLASH_TRANSMIT_COUNT         3

#define FLASH_LIMIT_STD              2
#define FLASH_LIMIT_PRG              8

#define STD_OFF_INTERVAL_MS          (1000 * 60 * 3)   // off interval po zapnuti (bez signalu) - 5 minut
#define STD_OFF_INTERVAL_RCV_MS      (1000 * 60 * 15)  // off interval po prijeti signalu - 15 minut

#define PRG_OFF_INTERVAL_MS          (1000 * 60 * 1)    // 1 minuta

#define EEPROM_FLASH_INTERVAL        0                                             // interval mezi zablesky
#define EEPROM_FLASHES               (EEPROM_FLASH_INTERVAL + sizeof(uint32_t))    // pocet zablesku

#define BUFFER_LEN                   5


const uint8_t g_CheckStamp = { 100 };
const uint8_t g_FlashStamp = { 200 };


uint8_t RxBuffer[BUFFER_LEN];

volatile FlagStatus g_RxDoneFlag = RESET;
volatile FlagStatus g_RXtimeout = RESET;
volatile FlagStatus g_RXdataDisc = RESET;

volatile FlagStatus g_TxDoneFlag = RESET;
volatile FlagStatus cmdFlag = RESET;
volatile FlagStatus xStartRx = RESET;

uint16_t exitCounter = 0;
uint8_t TxFrameBuff[BUFFER_LEN];

AppState_t g_eState = APP_STATE_IDLE;

uint8_t g_bMaster = 0;

volatile uint16_t g_nDelayTimer;    // odmerovani intervalu
volatile bool g_bDelayOver;

uint8_t  g_nTransmitCounter;          // citani poctu vysilani pro FLASH

void _MasterExec(void);
void _SlaveExec(void);

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

  HW_SetOffInterval(OFF_INTERVAL_MS);
  while (HW_IsButtonPressed_ms());
}

void App_Exec(void)
{
  if (g_bMaster)
  {
    _MasterExec();
  }
  else
  {
    _SlaveExec();
  }

  // kontrola casovace vypnuti (do Standby modu)
  if (HW_GetOffTime() == 0)
  {
    HW_StandbyMode();
  }

  // kontrola stisku tlacitka
  if (HW_IsButtonPressed_ms())
  {
    // Todo: zde by se mel resit stisk tlacitka pri MANUAL modu
    while (!HW_IsButtonPressed_ms());
    HW_DeviceOff();
  }

}

void _MasterExec(void)
{
  static uint32_t nNextCheckTime = 0;

  if (HW_IsInputActive())
  {
    HW_LedBlink(500);
    for (uint8_t i = 0; i < FLASH_TRANSMIT_COUNT; ++i)
    {
      SI4463_SendData((uint8_t*)&g_FlashStamp, sizeof(g_FlashStamp));
    }

    HW_SetOffInterval(FLASH_TRANSMIT_COUNT);
    nNextCheckTime = Timer_GetTicks_ms() + CHECK_INTERVAL_MS;
    App_WaitAfterFlash();
  }

  if (Timer_GetTicks_ms() > nNextCheckTime)
  {
    nNextCheckTime = Timer_GetTicks_ms() + CHECK_INTERVAL_MS;
    HW_LedBlink(100);

    // send CHECK
    SI4463_SendData((uint8_t*)&g_CheckStamp, sizeof(g_CheckStamp));
  }
}

void _SlaveExec(void)
{
  uint8_t buffer[10];
  SI4463_ReadData(buffer, sizeof(buffer), 10000);

  HW_FlashBlink();

}

void App_WaitAfterFlash(void)
{
  // wait 5 ms pro odezneni zablesku
  g_nDelayTimer = 100;  // 100 * 50 us
  while (g_nDelayTimer);
}
