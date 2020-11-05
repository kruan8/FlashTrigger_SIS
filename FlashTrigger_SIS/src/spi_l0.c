/*
 * SPI1.c
 *
 *  Created on: 13. 3. 2019
 *      Author: priesolv
 *
 *      implementace pro STM32F407
 */

#include <spi_l0.h>
#include "stm32l0xx_ll_gpio.h"


const spi_hw_t spi1_hw =
{
  .reg = SPI1,
  .nGpioAF = LL_GPIO_AF_0,
  .irq = SPI1_IRQn,
};

spi_drv_t _spi1_drv = { &spi1_hw, spi_mode_0, spi_br_256, spi_dir_full_duplex, 0, 0, NULL, NULL, false, false, false };

void spi_Init(spi_drv_t* pDrv, gpio_pins_e eClkPin, gpio_pins_e eMosiPin, gpio_pins_e eMisoPin)
{
  /* SPI SCK pin configuration */
  GPIO_ClockEnable(eClkPin);
  GPIO_ConfigPin(eClkPin, mode_alternate, outtype_pushpull, pushpull_no, speed_veryhigh);
  GPIO_SetAFpin(eClkPin, pDrv->pHW->nGpioAF);

  /* SPI  MOSI pin configuration */
  GPIO_ClockEnable(eMosiPin);
  GPIO_ConfigPin(eMosiPin, mode_alternate, outtype_pushpull, pushpull_no, speed_veryhigh);
  GPIO_SetAFpin(eMosiPin, pDrv->pHW->nGpioAF);

  /* SPI  MISO pin configuration */
  if (eMisoPin != P_UNUSED)
  {
    GPIO_ClockEnable(eMisoPin);
    GPIO_ConfigPin(eMisoPin, mode_alternate, outtype_pushpull, pushpull_no, speed_veryhigh);
    GPIO_SetAFpin(eMisoPin, pDrv->pHW->nGpioAF);
  }

  if (pDrv->pHW->reg == SPI1)
  {
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
  }

//  if (pDrv->pHW->reg == SPI2)
//  {
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
//  }

  /* SPI configuration -------------------------------------------------------*/
  LL_SPI_DeInit(pDrv->pHW->reg);

  LL_SPI_InitTypeDef  SPI_InitStructure;
  SPI_InitStructure.TransferDirection = pDrv->eDirMode;
  SPI_InitStructure.Mode = LL_SPI_MODE_MASTER;
  SPI_InitStructure.DataWidth = LL_SPI_DATAWIDTH_8BIT;
  SPI_InitStructure.ClockPolarity = LL_SPI_POLARITY_LOW;
  SPI_InitStructure.ClockPhase = LL_SPI_PHASE_1EDGE;
  SPI_InitStructure.NSS = LL_SPI_NSS_SOFT;
  SPI_InitStructure.BaudRate = pDrv->ePrescaler;  // SPI speed
  SPI_InitStructure.BitOrder = LL_SPI_MSB_FIRST;
  SPI_InitStructure.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
  LL_SPI_Init(pDrv->pHW->reg, &SPI_InitStructure);

  spi_SetMode(pDrv, pDrv->eMode);
  pDrv->pHW->reg->CR1 |= SPI_CR1_SPE;
}

uint16_t spi_SendData16(spi_drv_t* pDrv, uint16_t nValue)
{
  while (!(pDrv->pHW->reg->SR & LL_SPI_SR_TXE));
  pDrv->pHW->reg->DR = nValue >> 8;
  while (!(pDrv->pHW->reg->SR & LL_SPI_SR_TXE));
  pDrv->pHW->reg->DR = nValue & 0xFF;
  while (pDrv->pHW->reg->SR & LL_SPI_SR_BSY);
  return pDrv->pHW->reg->DR;
}

void spi_SendData16NoWait(spi_drv_t* pDrv, uint16_t nValue)
{
  while (!(pDrv->pHW->reg->SR & LL_SPI_SR_TXE));
  pDrv->pHW->reg->DR = nValue >> 8;
  while (!(pDrv->pHW->reg->SR & LL_SPI_SR_TXE));
  pDrv->pHW->reg->DR = nValue & 0xFF;
}

uint8_t spi_SendData8(spi_drv_t* pDrv, uint8_t nValue)
{
  while (!(pDrv->pHW->reg->SR & LL_SPI_SR_TXE));
  pDrv->pHW->reg->DR = nValue;

  while ((pDrv->pHW->reg->SR & LL_SPI_SR_BSY));
  return pDrv->pHW->reg->DR;
}

void spi_WriteBidirectionalByte(spi_drv_t* pDrv, uint16_t nValue)
{
  while (!(pDrv->pHW->reg->SR & LL_SPI_SR_TXE));
  pDrv->pHW->reg->DR = nValue;
  while (!(pDrv->pHW->reg->SR & LL_SPI_SR_TXE));
  while (pDrv->pHW->reg->SR & LL_SPI_SR_BSY);
}

uint8_t spi_ReadBidirectionalByte(spi_drv_t* pDrv)
{
  spi_SetDirection(pDrv, spi_dir_rx);
  while (!(pDrv->pHW->reg->SR & LL_SPI_SR_RXNE));
  uint8_t nValue = pDrv->pHW->reg->DR;
  spi_SetDirection(pDrv, spi_dir_tx);
  return nValue;
}

void spi_SetPrescaler(spi_drv_t* pDrv, spi_br_e ePrescaler)
{
  // stop SPI peripherial
  pDrv->pHW->reg->CR1 &= (uint16_t)~((uint16_t)SPI_CR1_SPE);

  pDrv->pHW->reg->CR1 = (pDrv->pHW->reg->CR1 & ~(SPI_CR1_BR)) | ePrescaler;

  // start SPI peripherial
  pDrv->pHW->reg->CR1 |= SPI_CR1_SPE;
}

void spi_SetMode(spi_drv_t* pDrv, spi_mode_e eMode)
{
  // stop SPI peripherial
  pDrv->pHW->reg->CR1 &= (uint16_t)~((uint16_t)SPI_CR1_SPE);

  pDrv->pHW->reg->CR1 = (pDrv->pHW->reg->CR1 & ~(SPI_CR1_CPHA | SPI_CR1_CPOL)) | eMode;

  // start SPI peripherial
  pDrv->pHW->reg->CR1 |= SPI_CR1_SPE;
}

void spi_WaitForNoBusy(spi_drv_t* pDrv)
{
  while (pDrv->pHW->reg->SR & LL_SPI_SR_BSY);
}

void spi_SetDirection(spi_drv_t* pDrv, spi_direction_e bDirection)
{
  LL_SPI_SetTransferDirection(pDrv->pHW->reg, bDirection);
}
