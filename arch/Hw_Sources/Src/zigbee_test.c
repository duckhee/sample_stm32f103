/*
 * zigbee_test.c
 */

#ifdef BOARD_DEF_MANGO_Z1

/* Includes ------------------------------------------------------------------*/
#include "zigbee_test.h"
#include "hal_rf.h"
#include "hw_config.h"
#include "hal_cc2520.h"
#include "string.h"
#include "basic_rf.h"
#include "debug.h"

/***********************************************************************************
 * CONSTANTS
 */

// PER test packet format
typedef struct {
    uint32 seqNumber;
    uint8 padding[100];
} perTestPacket_t;

// PER test receiver statistics
typedef struct {
    uint32 expectedSeqNum;
    uint32 rcvdPkts;
    uint32 lostPkts;
    int16 rssiSum;
} perRxStats_t;

#define KVP(v)	{ v, #v }

typedef struct {
    const uint8 iRegNum;
    const char *szRegName;
} regKvp_t;

// Application parameters
#define RF_CHANNEL                25      // 2.4 GHz RF channel

// BasicRF address definitions
#define PAN_ID               0x2007
#define TX_ADDR              0x2520
#define RX_ADDR              0xBEEF

// transmit data
#define APP_PAYLOAD_LENGTH    1
#define MAX_PAYLOAD_LENGTH    104
#define PACKET_SIZE           sizeof(perTestPacket_t)
#define RSSI_AVG_WINDOW_SIZE  32  // Window size for RSSI moving average

// Burst Sizes
#define BURST_SIZE_1            1000
#define BURST_SIZE_2            10000
#define BURST_SIZE_3            100000
#define BURST_SIZE_4            1000000

// Iteration count
#define ITERATION_COUNT         0xF

/***********************************************************************************
 * LOCAL VARIABLES
 */

static perTestPacket_t txPacket;
static perTestPacket_t rxPacket;

static uint8 pTxData[APP_PAYLOAD_LENGTH];
static uint8 pRxData[APP_PAYLOAD_LENGTH];
static basicRfCfg_t basicRfConfig;

#ifdef SECURITY_CCM
// Security key
static uint8 key[]= {
    0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7,
    0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf,
};
#endif

// CC2520 SREG names
static regKvp_t regLookup_SREG[]=
{
    KVP(CC2520_CHIPID),
    KVP(CC2520_VERSION),
    KVP(CC2520_EXTCLOCK),
    KVP(CC2520_MDMCTRL0),
    KVP(CC2520_MDMCTRL1),
    KVP(CC2520_FREQEST),
    KVP(CC2520_RXCTRL),
    KVP(CC2520_FSCTRL),
    KVP(CC2520_FSCAL0),
    KVP(CC2520_FSCAL1),
    KVP(CC2520_FSCAL2),
    KVP(CC2520_FSCAL3),
    KVP(CC2520_AGCCTRL0),
    KVP(CC2520_AGCCTRL1),
    KVP(CC2520_AGCCTRL2),
    KVP(CC2520_AGCCTRL3),
    KVP(CC2520_ADCTEST0),
    KVP(CC2520_ADCTEST1),
    KVP(CC2520_ADCTEST2),
    KVP(CC2520_MDMTEST0),
    KVP(CC2520_MDMTEST1),
    KVP(CC2520_DACTEST0),
    KVP(CC2520_DACTEST1),
    KVP(CC2520_ATEST),
    KVP(CC2520_DACTEST2),
    KVP(CC2520_PTEST0),
    KVP(CC2520_PTEST1),
    KVP(CC2520_DPUBIST),
    KVP(CC2520_ACTBIST),
    KVP(CC2520_RAMBIST),
    KVP(0xFF),
};

// CC2520 FREG names
static regKvp_t regLookup_FREG[]=
{
    KVP(CC2520_FRMFILT0),
    KVP(CC2520_FRMFILT1),
    KVP(CC2520_SRCMATCH),
    KVP(CC2520_SRCSHORTEN0),
    KVP(CC2520_SRCSHORTEN1),
    KVP(CC2520_SRCSHORTEN2),
    KVP(CC2520_SRCEXTEN0),
    KVP(CC2520_SRCEXTEN1),
    KVP(CC2520_SRCEXTEN2),
    KVP(CC2520_FRMCTRL0),
    KVP(CC2520_FRMCTRL1),
    KVP(CC2520_RXENABLE0),
    KVP(CC2520_RXENABLE1),
    KVP(CC2520_EXCFLAG0),
    KVP(CC2520_EXCFLAG1),
    KVP(CC2520_EXCFLAG2),
    KVP(CC2520_EXCMASKA0),
    KVP(CC2520_EXCMASKA1),
    KVP(CC2520_EXCMASKA2),
    KVP(CC2520_EXCMASKB0),
    KVP(CC2520_EXCMASKB1),
    KVP(CC2520_EXCMASKB2),
    KVP(CC2520_EXCBINDX0),
    KVP(CC2520_EXCBINDX1),
    KVP(CC2520_EXCBINDY0),
    KVP(CC2520_EXCBINDY1),
    KVP(CC2520_GPIOCTRL0),
    KVP(CC2520_GPIOCTRL1),
    KVP(CC2520_GPIOCTRL2),
    KVP(CC2520_GPIOCTRL3),
    KVP(CC2520_GPIOCTRL4),
    KVP(CC2520_GPIOCTRL5),
    KVP(CC2520_GPIOPOLARITY),
    KVP(CC2520_GPIOCTRL),
    KVP(CC2520_DPUCON),
    KVP(CC2520_DPUSTAT),
    KVP(CC2520_FREQCTRL),
    KVP(CC2520_FREQTUNE),
    KVP(CC2520_TXPOWER),
    KVP(CC2520_TXCTRL),
    KVP(CC2520_FSMSTAT0),
    KVP(CC2520_FSMSTAT1),
    KVP(CC2520_FIFOPCTRL),
    KVP(CC2520_FSMCTRL),
    KVP(CC2520_CCACTRL0),
    KVP(CC2520_CCACTRL1),
    KVP(CC2520_RSSI),
    KVP(CC2520_RSSISTAT),
    KVP(CC2520_TXFIFO_BUF),
    KVP(CC2520_RXFIRST),
    KVP(CC2520_RXFIFOCNT),
    KVP(CC2520_TXFIFOCNT),
    KVP(0xFF),
};

#define LED_ONOFF_DELAY  50

static void led_tx_ok(void)
{
    LED_Off_All();

    LED_On_Blue();
    Delay(LED_ONOFF_DELAY);
    LED_Off_Blue();
    Delay(LED_ONOFF_DELAY);
    LED_On_Blue();
    Delay(LED_ONOFF_DELAY);
    LED_Off_Blue();
    Delay(LED_ONOFF_DELAY);
    LED_On_Blue();
}

static void led_rx_ok(void)
{
    LED_Off_All();

    LED_On_Yellow();
    Delay(LED_ONOFF_DELAY);
    LED_Off_Yellow();
    Delay(LED_ONOFF_DELAY);
    LED_On_Yellow();
    Delay(LED_ONOFF_DELAY);
    LED_Off_Yellow();
    Delay(LED_ONOFF_DELAY);
    LED_On_Yellow();
}

static void led_txrx_error(void)
{
    LED_Off_All();

    LED_On_Red();
    Delay(LED_ONOFF_DELAY);
    LED_Off_Red();
    Delay(LED_ONOFF_DELAY);
    LED_On_Red();
    Delay(LED_ONOFF_DELAY);
    LED_Off_Red();
    Delay(LED_ONOFF_DELAY);
    LED_On_Red();
}

/*********************************************************************
 * @fn      appPrintRfRegs_SREG
 * @brief   Print radio registers
 * @param   none
 * @return  none
 */
static void appPrintRfRegs_SREG(void)
{
    regKvp_t *p;
    uint16 iRegVal;

    p = regLookup_SREG;

    while ( p->iRegNum != 0xFF) {
        // Read radio registers
        iRegVal = CC2520_MEMRD8(p->iRegNum);

        // Print name and contents
        printf("0x%02X %-20s : 0x%02X\r\n",
               p->iRegNum, (char*)p->szRegName, iRegVal);

        p++;
    }
}

static void appPrintRfRegs_FREG(void)
{
    regKvp_t *p;
    uint16 iRegVal;

    p = regLookup_FREG;

    while ( p->iRegNum != 0xFF) {
        // Read radio registers
        iRegVal = CC2520_REGRD8(p->iRegNum);

        // Print name and contents
        printf("0x%02X %-20s : 0x%02X\r\n",
               p->iRegNum, (char*)p->szRegName, iRegVal);

        p++;
    }
}

/***********************************************************************************
* @fn          appLight
*
* @brief       Application code for light application. Puts MCU in endless
*              loop waiting for user input from joystick.
*
* @param       basicRfConfig - file scope variable. Basic RF configuration data
*              pRxData - file scope variable. Pointer to buffer for RX data
*
* @return      none
*/
static void appLight(void)
{
    DEBUG_MSG_FUNC_START;
    
    // Initialize BasicRF
    basicRfConfig.myAddr = RX_ADDR;
    if(basicRfInit(&basicRfConfig) == HAL_RF_FAILED) {
        DEBUG_MSG_ASSERT;
    }
    basicRfReceiveOn();

    DEBUG_MSG_FUNC_NOTIFY;

    // Main loop
    while (TRUE) {
        while(!basicRfPacketIsReady());

        DEBUG_MSG_FUNC_NOTIFY;

        if(basicRfReceive(pRxData, APP_PAYLOAD_LENGTH, NULL)>0) {
            printf("pRxData[0]: %c\r\n", pRxData[0]);
#ifdef DEBUG_MSG_LEVEL0
            printf("basicRfGetRssi() val: %d\r\n", basicRfGetRssi());
#endif
            led_rx_ok();
        }
        else
        {
            DEBUG_MSG_FUNC_ERROR;
            led_txrx_error();
        }
    }
}


/***********************************************************************************
* @fn          appSwitch
*
* @brief       Application code for switch application. Puts MCU in
*              endless loop to wait for commands from from switch
*
* @param       basicRfConfig - file scope variable. Basic RF configuration data
*              pTxData - file scope variable. Pointer to buffer for TX
*              payload
*              appState - file scope variable. Holds application state
*
* @return      none
*/
static void appSwitch(void)
{
    uint8 status;

    DEBUG_MSG_FUNC_START;

    // Initialize BasicRF
    basicRfConfig.myAddr = TX_ADDR;
    if(basicRfInit(&basicRfConfig) == HAL_RF_FAILED) {
        DEBUG_MSG_ASSERT;
    }

    // Keep Receiver off when not needed to save power
    basicRfReceiveOff();

    // Set TX output power
    CC2520_REGWR8(CC2520_TXPOWER, CC2520_TXPOWER_4_DBM);

    pTxData[0] = 'a' - 1;

    // Main loop
    while (TRUE) {
        Delay(500);
        if(pTxData[0] == 'z')
        {
            pTxData[0] = 'a';
        }
        else
        {
            pTxData[0] += 1;
        }
        printf("send data is %c\r\n", pTxData[0]);

        status = basicRfSendPacket(RX_ADDR, pTxData, APP_PAYLOAD_LENGTH);
        if(HAL_RF_SUCCESS != status)
        {
            DEBUG_MSG_FUNC_ERROR;
            led_txrx_error();
        }
        else
        {
            led_tx_ok();
        }

#if 0 // POOH
        // Put MCU to sleep. It will wake up on joystick interrupt
        halIntOff();
        halMcuSetLowPowerMode(HAL_MCU_LPM_3); // Will turn on global
        // interrupt enable
        halIntOn();
#endif
    }
}

/***********************************************************************************
* @fn          perTest_appReceiver
*
* @brief       Application code for the receiver mode. Puts MCU in endless loop
*
* @param       basicRfConfig - file scope variable. Basic RF configuration data
*              rxPacket - file scope variable of type perTestPacket_t
*
* @return      none
*/
static void perTest_appReceiver(void)
{
    uint32 iteration = 0;
    uint32 segNumber=0;
    int16 perRssiBuf[RSSI_AVG_WINDOW_SIZE] = {0};    // Ring buffer for RSSI
    uint8 perRssiBufCounter = 0;                     // Counter to keep track of the
    // oldest newest byte in RSSI
    // ring buffer
    perRxStats_t rxStats = {0,0,0,0};
    int16 rssi;

#ifdef INCLUDE_PA
    uint8 gain;

    // Select gain (for modules with CC2590/91 only)
    gain = appSelectGain();
    halRfSetGain(gain);
#endif

    // Initialize BasicRF
    basicRfConfig.myAddr = RX_ADDR;
    if(basicRfInit(&basicRfConfig) == HAL_RF_FAILED)
    {
        DEBUG_MSG_ASSERT;
    }
    basicRfReceiveOn();

    // Main loop
    while (TRUE)
    {
        iteration ++;

        while(!basicRfPacketIsReady());

        if(basicRfReceive((uint8*)&rxPacket, MAX_PAYLOAD_LENGTH, &rssi)>0) {
			// Change byte order from network to host order
			UINT32_NTOH(rxPacket.seqNumber);
            segNumber = rxPacket.seqNumber;

            // Subtract old RSSI value from sum
            rxStats.rssiSum -= perRssiBuf[perRssiBufCounter];
            // Store new RSSI value in ring buffer, will add it to sum later
            perRssiBuf[perRssiBufCounter] =  rssi;

            // Add the new RSSI value to sum
            rxStats.rssiSum += perRssiBuf[perRssiBufCounter];
            if (++perRssiBufCounter == RSSI_AVG_WINDOW_SIZE) {
                perRssiBufCounter = 0;      // Wrap ring buffer counter
            }

            // Check if received packet is the expected packet
            if (rxStats.expectedSeqNum == segNumber) {
                rxStats.expectedSeqNum++;
            }
            // If there is a jump in the sequence numbering
            // this means some packets in between has been lost.
            else if (rxStats.expectedSeqNum < segNumber){
                rxStats.lostPkts += segNumber - rxStats.expectedSeqNum;
                rxStats.expectedSeqNum = segNumber + 1;
            }
            // If the sequence number is lower than the previous one,
            // we will assume a new data burst has started
            // and we will reset our statistics variables.
            else {
                // Update our expectations assuming this is a new burst
                rxStats.expectedSeqNum = segNumber + 1;
                rxStats.rcvdPkts = 0;
                rxStats.lostPkts = 0;
            }
            rxStats.rcvdPkts++;
        }

        if(0 == (iteration & ITERATION_COUNT))
        {
            led_rx_ok();
            printf("rcvP: %d, lostP: %d, PER:%5.2f %%, RSSI:%5.2f dBm\r\n",
                   rxStats.rcvdPkts, rxStats.lostPkts,
                   (float)(rxStats.lostPkts)
                   /(float)(rxStats.lostPkts+rxStats.rcvdPkts)*100,
                   (float)(rxStats.rssiSum/32));
        }
    }
}


/***********************************************************************************
* @fn          perTest_appTransmitter
*
* @brief       Application code for the transmitter mode. Puts MCU in endless
*              loop
*
* @param       basicRfConfig - file scope variable. Basic RF configuration data
*              txPacket - file scope variable of type perTestPacket_t
*              appState - file scope variable. Holds application state
*              appStarted - file scope variable. Controls start and stop of
*                           transmission
*
* @return      none
*/
static void perTest_appTransmitter(void)
{
    uint32 burstSize=0;
    uint32 pktsSent=0;
	uint8  n;
    uint32 iteration = 0;
    u8     ch;

    // Initialize BasicRF
    basicRfConfig.myAddr = TX_ADDR;
    if(basicRfInit(&basicRfConfig) == HAL_RF_FAILED) {
        DEBUG_MSG_ASSERT;
    }
    
    // Set TX output power
    CC2520_REGWR8(CC2520_TXPOWER, CC2520_TXPOWER_4_DBM);

    printf("PACKET_SIZE sizeof(perTestPacket_t) is %d\r\n", PACKET_SIZE);

    // Set burst size
    printf("Select Burst Size\r\n");
    printf("1> BURST_SIZE 1000\r\n");
    printf("2> BURST_SIZE 10000\r\n");
    printf("3> BURST_SIZE 100000\r\n");
    printf("4> BURST_SIZE 1000000\r\n");

    ch = USART_GetCharacter(USART1);
    printf(" is selected\r\n");

    switch((char)ch)
    {
    case '1':
        burstSize = BURST_SIZE_1;
        break;

    case '2':
        burstSize = BURST_SIZE_2;
        break;

    case '3':
        burstSize = BURST_SIZE_3;
        break;

    case '4':
        burstSize = BURST_SIZE_4;
        break;

    default:
        burstSize = BURST_SIZE_1;
        break;
    }

    printf("Selected Burst Size: %d\r\n", burstSize);
    
    // Basic RF puts on receiver before transmission of packet, and turns off
    // after packet is sent
    basicRfReceiveOff();

    // Initalise packet payload
    txPacket.seqNumber = 0;
    for(n = 0; n < sizeof(txPacket.padding); n++) {
        txPacket.padding[n] = n;
    }

    // Main loop
    while (TRUE)
    {
        printf("Press Any Key to start sending data ...\r\n");
        USART_GetCharacter(USART1);

        while (TRUE)
        {
            iteration ++;

            if (pktsSent >= burstSize)
            {
                break;
            }
            // Delay(3);

            // Make sure sequence number has network byte order
            UINT32_HTON(txPacket.seqNumber);

            basicRfSendPacket(RX_ADDR, (uint8*)&txPacket, PACKET_SIZE);

            // Change byte order back to host order before increment
            UINT32_NTOH(txPacket.seqNumber);
            txPacket.seqNumber++;

            if(0 == (iteration & ITERATION_COUNT))
            {
                led_tx_ok();
                printf("sntP: %d\r\n", txPacket.seqNumber);
            }
        
            pktsSent++;
        }

        // Reset statistics and sequence number
        iteration = 0;
        pktsSent = 0;
        txPacket.seqNumber = 0;
    }
}

void ZigBee_Test(void)
{
    u8 ch;
    int channelNum;

    DEBUG_MSG_FUNC_START;

    printf("\r\n- Select Channel -\r\n");
    printf("<a> channel 11, <b> channel 12, <c> channel 13, <d> channel 14\r\n");
    printf("<e> channel 15, <f> channel 16, <g> channel 17, <h> channel 18\r\n");
    printf("<i> channel 19, <j> channel 20, <k> channel 21, <l> channel 22\r\n");
    printf("<m> channel 23, <n> channel 24, <o> channel 25, <p> channel 26\r\n");

    ch = USART_GetCharacter(USART1);
    printf(" is selected\r\n");

    channelNum = MIN_CHANNEL + ((int) (ch - 'a'));
    printf("selected channel is %d\r\n", channelNum);
    
    if((channelNum >= MIN_CHANNEL) && (channelNum <= MAX_CHANNEL))
    {
        basicRfConfig.channel = channelNum;
    }
    else
    {
        printf("default channel is %d\r\n", RF_CHANNEL);
        basicRfConfig.channel = RF_CHANNEL;
    }

    // F = 2405 + 5 * (k - 11) [MHz] k ��[11,26]
    printf("frequency range is from %d MHz to %d MHz\r\n", 2405, 2480);
    printf("selected frequency is %d MHz\r\n",
           2405 + 5 * (basicRfConfig.channel - MIN_CHANNEL));

    // Config basicRF
    basicRfConfig.panId = PAN_ID;

#if 1
    basicRfConfig.ackRequest = TRUE;
#else
    basicRfConfig.ackRequest = FALSE;
#endif

#ifdef SECURITY_CCM
    basicRfConfig.securityKey = key;
#endif

    SPIx_Init();

    halRfInit();

    printf("chipid : 0x%x\r\n", halRfGetChipId());
    printf("version : 0x%x\r\n", halRfGetChipVer());

    while(1)
    {
        printf("\r\n\r\n---------------------\r\n");
        printf("Press menu key\r\n");
        printf("---------------------\r\n");
        printf("1> Print Registers: FREG\r\n");
        printf("2> Print Registers: SREG\r\n");
        printf("---------------------\r\n");
        printf("3> appLight\r\n");
        printf("4> appSwitch\r\n");
        printf("---------------------\r\n");
        printf("5> PER Test appReceiver\r\n");
        printf("6> PER Test appTransmitter\r\n");
        printf("---------------------\r\n");
        printf("x> quit\r\n\r\n");

#if 1
        ch = USART_GetCharacter(USART1);
#else
        ch = '4';
#endif

        printf(" is selected\r\n");

        switch((char)ch)
        {
        case '1':
            appPrintRfRegs_FREG();
            break;

        case '2':
            appPrintRfRegs_SREG();
            break;

        case '3':
            appLight();
            break;

        case '4':
            appSwitch();
            break;

        case '5':
            perTest_appReceiver();
            break;

        case '6':
            perTest_appTransmitter();
            break;
        }

        if('x' == (char)ch)
        {
            break;
        }
    }
}

#endif /* BOARD_DEF_MANGO_Z1 */

