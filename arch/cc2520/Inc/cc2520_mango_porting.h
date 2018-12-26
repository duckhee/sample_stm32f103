/*
 * cc2520_mango_porting.h
 */

#ifndef __CC2520_MANGO_PORTING_H
#define __CC2520_MANGO_PORTING_H

/* Includes ------------------------------------------------------------------*/

#include "hw_config.h"
#include "stm32f10x_spi.h"
#include "hal_types.h"
#include "stm32f10x_gpio.h"

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

#define HAL_INT_ON(x)      ;
#define HAL_INT_OFF(x)     ;
#define HAL_INT_LOCK(x)    ;
#define HAL_INT_UNLOCK(x)  ;

#define CC2520_SPI_BEGIN()  GPIO_ResetBits(GPIO_RF_SPI, GPIO_RF_SPI_NCS_PIN)
#define CC2520_SPI_END()    GPIO_SetBits(GPIO_RF_SPI, GPIO_RF_SPI_NCS_PIN)

// CC2520 pin assignments
#define CC2520_RX_FRM_DONE_PIN   \
                    GPIO_ReadInputDataBit(GPIO_RF_GPIO0_4, GPIO_RF_GPIO0_PIN)
#define CC2520_SAMPLED_CCA_PIN   \
                    GPIO_ReadInputDataBit(GPIO_RF_GPIO0_4, GPIO_RF_GPIO1_PIN)
// GPIO2 are reused for RSSI Valid or TX_FRM_DONE
#define CC2520_RSSI_VALID_PIN    \
                    GPIO_ReadInputDataBit(GPIO_RF_GPIO0_4, GPIO_RF_GPIO2_PIN)
#define CC2520_TX_FRM_DONE_PIN   \
                    GPIO_ReadInputDataBit(GPIO_RF_GPIO0_4, GPIO_RF_GPIO2_PIN)
#define CC2520_SFD_PIN           \
                    GPIO_ReadInputDataBit(GPIO_RF_GPIO0_4, GPIO_RF_GPIO3_PIN)
#define CC2520_GPIO_SNIFFER_DATA_PIN  \
                    GPIO_ReadInputDataBit(GPIO_RF_GPIO0_4, GPIO_RF_GPIO4_PIN)
#define CC2520_GPIO_SNIFFER_CLK_PIN   \
                    GPIO_ReadInputDataBit(GPIO_RF_GPIO0_4, GPIO_RF_GPIO5_PIN)

/* External variables --------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */

void   halIntOn(void);
void   halIntOff(void);
uint16 halIntLock(void);
void   halIntUnlock(uint16 key);

void CC2520_GPIO_0_Interrupt_Setting(FunctionalState  fStateE);

void CC2520_RESET_OPIN(u8 data);
void CC2520_VREG_EN_OPIN(u8 data);

void CC2520_SPI_TX(u8 data);
u8 CC2520_SPI_RX(void);
void CC2520_SPI_WAIT_RXRDY(void);

#endif  /* __CC2520_MANGO_PORTING_H */
