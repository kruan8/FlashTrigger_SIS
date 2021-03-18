/*
 * app.h
 *
 *  Created on: 6. 11. 2020
 *      Author: Priesol Vladimir
 */

#ifndef APP_H_
#define APP_H_

#include "stm32l0xx.h"
#include <stdbool.h>

typedef enum
{
  APP_STATE_IDLE = 0,
  APP_STATE_PROG,
  APP_STATE_START,
  APP_STATE_MANUAL_TRIGGER,
  APP_STATE_START_RX,
  APP_STATE_WAIT_FOR_RX_DONE,
  APP_STATE_DATA_RECEIVED,
  APP_STATE_SEND_CHECK,
  APP_STATE_SEND_FLASH,
  APP_STATE_WAIT_FOR_TX_DONE_CHECK,
  APP_STATE_WAIT_FOR_TX_DONE_FLASH,
} AppState_t;

void App_Exec(void);
void App_Init(void);

#endif /* APP_H_ */
