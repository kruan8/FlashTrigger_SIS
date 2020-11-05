/*
 * spirit_gpio.c
 *
 *  Created on: 24. 8. 2016
 *      Author: Priesol Vladimir
 */

#include "spirit.h"
#include "spirit_spi.h"
#include "SPIRIT_Config.h"

// Radio structure fitting
SRadioInit xRadioInit =
{
  .nXtalOffsetPpm = XTAL_OFFSET_PPM,
  .lFrequencyBase = BASE_FREQUENCY,
  .nChannelSpace = CHANNEL_SPACE,
  .cChannelNumber = CHANNEL_NUMBER,
  .xModulationSelect = MODULATION_SELECT,
  .lDatarate = DATARATE,
  .lFreqDev = FREQ_DEVIATION,
  .lBandwidth = BANDWIDTH
};

PktBasicInit xBasicInit=
{
  .xPreambleLength = PREAMBLE_LENGTH,
  .xSyncLength = SYNC_LENGTH,
  .lSyncWords = SYNC_WORD,
  .xFixVarLength = LENGTH_TYPE,
  .cPktLengthWidth = LENGTH_WIDTH,
  .xCrcMode = CRC_MODE,
  .xControlLength = CONTROL_LENGTH,
  .xAddressField = S_ENABLE,
  .xFec = EN_FEC,
  .xDataWhitening = EN_WHITENING
};

PktBasicAddressesInit xAddressInit=
{
  .xFilterOnMyAddress = EN_FILT_MY_ADDRESS,
  .cMyAddress = MY_ADDRESS,
  .xFilterOnMulticastAddress = EN_FILT_MULTICAST_ADDRESS,
  .cMulticastAddress = MULTICAST_ADDRESS,
  .xFilterOnBroadcastAddress = EN_FILT_BROADCAST_ADDRESS,
  .cBroadcastAddress = BROADCAST_ADDRESS
};


#define SPIRIT1_SDN_PIN             (1 << 2)  // out
#define SPIRIT1_SDN_GPIO_PORT       GPIOA

#define SPIRIT1_GPIO3_PIN           (1 << 10) // in
#define SPIRIT1_GPIO3_GPIO_PORT     GPIOA

#define SPIRIT1_SDN_INACTIVE        (SPIRIT1_SDN_GPIO_PORT->BSRR = SPIRIT1_SDN_PIN)
#define SPIRIT1_SDN_ACTIVE          (SPIRIT1_SDN_GPIO_PORT->BRR = SPIRIT1_SDN_PIN)

#define POR_TIME                    ((uint16_t)0x1E00)


Ptr_OnGPIO3_EXTI g_pOnGPIO3_EXTI;

void Spirit_Init(Ptr_OnGPIO3_EXTI pOnGPIO3Exti)
{
  RCC->IOPENR |= RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOCEN;

  // Configure SPIRIT SDN pin as output
  GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODE2))| (GPIO_MODER_MODE2_0);

  // Configure SPIRIT GPIO3 pin as input
  GPIOA->MODER = (GPIOA->MODER & ~(GPIO_MODER_MODE10));

  // pull up
  GPIOA->PUPDR = (GPIOA->PUPDR & ~(GPIO_PUPDR_PUPD10)) | (GPIO_PUPDR_PUPD10_0);

  //SYSCFG->EXTICR[0] &= (uint16_t)~SYSCFG_EXTICR1_EXTI0_PA; /* (3) */
  EXTI->IMR |= SPIRIT1_GPIO3_PIN; // Configure the corresponding mask bit in the EXTI_IMR register
  EXTI->FTSR |= SPIRIT1_GPIO3_PIN; // Configure the Trigger Selection bits of the Interrupt line (falling edge)
  //EXTI->FTSR |= 0x0001; /* (6) */

  g_pOnGPIO3_EXTI = pOnGPIO3Exti;

  SPIspirit_init();

}

// Puts at logic 1 the SDN pin.
void Spirit_EnterShutdown(void)
{
  SPIRIT1_SDN_INACTIVE;
}

// Put at logic 0 the SDN pin.
void Spirit_ExitShutdown(void)
{
  SPIRIT1_SDN_ACTIVE;

  /* Delay to allow the circuit POR, about 700 us */
  for (volatile uint32_t i = 0; i < POR_TIME; i++);
}

void Spirit_EnableIRQ(void)
{
  // Configure NVIC for Extended Interrupt
  NVIC_SetPriority(EXTI4_15_IRQn, 2);
  NVIC_EnableIRQ(EXTI4_15_IRQn);
}

void Spirit_DisableIRQ(void)
{
  NVIC_DisableIRQ(EXTI4_15_IRQn);
}

void EXTI4_15_IRQHandler(void)
{
  // EXTI line 10 interrupt detected
  if (EXTI->PR & SPIRIT1_GPIO3_PIN)
  {
    EXTI->PR = SPIRIT1_GPIO3_PIN; // Clear interrupt flag
    if (g_pOnGPIO3_EXTI)
    {
      g_pOnGPIO3_EXTI();
    }
  }
}

void Spirit_WriteReg(uint8_t nRegAddr, uint8_t nValue)
{
  SPIspirit_WriteRegisters(nRegAddr, 1, &nValue);
}

uint8_t Spirit_ReadReg(uint8_t nRegAddr)
{
  uint8_t nValue;
  SPIspirit_ReadRegisters(nRegAddr, 1, &nValue);
  return nValue;
}

void Spirit_WriteCommand(uint8_t nCommand, SpiritState state)
{
  SpiritRefreshStatus();
  SPIspirit_CommandStrobes(nCommand);
  do {
     /* Delay for state transition */
     for(volatile uint8_t i=0; i!=0xFF; i++);

     /* Reads the MC_STATUS register */
     SpiritRefreshStatus();
   } while(g_xStatus.MC_STATE != state);

}

void Spirit_InitRegs(bool bMaster)
{
  Spirit_WriteReg(SYNTH_CONFIG0_BASE, 160);      // 0x9F  split time=3.47 ns
  Spirit_WriteCommand(COMMAND_STANDBY, MC_STATE_STANDBY);

  Spirit_WriteReg(XO_RCO_TEST_BASE, 33);
  Spirit_WriteCommand(COMMAND_READY, MC_STATE_READY);

  Spirit_WriteReg(0xA3, 53);                    // (DEM_CONFIG)  enable initialization
  Spirit_WriteReg(IF_OFFSET_ANA_BASE, 0x36);      // Intermediate frequency setting for the analog RF synthesizer.

  Spirit_WriteReg(ANA_FUNC_CONF0_BASE, 192);

  // Channel number. This value is  multiplied by the channel
  // spacing and added to the synthesizer base frequency to
  // generate the actual RF carrier  frequency.
  Spirit_WriteReg(CHNUM_BASE, 0);        //

  Spirit_WriteReg(CHSPACE_BASE, 14);           //
  Spirit_WriteReg(IF_OFFSET_DIG_BASE, 172);    // 0x0D (IF_OFFSET_DIG) Intermediate frequency setting for the digital shift-to-baseband
  Spirit_WriteReg(FC_OFFSET1_BASE, 0);         // 0x0E (FC_OFFSET[1])  Carrier offset in steps of fXO/218
  Spirit_WriteReg(FC_OFFSET0_BASE, 0);         // 0x0F (FC_OFFSET[0])

  // Radio configuration
  // datarate:
  // reset hodnoty (0x83, 0xA) jsou pro 38400
  // vzorec Excelu pro vypocet nastaveni datarate '=26000000*((256+AN14)*POWER(2;AN15))/POWER(2;28)' AN14=mantisa, AN15=exponent, AN16=vysledek

  Spirit_WriteReg(MOD1_BASE, 131);   // The mantissa value of the DATARATE equation (131/0x83 pro 38400) 19200:mant=131,exp=9
  Spirit_WriteReg(MOD0_BASE, 10);   // The exponent value of the DATARATE equation ( 10/0x0A pro 38400)
  SpiritRadioSetModulation(MODULATION_SELECT);

  Spirit_WriteReg(FDEV0_BASE, 0x45);  // Sets the Mantissa and exponent of frequency deviation (frequency separation/2) and PLL or DLL alogrithm from clock recovery in RX digital demod
  Spirit_WriteReg(CHFLT_BASE, 0x23);   // Channel Filter Bandwidth (100kHz)

  Spirit_WriteReg(AFC2_BASE, 200);  // Automatic frequency compensation algorithm parameters (FSK/GFSK/MSK)
  Spirit_WriteReg(153, 128);      // 0x99
  Spirit_WriteReg(154, 227);      // 0x9A
  Spirit_WriteReg(SYNTH_CONFIG1_BASE, 91);       // fREF = fXO frequency  delicka krystalu

  Spirit_SetFrequency();

//  SpiritRadioInit(&xRadioInit);
//  Spirit_Calibrate(bMaster);
}

void Spirit_SetFrequency()
{
  // nastaveni syntezatoru SYNT0 - SYNT3
#ifdef USE_SPIRIT1_868MHz  // for 848 MHz
  Spirit_WriteReg(SYNT3_BASE, 6);
  Spirit_WriteReg(SYNT2_BASE, 130);
  Spirit_WriteReg(SYNT1_BASE, 143);
  Spirit_WriteReg(SYNT0_BASE, 89);
#endif

#ifdef USE_SPIRIT1_915MHz  // for 915 MHz
  Spirit_WriteReg(SYNT3_BASE, 134);
  Spirit_WriteReg(SYNT2_BASE, 220);
  Spirit_WriteReg(SYNT1_BASE, 204);
  Spirit_WriteReg(SYNT0_BASE, 201);
#endif
}


/** @brief  This function initializes the BASIC Packet handler of spirit1
* @param  None
* @retval None
*/

/*
 *   |   1-32   | 1-4  | 0-16 bit |   0-1   |   0-4   | 0-65535 | 0-3 |
 *   | Preamble | Sync |  Length  | Address | Control | Payload | CRC |
 *
 *  Preamble (programmable field): the length of the preamble is programmable from 1 to 32
    bytes by the PREAMBLE_LENGTH field of the PCKTCTRL2 register. Each preamble byte is
    a '10101010' binary sequence.

    Sync (programmable field): the length of the synchronization field is programmable (from 1
    to 4 bytes) through dedicated registers. The SYNC word is programmable through registers
    SYNC1, SYNC2, SYNC3, and SYNC4. If the programmed sync length is 1, then only SYNC
    word is transmitted; if the programmed sync length is 2 then only SYNC1 and SYNC2 words
    are transmitted and so on.

    Length (programmable/optional field): the packet length field is an optional field that is
    defined as the cumulative length of Address, Control, and Payload fields. It is possible to
    support fixed and variable packet length. In fixed mode, the field length is not used.

    Destination address (programmable/optional field): when the destination address filtering
    is enabled in the receiver, the packet handler engine compares the destination address field
    of the packet received with the value of register TX_SOURCE_ADDR. If broadcast address
    and/or multicast address filtering are enabled, the packet handler engine compares the
    destination address with the programmed broadcast and/or multicast address.

    Control (programmable/optional field): is programmable from 0 to 4 bytes through the
    CONTROL_LEN field of the PCKTCTRL4 register. Control fields of the packet can be set
    using the TX_CTRL_FIELD[3:0] register.

    Payload (programmable/optional field): the device supports both fixed and variable payload
    length transmission from 0 to 65535 bytes.
 */

void Spirit_BasicProtocolInit(void)
{
  SpiritPktBasicSetFormat();

  // Spirit Packet config
  SpiritPktBasicInit(&xBasicInit);



  SpiritPktBasicAddressesInit(&xAddressInit);
}

void Spirit_Calibrate(bool bMaster)
{
  // start calibration
  Spirit_WriteReg(VCO_CONFIG_BASE, 0x25);  // Set the VCO current
  Spirit_WriteReg(PROTOCOL2_BASE, PROTOCOL2_VCO_CALIBRATION_MASK); // enable the automatic VCO calibration

  uint8_t nVCOReg;
  if (bMaster)
  {
    // master - kalibrace vysilace
    Spirit_WriteCommand(COMMAND_LOCKTX, MC_STATE_LOCK);

    /* Read the VCO calibration word from VCO_CALIBR_DATA, register RCO_VCO_CALIBR_OUT[0] (register address 0xE5).
     * Write the value read into the VCO_CALIBR_TX, in register RCO_VCO_CALIBR_IN[1] (register address 0x6E);
     * optionally this value can be saved in the micro NVM.
     */

    //RCO_VCO_CALIBR_OUT0_BASE -> RCO_VCO_CALIBR_IN1_BASE
    nVCOReg = RCO_VCO_CALIBR_IN1_BASE;
  }
  else
  {
    // slave - kalibrace prijimace
    Spirit_WriteCommand(COMMAND_LOCKRX, MC_STATE_LOCK);

    /* Read the VCO calibration word from VCO_CALIBR_DATA, register RCO_VCO_CALIBR_OUT[0] (register address 0xE5).
     * Write the value read into the VCO_CALIBR_RX, in register RCO_VCO_CALIBR_IN[0] (register address 0x6F);
     * optionally this value can be saved in the micro NVM
     */

    // RCO_VCO_CALIBR_OUT0_BASE -> RCO_VCO_CALIBR_IN0_BASE
    nVCOReg = RCO_VCO_CALIBR_IN0_BASE;
  }

  uint8_t nValue = Spirit_ReadReg(RCO_VCO_CALIBR_OUT0_BASE);
  uint8_t nTmp = Spirit_ReadReg(nVCOReg);
  nTmp &= 0x80;
  nTmp |= nValue;
  Spirit_WriteReg(nVCOReg, nTmp);

  Spirit_WriteCommand(COMMAND_READY, MC_STATE_READY);

  Spirit_WriteReg(PROTOCOL2_BASE, 0); // disable the automatic VCO calibration

  Spirit_WriteReg(VCO_CONFIG_BASE, 17);    // Set the VCO current

  // end calibration
}

void Spirit_SetPowerRegs(void)
{
  Spirit_WriteReg(PA_POWER8_BASE, 1);  // Output power level for 8th slot (+12 dBm)

  // Final level for power ramping or selected output power index
  Spirit_WriteReg(PA_POWER0_BASE, PA_POWER0_PA_LEVEL_MAX_INDEX_7);
}

void Spirit_EnableSQI(void)
{
  Spirit_WriteReg(QI_BASE, 2);
}

void Spirit_SetRssiThreshold(void)
{
  Spirit_WriteReg(RSSI_TH_BASE, 20);

}

void Spirit_ReadRegs(uint8_t nRegAddr, uint8_t nLenght, uint8_t *pBuffer)
{
  SPIspirit_ReadRegisters(nRegAddr, nLenght, pBuffer);
}
