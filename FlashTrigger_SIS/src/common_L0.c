/*
 * common.c
 *
 *  Created on: 9. 8. 2016
 *      Author: priesolv
 */

#include <common_L0.h>

void GPIO_ClockEnable(gpio_pins_e ePortPin)
{
  uint16_t nPort = ((uint32_t)GET_PORT(ePortPin) - (GPIOA_BASE)) / ((GPIOB_BASE) - (GPIOA_BASE));
  RCC->IOPENR |= (1 << nPort);
}

void GPIO_ClockDisable(gpio_pins_e ePortPin)
{
  uint16_t nPort = ((uint32_t)GET_PORT(ePortPin) - (GPIOA_BASE)) / ((GPIOB_BASE) - (GPIOA_BASE));
  RCC->IOPENR &= ~(1 << nPort);
}

GPIO_TypeDef* GPIO_GetPort(gpio_pins_e ePortPin)
{
  GPIO_TypeDef* port;
  port = (GPIO_TypeDef*)(GPIOA_BASE + ((ePortPin >> 4) * ((GPIOB_BASE) - (GPIOA_BASE))));
  return port;
}

uint16_t GPIO_GetPin(gpio_pins_e ePortPin)
{
  return (1 << (ePortPin & 0x0F));
}

void GPIO_SetAFpin(gpio_pins_e ePortPin, uint8_t nAF)
{
  uint32_t nPin = ePortPin & 0xF;
  if (nPin < 8)
  {
    MODIFY_REG(GET_PORT(ePortPin)->AFR[0], GPIO_AFRL_AFSEL0 << (nPin << 2), nAF << (nPin << 2));
  }
  else
  {
    nPin -= 8;
    MODIFY_REG(GET_PORT(ePortPin)->AFR[1], GPIO_AFRH_AFSEL8 << (nPin << 2), nAF << (nPin << 2));
  }
}


uint16_t GPIO_GetPinSource(uint16_t GPIO_Pin)
{
  uint16_t pinsource = 0;

  /* Get pinsource */
  while (GPIO_Pin > 1)
  {
    pinsource++;
    GPIO_Pin >>= 1;
  }

  /* Return source */
  return pinsource;
}

void GPIO_ConfigPin(gpio_pins_e ePin, pin_mode_e eMode, pin_output_type_e eOutType, pin_pushpull_e ePull, pin_speed_e eSpeed)
{
  LL_GPIO_SetPinMode(GET_PORT(ePin), GET_PIN(ePin), eMode);

  LL_GPIO_SetPinOutputType(GET_PORT(ePin), GET_PIN(ePin), eOutType);

  LL_GPIO_SetPinPull(GET_PORT(ePin), GET_PIN(ePin), ePull);

  LL_GPIO_SetPinSpeed(GET_PORT(ePin), GET_PIN(ePin), eSpeed);
}
