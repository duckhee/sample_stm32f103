/***********************************************************************************

  Filename:       hal_rf.c

  Description:    CC2520 radio interface.

***********************************************************************************/

/***********************************************************************************
* INCLUDES
*/

#include "hal_rf.h"
#include "hal_cc2520.h"
#include "hal_defs.h"

/***********************************************************************************
* CONSTANTS AND DEFINES
*/

#define RSSI_OFFSET               76
#define RSSI_OFFSET_LNA_HIGHGAIN  ( RSSI_OFFSET + 8 )
#define RSSI_OFFSET_LNA_LOWGAIN   ( RSSI_OFFSET - 5 )

// CC2590-CC2591 support
//
#ifdef INCLUDE_PA

/* Select CC2591 RX low gain mode */
#define HAL_PA_LNA_RX_LGM() st( CC2520_CFG_GPIO_OUT(3,CC2520_GPIO_LOW); )

/* Select CC2591 RX high gain mode */
#define HAL_PA_LNA_RX_HGM() st( CC2520_CFG_GPIO_OUT(3,CC2520_GPIO_HIGH); )

/* TX power lookup index */
#define HAL_RF_TXPOWER_5_DBM   1
#define HAL_RF_TXPOWER_10_DBM  2
#define HAL_RF_TXPOWER_13_DBM  3
#define HAL_RF_TXPOWER_15_DBM  4
#define HAL_RF_TXPOWER_17_DBM  5

/* TX power values */
#define CC2520_TXPOWER_5_DBM   0x49
#define CC2520_TXPOWER_10_DBM  0x79
#define CC2520_TXPOWER_13_DBM  0x6C
#define CC2520_TXPOWER_15_DBM  0xE0
#define CC2520_TXPOWER_17_DBM  0xF9

#else

/* dummy macros when not using CC2591 */
#define HAL_PA_LNA_RX_LGM()
#define HAL_PA_LNA_RX_HGM()

/* TX power constants */
#define HAL_RF_TXPOWER_MIN_4_DBM   0
#define HAL_RF_TXPOWER_0_DBM       1
#define HAL_RF_TXPOWER_4_DBM       2

#endif


/***********************************************************************************
* LOCAL DATA
*/

#if 0 // POOH
#ifdef INCLUDE_PA
static const menuItem_t pPowerSettings[] =
{
  "5dBm",  HAL_RF_TXPOWER_5_DBM,
  "10dBm", HAL_RF_TXPOWER_10_DBM,
  "13dBm", HAL_RF_TXPOWER_13_DBM,
  "15dBm", HAL_RF_TXPOWER_15_DBM,
  "17dBm", HAL_RF_TXPOWER_17_DBM,
};
#else
static const menuItem_t pPowerSettings[] =
{
    "-4 dBm",  HAL_RF_TXPOWER_MIN_4_DBM,
    "0 dBm",   HAL_RF_TXPOWER_0_DBM,
    "4 dBm",   HAL_RF_TXPOWER_4_DBM
};
#endif
#endif

/***********************************************************************************
* LOCAL DATA TYPES
*/
typedef struct {
    uint8 reg;
    uint8 val;
} regVal_t;


/***********************************************************************************
* LOCAL DATA
*/

// Recommended register settings which differ from the data sheet
static regVal_t regval[]= {
    // Tuning settings
#ifdef INCLUDE_PA
    CC2520_TXPOWER,     0xF9,       // Max TX output power
#else
    CC2520_TXPOWER,     0x32,       // Max TX output power
#endif
    CC2520_CCACTRL0,    0xF8,       // CCA treshold -80dBm

    // Recommended RX settings
    CC2520_MDMCTRL0,    0x85,
    CC2520_MDMCTRL1,    0x14,
    CC2520_RXCTRL,      0x3F,
    CC2520_FSCTRL,      0x5A,
    CC2520_FSCAL1,      0x2B,
#ifdef INCLUDE_PA
    CC2520_AGCCTRL1,    0x16,
#else
    CC2520_AGCCTRL1,    0x11,
#endif
    CC2520_ADCTEST0,    0x10,
    CC2520_ADCTEST1,    0x0E,
    CC2520_ADCTEST2,    0x03,

    // Terminate array
    0,                  0x00
};


#ifdef INCLUDE_PA
static uint8 rssiOffset = RSSI_OFFSET_LNA_HIGHGAIN;
#else
static uint8 rssiOffset = RSSI_OFFSET;
#endif


/***********************************************************************************
* LOCAL FUNCTIONS
*/
#if 0 // POOH
static uint8 halRfWaitRadioReady(void);
static uint8 halRfGetStatusByte(void);
#endif


/***********************************************************************************
* GLOBAL FUNCTIONS
*/

/***********************************************************************************
* @fn      halRfInit
*
* @brief   Power up, sets default tuning settings, enables autoack and configures
*          chip IO
*
* @param   none
*
* @return  HAL_RF_SUCCESS if the radio has started, FAILURE otherwise
*/
uint8 halRfInit(void)
{
    regVal_t* p;

    DEBUG_MSG_FUNC_START;

    // Avoid GPIO0 interrupts during reset
#if 0 // POOH
    halDigioIntDisable(&pinRadio_GPIO0);
#else
    CC2520_GPIO_0_Interrupt_Setting(DISABLE);
#endif

    // Make sure to pull the CC2520 RESETn and VREG_EN pins low
    CC2520_RESET_OPIN(0);
    CC2520_SPI_END();
    CC2520_VREG_EN_OPIN(0);
#if 0 // POOH
    halMcuWaitUs(1100);
#else
    Delay(110);
#endif

    // Make sure MISO is configured as output.
#if 0 // POOH
    CC2520_MISO_DIR_OUT();
#endif

    // Enable the voltage regulator and wait for it (CC2520 power-up)
    CC2520_VREG_EN_OPIN(1);
#if 0 // POOH
    halMcuWaitUs(CC2520_VREG_MAX_STARTUP_TIME);
#else
    Delay(200);
#endif

    // Release reset
    CC2520_RESET_OPIN(1);

#if 0 // POOH
    // Wait for XOSC stable to be announced on the MISO pin
    if (halRfWaitRadioReady()==HAL_RF_FAILED)
        return HAL_RF_FAILED;
#else
    Delay(200);
#endif

    // Write non-default register values
    p= regval;
    while (p->reg!=0) {
        CC2520_MEMWR8(p->reg,p->val);
        p++;
    }

    return HAL_RF_SUCCESS;
}


/***********************************************************************************
* @fn      halRfGetChipId
*
* @brief   Get chip id
*
* @param   none
*
* @return  uint8 - result
*/
uint8 halRfGetChipId(void)
{
    return(CC2520_MEMRD8(CC2520_CHIPID));
}


/***********************************************************************************
* @fn      halRfGetChipVer
*
* @brief   Get chip version
*
* @param   none
*
* @return  uint8 - result
*/
uint8 halRfGetChipVer(void)
{
    return(CC2520_MEMRD8(CC2520_VERSION));
}


/***********************************************************************************
* @fn      halRfGetRandomByte
*
* @brief   Return random byte
*
* @param   none
*
* @return  uint8 - random byte
*/

// Comment: has the random generator been initalised
uint8 halRfGetRandomByte(void)
{
    return CC2520_RANDOM8();
}


/***********************************************************************************
* @fn      halRfGetRssiOffset
*
* @brief   Return RSSI Offset
*
* @param   none
*
* @return  uint8 - RSSI offset
*/
uint8 halRfGetRssiOffset(void)
{
    return rssiOffset;
}


/***********************************************************************************
* @fn      halRfSetChannel
*
* @brief   Set RF channel in the 2.4GHz band. The Channel must be in the range 11-26,
*          11= 2005 MHz, channel spacing 5 MHz.
*
* @param   channel - logical channel number
*
* @return  none
*/
void halRfSetChannel(uint8 channel)
{
    CC2520_REGWR8(CC2520_FREQCTRL, MIN_CHANNEL + ((channel - MIN_CHANNEL) * CHANNEL_SPACING));
}


/***********************************************************************************
* @fn      halRfSetShortAddr
*
* @brief   Write short address to chip
*
* @param   none
*
* @return  none
*/
void halRfSetShortAddr(uint16 shortAddr)
{
    CC2520_MEMWR16(CC2520_RAM_SHORTADDR, shortAddr);
}


/***********************************************************************************
* @fn      halRfSetPanId
*
* @brief   Write PAN Id to chip
*
* @param   none
*
* @return  none
*/
void halRfSetPanId(uint16 panId)
{
    CC2520_MEMWR16(CC2520_RAM_PANID, panId);
}


/***********************************************************************************
* @fn      halRfSetTxPower
*
* @brief   Set TX output power
*
* @param   uint8 power - power level
*
* @return  HAL_RF_SUCCESS or HAL_RF_FAILED
*/
uint8 halRfSetTxPower(uint8 power)
{
    uint8 n;

    switch(power)
    {
#ifdef INCLUDE_PA
    case HAL_RF_TXPOWER_5_DBM: n = CC2520_TXPOWER_5_DBM; break;
    case HAL_RF_TXPOWER_10_DBM: n = CC2520_TXPOWER_10_DBM; break;
    case HAL_RF_TXPOWER_13_DBM: n = CC2520_TXPOWER_13_DBM; break;
    case HAL_RF_TXPOWER_15_DBM: n = CC2520_TXPOWER_15_DBM; break;
    case HAL_RF_TXPOWER_17_DBM: n = CC2520_TXPOWER_17_DBM; break;
#else
    case HAL_RF_TXPOWER_MIN_4_DBM: n = CC2520_TXPOWER_MIN_4_DBM; break;
    case HAL_RF_TXPOWER_0_DBM: n = CC2520_TXPOWER_0_DBM; break;
    case HAL_RF_TXPOWER_4_DBM: n = CC2520_TXPOWER_4_DBM; break;
#endif
    default:
        return HAL_RF_FAILED;
    }
    CC2520_REGWR8(CC2520_TXPOWER, n);
    return HAL_RF_SUCCESS;
}


/***********************************************************************************
* @fn      halRfSetGain
*
* @brief   Set gain mode - only applicable for units with CC2590/91.
*
* @param   uint8 - gain mode
*
* @return  none
*/
void halRfSetGain(uint8 gainMode)
{
    if (gainMode==HAL_RF_GAIN_LOW) {
        HAL_PA_LNA_RX_LGM();
        rssiOffset = RSSI_OFFSET_LNA_LOWGAIN;
    } else {
        HAL_PA_LNA_RX_HGM();
        rssiOffset = RSSI_OFFSET_LNA_HIGHGAIN;
    }
}

/***********************************************************************************
* @fn      halRfWriteTxBuf
*
* @brief   Write to TX buffer
*
* @param   uint8* data - buffer to write
*          uint8 length - number of bytes
*
* @return  none
*/
void halRfWriteTxBuf(uint8* data, uint8 length)
{
    // Copy packet to TX FIFO
    CC2520_TXBUF(length,data);
}


/***********************************************************************************
* @fn      halRfReadRxBuf
*
* @brief   Read RX buffer
*
* @param   uint8* data - data buffer. This must be allocated by caller.
*          uint8 length - number of bytes
*
* @return  none
*/
void halRfReadRxBuf(uint8* data, uint8 length)
{
    CC2520_RXBUF(length, data);
}


/***********************************************************************************
* @fn      halRfTransmit
*
* @brief   Transmit frame with Clear Channel Assessment.
*
* @param   none
*
* @return  uint8 - HAL_RF_SUCCESS or HAL_RF_FAILED
*/
uint8 halRfTransmit(void)
{
    uint16 timeout = 2500; // 2500 x 20us = 50ms
    uint8 status=0;

    // DEBUG_MSG_FUNC_START;

    // Wait for RSSI to become valid
    while(!CC2520_RSSI_VALID_PIN);

    // Reuse GPIO2 for TX_FRM_DONE exception
    HAL_INT_OFF();
    CC2520_CFG_GPIO_OUT(2, 1 + CC2520_EXC_TX_FRM_DONE);
    HAL_INT_ON();

    // Wait for the transmission to begin before exiting (makes sure that this function cannot be called
    // a second time, and thereby cancelling the first transmission.
    while(--timeout > 0) {
        HAL_INT_OFF();
        CC2520_INS_STROBE(CC2520_INS_STXONCCA);
        HAL_INT_ON();
        if (CC2520_SAMPLED_CCA_PIN) break;
#if 0 // POOH
        halMcuWaitUs(20);
#else
        Delay(20);
#endif
    }
    if (timeout == 0) {
        DEBUG_MSG_FUNC_ERROR;
        status = HAL_RF_FAILED;
        CC2520_INS_STROBE(CC2520_INS_SFLUSHTX);
    }
    else {
        status = HAL_RF_SUCCESS;
        // Wait for TX_FRM_DONE exception
        while(!CC2520_TX_FRM_DONE_PIN);
        HAL_INT_OFF();
        CC2520_CLEAR_EXC(CC2520_EXC_TX_FRM_DONE);
        HAL_INT_ON();
    }

    // Reconfigure GPIO2
    HAL_INT_OFF();
    CC2520_CFG_GPIO_OUT(2,     CC2520_GPIO_RSSI_VALID);
    HAL_INT_ON();
    return status;
}


/***********************************************************************************
* @fn      halRfReceiveOn
*
* @brief   Turn receiver on
*
* @param   none
*
* @return  none
*/
void halRfReceiveOn(void)
{
    CC2520_INS_STROBE(CC2520_INS_SRXON);
}

/***********************************************************************************
* @fn      halRfReceiveOff
*
* @brief   Turn receiver off
*
* @param   none
*
* @return  none
*/
void halRfReceiveOff(void)
{
    CC2520_INS_STROBE(CC2520_INS_SRFOFF);
}


/***********************************************************************************
* @fn      halRfDisableRxInterrupt
*
* @brief   Clear and disable RX interrupt.
*
* @param   none
*
* @return  none
*/
void halRfDisableRxInterrupt(void)
{
    // Clear the exception and the IRQ
    CLEAR_EXC_RX_FRM_DONE();
#if 0 // POOH
    halDigioIntClear(&pinRadio_GPIO0);
    halDigioIntDisable(&pinRadio_GPIO0);
#else
    EXTI_ClearITPendingBit(GPIO_EXTI_Line_RF_GPIO0);
    CC2520_GPIO_0_Interrupt_Setting(DISABLE);
#endif
}


/***********************************************************************************
* @fn      halRfEnableRxInterrupt
*
* @brief   Enable RX interrupt.
*
* @param   none
*
* @return  none
*/
void halRfEnableRxInterrupt(void)
{
#if 0 // POOH
    halDigioIntEnable(&pinRadio_GPIO0);
#else
    CC2520_GPIO_0_Interrupt_Setting(ENABLE);
#endif
}


/***********************************************************************************
* @fn      halRfRxInterruptConfig
*
* @brief   Enable RX interrupt.
*
* @param   none
*
* @return  none
*/
void halRfRxInterruptConfig(ISR_FUNC_PTR pfISR)
{
#if 0 // POOH
    halDigioIntSetEdge(&pinRadio_GPIO0, HAL_DIGIO_INT_RISING_EDGE);
    halDigioIntConnect(&pinRadio_GPIO0, pfISR);
    halDigioIntEnable(&pinRadio_GPIO0);

    // And clear the exception
    CLEAR_EXC_RX_FRM_DONE();
#endif
}

/***********************************************************************************
* @fn      halRfWaitTransceiverReady
*
* @brief   Wait until the transceiver is ready (SFD low).
*
* @param   none
*
* @return  none
*/
void halRfWaitTransceiverReady(void)
{
#ifdef INCLUDE_PA
    // GPIO3 is not conncted to combo board; use SFD at GPIO2 instead
    HAL_INT_OFF();
    // GPIO2 = SFD
    CC2520_CFG_GPIO_OUT(2,CC2520_GPIO_SFD);
    while (CC2520_GPIO2_IPIN);
    // GPIO2 = default (RSSI_VALID)
    CC2520_CFG_GPIO_OUT(2,CC2520_GPIO_RSSI_VALID);
    HAL_INT_ON();
#else
    while (CC2520_SFD_PIN);
#endif
}

#if 0 // POOH

/***********************************************************************************
* LOCAL FUNCTIONS
*/
/***********************************************************************************
* @fn      halRfWaitRadioReady
*
* @brief   Wait for the crystal oscillator to stabilise.
*
* @param   none
*
* @return  HAL_RF_SUCCESS if oscillator starts, HAL_RF_FAILED otherwise
*/
static uint8 halRfWaitRadioReady(void)
{
    uint8 i;

    // Wait for XOSC stable to be announced on the MISO pin
    i= 100;
    CC2520_CSN_OPIN(0);
    while (i>0 && !CC2520_MISO_IPIN) {
        halMcuWaitUs(10);
        --i;
    }
    CC2520_CSN_OPIN(1);

    return i>0 ? HAL_RF_SUCCESS : HAL_RF_FAILED;
}

/***********************************************************************************
* @fn      halRfGetStatusByte
*
* @brief   Get chip status byte
*
* @param   none
*
* @return  uint8 - chip status byte
*/
uint8 halRfGetStatusByte(void)
{
    return CC2520_INS_STROBE(CC2520_INS_SNOP);
}

#endif


/***********************************************************************************
  Copyright 2007 Texas Instruments Incorporated. All rights reserved.

  IMPORTANT: Your use of this Software is limited to those specific rights
  granted under the terms of a software license agreement between the user
  who downloaded the software, his/her employer (which must be your employer)
  and Texas Instruments Incorporated (the "License").  You may not use this
  Software unless you agree to abide by the terms of the License. The License
  limits your use, and you acknowledge, that the Software may not be modified,
  copied or distributed unless embedded on a Texas Instruments microcontroller
  or used solely and exclusively in conjunction with a Texas Instruments radio
  frequency transceiver, which is integrated into your product.  Other than for
  the foregoing purpose, you may not use, reproduce, copy, prepare derivative
  works of, modify, distribute, perform, display or sell this Software and/or
  its documentation for any purpose.

  YOU FURTHER ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE
  PROVIDED “AS IS?WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED,
  INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, TITLE,
  NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL
  TEXAS INSTRUMENTS OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT,
  NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR OTHER
  LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
  INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE
  OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT
  OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
  (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.

  Should you have any questions regarding your right to use this Software,
  contact Texas Instruments Incorporated at www.TI.com.
***********************************************************************************/
