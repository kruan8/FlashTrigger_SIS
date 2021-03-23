/*
 * main.c
 *
 *  Created on: 10. 8. 2016
 *      Author: Priesol Vladimir
 */

#include "stm32l0xx.h"

#include "app.h"

/**
** =================== Application pin asignment ==============================
**
** HW/Pins layout:
**               |   HW       | PINS
**  ------------------------------------------------------------------
**  SI4463       | SPI1       | PA5, PA7, PA6
**       CS      | GPIO-OUT   | PB1
**       SDN     | GPIO-OUT   | PA2
**       IRQ     | GPIO-IN    | PA3
**
**  BUTTON       | GPIO_IN    | PA0
**  LED          | GPIO-OUT   | PA4
**  FLASH-OUTPUT | GPIO-OUT   | PC14
**  FLASH-INPUT  | GPIO_IN    | PA10
* ------------------------------------------------------------------------------
*
* --------------------------------------------------------------------------------
 *
 * Current:
 *   OFF:   MASTER & SLAVE 1,6 uA
 *   ON:
 *     MASTER         SLAVE
 *      4 mA            15 mA
 *
 * --------------------------------------------------------------------------------
 *
 * Button intervals (master & slave):
 *   < 1500ms: ON
 *   > 1500ms: manual flash activation
 *
 * v0.1 - prvni verze (23.3.2021)
 *
 */


int main(void)
{




  App_Init();

  while (1)
  {
    App_Exec();
  }

}
