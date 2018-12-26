/*
 * cc2520_mango_porting.c
 */

#include "cc2520_mango_porting.h"
#include "misc.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"

void CC2520_GPIO_0_Interrupt_Setting(FunctionalState  fStateE)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;

    /* Configure one bit for preemption priority */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    /* Enable the EXTI8 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel                   = EXTI9_5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd                = fStateE;
    NVIC_Init(&NVIC_InitStructure);

    /* Connect EXTI Line to gpio pin */
    GPIO_EXTILineConfig(GPIO_PORTSOURCE_RF_GPIO0, GPIO_PINSOURCE_RF_GPIO0);

    /* Configure EXTI Line to generate an interrupt */
    EXTI_ClearITPendingBit(GPIO_EXTI_Line_RF_GPIO0);
    EXTI_InitStructure.EXTI_Line    = GPIO_EXTI_Line_RF_GPIO0;
    EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = fStateE;
    EXTI_Init(&EXTI_InitStructure);
}

// Outputs: Power and reset control
// #define CC2520_RESET_OPIN(v)            MCU_IO_SET(5,7,v)
// #define CC2520_VREG_EN_OPIN(v)          MCU_IO_SET(1,0,v)

void CC2520_RESET_OPIN(u8 data)
{
    if(0 == data)
    {
        GPIO_ResetBits(GPIO_RF_RESET, GPIO_RF_RESET_PIN);
    }
    else
    {
        GPIO_SetBits(GPIO_RF_RESET, GPIO_RF_RESET_PIN);
    }
}

void CC2520_VREG_EN_OPIN(u8 data)
{
    if(0 == data)
    {
        GPIO_ResetBits(GPIO_RF_VREG_EN, GPIO_RF_VREG_EN_PIN);
    }
    else
    {
        GPIO_SetBits(GPIO_RF_VREG_EN, GPIO_RF_VREG_EN_PIN);
    }
}

// SPI access macros
// #define CC2520_SPI_BEGIN()      st( CC2520_CSN_OPIN(0); )
// #define CC2520_SPI_TX(x)        st( CC2520_SPI_RX_NOT_READY(); CC2520_SPI_TX_REG = x; )
// #define CC2520_SPI_RX()         (CC2520_SPI_RX_REG)
// #define CC2520_SPI_WAIT_RXRDY() st( while (!CC2520_SPI_RX_IS_READY()); )
// #define CC2520_SPI_END()        st( CC2520_CSN_OPIN(1); )

void CC2520_SPI_TX(u8 data)
{
    /* Wait for SPI1 Tx buffer empty */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    /* Send SPI1 data */
    SPI_I2S_SendData(SPI1, data);
}

u8 CC2520_SPI_RX(void)
{
    return SPI_I2S_ReceiveData(SPI1);
}

void CC2520_SPI_WAIT_RXRDY(void)
{
    /* Wait for SPI1 data reception */
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
}

