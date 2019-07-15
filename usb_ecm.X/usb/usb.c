#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/attribs.h>
#include <sys/kmem.h>
#include "usb.h"

#include "/opt/microchip/xc32/v2.20/pic32mx/include/proc/p32mz2048efh144.h"

#include "../global.h"

/* USB related values */


/*********** Variable Declarations ***********/
volatile int ep0rbc; // Endpoint 0 - Received Bytes Count (USB).
volatile int ep3rbc; // Endpoint 3 - Received Bytes Count (USB).
volatile uint32_t i;
uint8_t *ep0usbData;
uint8_t *ep1usbData;
uint8_t *ep2usbData;
uint8_t endpoint;
uint8_t *ep0BlockData;
uint8_t *ep1BlockData;
uint8_t *ep2BlockData;
uint16_t NumBytes, packetSize;
uint16_t packetSize_ep0, packetSize_ep1, packetSize_ep2;
uint16_t ep0RemainingBytes;
uint16_t ep1RemainingBytes;
uint16_t ep2RemainingBytes;
volatile int usbReset;
uint8_t ep0data[(EP0BUFFSIZE * 8)]; // USB end point 0 data 
uint8_t ep1data[(EP1TXBUFFSIZE * 8)]; // USB end point 1 data 
uint8_t ep2data[(EP2TXBUFFSIZE * 8)]; // USB end point 2 data
uint8_t ep3data[(EP3RXBUFFSIZE * 8)]; // USB end point 2 data 
volatile uint8_t *tempData;
volatile uint16_t bmbRequest; // bmRequestType, bRequest
volatile uint16_t wValue;
volatile uint16_t wIndex;
volatile uint16_t wLength;
volatile uint8_t usbAddress = 0;
volatile uint8_t setDevAddr;
volatile uint32_t ep0ArrPos;
volatile uint32_t ep1ArrPos;
volatile uint32_t ep2ArrPos;
char usbConfiguration;
char usbInterface;

unsigned char UART_state;
unsigned char testState[] = {0xA1, 0x20, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00};

enum {
    FALSE, TRUE
};
/******* End of Variable Declarations ********/

/************* Declare functions *************/
void initBoard(void);
void initUSB(void);
void setAddress(uint8_t address);
void controlTrans(void);
void inPacket(void);
void serialStateNotification(void);
int txBlock_ep0(uint16_t NumBytes_ep0);
int txBlock_ep1(uint16_t NumBytes_ep1);
void txByte_ep2(uint16_t ep2ArrPos);
void rxUSB_ep0(volatile int ep0rbc);
void rxUSB_ep3(void);

void __ISR_AT_VECTOR(_USB_VECTOR, IPL4SRS) _USB_Handler(void) {

    in_interrupt();
    
    if (USBCSR2bits.RESETIF) { // In Device mode, indicates reset signaling is detected on the bus.
        usbReset = TRUE; // Indicate the bus was reset and should be IDLE now.

        USBE1CSR0bits.MODE = 0; // 1 = Endpoint is RX.
        USBE2CSR0bits.MODE = 0; // 1 = Endpoint is RX
        USBE3CSR0bits.MODE = 1; // 0 = Endpoint is TX.

        // Note that USB reset will reset a lot of the registers and you should 
        // do most initialization in the USB reset interrupt handler.
        USBE0CSR0bits.TXMAXP = EP0BUFFSIZE;
        USBE0CSR2bits.SPEED = 1; // Endpoint 0 Operating Speed Control bits
        USBE1CSR2bits.SPEED = 1; // Endpoint 1: TX Endpoint Operating Speed Control bits - High speed
        USBE2CSR2bits.SPEED = 1; // Endpoint 2: TX Endpoint Operating Speed Control bits - High speed
        USBE3CSR3bits.SPEED = 1; // Endpoint 3: RX Endpoint Operating Speed Control bits - High speed

        /* These are multiples of 8, e.g.- "1" is 8 bytes, "8" is 64 bytes */
        USBE1CSR0bits.TXMAXP = EP1TXBUFFSIZE; // Endpoint 1 - Maximum TX Payload Per Transaction Control bits
        USBE2CSR0bits.TXMAXP = EP2TXBUFFSIZE; // Endpoint 2 - Maximum TX Payload per transaction Control bits
        USBE3CSR1bits.RXMAXP = EP3RXBUFFSIZE; // Endpoint 3 - Maximum RX Payload Per Transaction Control bits

        USBE1CSR2bits.PROTOCOL = 3; // Endpoint 1 - TX Endpoint Protocol Control bits 
        USBE2CSR2bits.PROTOCOL = 2; // Endpoint 2 - TX Endpoint Protocol Control bits
        USBE3CSR3bits.PROTOCOL = 2; // Endpoint 3 - RX Endpoint Protocol Control bits
        //PROTOCOL<1:0>: RX/TX Endpoint Protocol Control bits 
        //11 = Interrupt
        //10 = Bulk
        //01 = Isochronous
        //00 = Control

        USBCSR2bits.EP1RXIE = 1; // Endpoint 1 RX interrupt enable
        USBCSR2bits.EP2RXIE = 1; // Endpoint 2 RX interrupt enable
        USBCSR1bits.EP3TXIE = 1; // Endpoint 3 TX interrupt enable

        USBCSR2bits.RESETIF = 0;
    }

    /* Endpoint 0 Interrupt Handler */
    if (USBCSR0bits.EP0IF == 1) {
        //led_2 = 1;
        usbReset = FALSE;

        on_event("ep 0 interrupt");
        
        if (setDevAddr == TRUE) { // Set Address, upon first IN transaction
            setAddress(usbAddress);
            setDevAddr = FALSE;
            on_event("address changed");
        }

        if (USBE0CSR0bits.RXRDY) {
            ep0rbc = USBE0CSR2bits.RXCNT; // Endpoint 0 - Received Bytes Count
            tempData = (uint8_t *) & USBFIFO0;

            on_event("0 rx ready");

            for (i = 0; i < ep0rbc; i++) {
                ep0data[i] = *(tempData + (i & 3));
            }
            USBE0CSR0bits.RXRDYC = 1;
            push_event(EP0_RX_READY, ep0data, ep0rbc);
            // The packet length of control transfers in low speed devices must be 
            // 8 bytes, high speed devices allow a packet size of 8, 16, 32 or 64 bytes 
            // and full speed devices must have a packet size of 64 bytes.
            bmbRequest = (ep0data[0] << 8) | ep0data[1]; // bmRequestType, bRequest
            wValue = (ep0data[3] << 8) | ep0data[2];
            wIndex = (ep0data[5] << 8) | ep0data[4];
            wLength = (ep0data[7] << 8) | ep0data[6];

            if (ep0data[0] & 0x80) { /* A "1" indicates OUT from the host or "0" indicates IN to the host */
                controlTrans(); // Process Control Transfers and OUT Packets.
            } else {
                inPacket(); // Process IN packets.
            }
        }


        if (USBE0CSR0bits.SETEND) {
            USBE0CSR0bits.SETENDC = 1;
        }

        USBCSR0bits.EP0IF = 0; // Clear the USB EndPoint 0 Interrupt Flag.
    }

    /* Endpoint 1 TX Interrupt Handler */
    if (USBCSR1bits.EP1RXIF == 1) { // INTERRUPT Endpoint 1 Transmit A Packet.
        //serialStateNotification();
        //ep1BlockData = serialState;
        //ep1BlockData = testState;
        on_event("EP 1 RX");
        //rxBlock_ep1(sizeof (ep1BlockData));
        USBCSR1bits.EP1RXIF = 0;
    }

    /* Endpoint 2 TX Interrupt Handler */
    if (USBCSR1bits.EP2RXIF == 1) { // BULK Endpoint 2 Transmit A Packet.
        //ep2BlockData = ep3data;
        on_event("EP 2 RX");
        //rxByte_ep2(0);
        USBCSR1bits.EP2RXIF = 0;
    }

    /* Endpoint 3 RX Interrupt Handler */
    if (USBCSR0bits.EP3TXIF == 1) { // BULK Endpoint 3 Received A Packet.
        //rxUSB_ep3();

        ep2BlockData = ep3data;
        //txByte_ep2(0);
        //push_event(RECV_DATA, ep3data, 1);
        on_event("EP 3 TX");
        //led_1 = ~led_1;
        USBCSR0bits.EP3TXIF = 0;
    }



    IFS4bits.USBIF = 0; // Reset the USB Interrupt flag
}

void setAddress(volatile uint8_t addr) {
    // The Set_Address request changes the address after the status stage. 
    // You must update the FUNC field only after you get the status interrupt.
    USBCSR0bits.FUNC = addr & 0x7F;
}

void controlTrans() {
    //led_3 = 1;

    switch (bmbRequest) {
        case 0x8000: // Get Status (Device)
            ep0BlockData = replyDoubleZero;
            txBlock_ep0(2);
            break;

        case 0x8100: // Get Status (Interface)
            ep0BlockData = replyDoubleZero;
            txBlock_ep0(2);
            break;

        case 0x8200: // Get Status (Endpoint)
            ep0BlockData = replyDoubleZero;
            txBlock_ep0(2);
            break;

        case 0x8002: // Get Configuration
            ep0BlockData = &usbConfiguration;
            txBlock_ep0(wLength);
            break;

        case 0x8006:
        { // Get Descriptor(s)

            switch (ep0data[3]) {
                case 0x00:
                { // Undefined
                    USBE0CSR0bits.STALL = 1;
                }
                    break;

                case 0x01:
                { // Device Descriptor
                    //led_4r = 0;
                    ep0BlockData = dev_desc;
                    txBlock_ep0(sizeof (dev_desc));
                }
                    break;

                case 0x02:
                { // Configuration Descriptor
                    /* Per the USB Specifications:
                     * A request for a configuration descriptor returns the 
                     * configuration descriptor, all interface descriptors, and 
                     * endpoint descriptors for all of the interfaces in a single 
                     * request. The first interface descriptor follows the 
                     * configuration descriptor. 
                     * 
                     * The endpoint descriptors for the first interface follow the 
                     * first interface descriptor. 
                     * 
                     * If there are additional interfaces, their interface 
                     * descriptor and endpoint descriptors follow the first 
                     * interface's endpoint descriptors. 
                     * 
                     * Class-specific and/or vendor-specific descriptors follow the 
                     * standard descriptors they extend or modify.
                     * 
                     * All devices must provide a device descriptor and at least one 
                     * configuration descriptor. If a device does not support a 
                     * requested descriptor, it responds with a Request Error.
                     */
                    ep0BlockData = CDC_ConfigDescriptorArray;
                    txBlock_ep0(wLength);
                    //led_4b = 0; // Enumeration Complete (or it should be!)
                }
                    break;

                case 0x03:
                { // String Descriptors
                    switch (ep0data[2]) {
                        case 0x00:
                        { // Language ID
                            ep0BlockData = string0;
                            txBlock_ep0(sizeof (string0));
                        }
                            break;

                        case 0x01:
                        { // Manufacturer
                            ep0BlockData = string1;
                            txBlock_ep0(sizeof (string1));
                        }
                            break;

                        case 0x02:
                        { // Product
                            ep0BlockData = string2;
                            txBlock_ep0(sizeof (string2));
                        }
                            break;

                        case 0x03:
                        { // Serial
                            ep0BlockData = string3;
                            txBlock_ep0(sizeof (string3));
                        }
                            break;

                        case 0x04:
                        { // Configuration
                            ep0BlockData = string4;
                            txBlock_ep0(sizeof (string4));
                        }
                            break;

                        case 0x05:
                        { // Interface
                            ep0BlockData = string5;
                            txBlock_ep0(sizeof (string5));
                        }
                            break;

                    } // End of case ep0data[2] switch
                    break;

                } // End of case 0x03 switch - String Descriptors
                    break;

                case 0x04:
                { // Get Interface
                    switch (ep0data[2]) {
                            //case 0x00:
                            //break;

                        case 0x00:
                            USBE0CSR0bits.STALL = 1;
                            break;

                        case 0x01:
                            USBE0CSR0bits.STALL = 1;
                            break;
                    }

                } // End of case 0x04 switch
                    break;

                case 0x05:
                { // Get Endpoint
                    switch (ep0data[2]) {
                            //case 0x00:

                            //break;

                        case 0x00:
                            USBE0CSR0bits.STALL = 1;
                            break;

                        case 0x01:
                            USBE0CSR0bits.STALL = 1;
                            break;
                    }
                } // End of case 0x05 switch

                    break;

                case 0x06: // Get Device Qualifier

                    break;

                case 0x07: // Other Speed COnfiguration

                    break;

                default:
                    USBE0CSR0bits.STALL = 1;
                    break;
            } // End of switch ep0data[3].

        } // End of case 0x8006 switch - Get Descriptor(s).
            break;

        case 0x8008: // Get Configuration
            ep0BlockData = &usbConfiguration;
            txBlock_ep0(sizeof (usbConfiguration));
            //USBE0CSR0bits.STALL = 1;
            break;

        case 0x810A: // Get Interface
            ep0BlockData = &usbInterface;
            txBlock_ep0(sizeof (usbInterface));
            //USBE0CSR0bits.STALL = 1;
            break;

        case 0x8025: // When connected ep0data[6] = 0x08
            USBE0CSR0bits.STALL = 1;
            break;

        default:
            USBE0CSR0bits.STALL = 1;
            break;
    } // End of switch bmbRequest.

    return;
}

void inPacket() {
    switch (bmbRequest) {

        case 0x0000: // Get Status
            ep0BlockData = replyDoubleZero;
            txBlock_ep0(2);
            break;

        case 0x0001: // Clear Feature

            break;

        case 0x0003: // Set Feature

            break;

        case 0x0005: // Save Address
            USBE0CSR0bits.RXRDYC = 1;
            USBE0CSR0bits.DATAEND = 1;
            usbAddress = ep0data[2];
            setDevAddr = TRUE;
            push_event(SET_ADDR, &usbAddress, 1);
            break;

        case 0x0006:
        { // Get Descriptor(s)

            switch (ep0data[3]) {
                case 0x00:
                { // Undefined
                    USBE0CSR0bits.STALL = 1;
                }
                    break;

                case 0x01:
                { // Device Descriptor
                    //led_4r = 0;
                    ep0BlockData = dev_desc;
                    txBlock_ep0(sizeof (dev_desc));
                }
                    break;

                case 0x02:
                { // Configuration Descriptor
                    //led_4b = 0;
                    USBE0CSR0bits.STALL = 1;
                }
                    break;

                case 0x03:
                { // String Descriptors // Get Descriptor
                    switch (ep0data[2]) {
                        case 0x00:
                        { // Language ID
                            ep0BlockData = string0;
                            txBlock_ep0(sizeof (string0));
                        }
                            break;

                        case 0x01:
                        { // Manufacturer
                            ep0BlockData = string1;
                            txBlock_ep0(sizeof (string1));
                        }
                            break;

                        case 0x02:
                        { // Product
                            ep0BlockData = string2;
                            txBlock_ep0(sizeof (string2));
                        }
                            break;

                        case 0x03:
                        { // Serial
                            ep0BlockData = string3;
                            txBlock_ep0(sizeof (string3));
                        }
                            break;

                        case 0x04:
                        { // Configuration
                            ep0BlockData = string4;
                            txBlock_ep0(sizeof (string4));
                        }
                            break;

                        case 0x05:
                        { // Interface
                            ep0BlockData = string5;
                            txBlock_ep0(sizeof (string5));
                        }
                            break;

                    } // End of case ep0data[2] switch
                    break;

                } // End of case 0x03 switch - String Descriptors 
                default:
                    USBE0CSR0bits.STALL = 1;
                    break;
            } // End of switch ep0data[3].

        } // End of case 0x0006 switch - Get Descriptor(s).
            break;

        case 0x0007: // Set Descriptor
            USBE0CSR0bits.STALL = 1;
            break;

        case 0x0008: // Get Configuration
            ep0BlockData = &usbConfiguration;
            txBlock_ep0(sizeof (usbConfiguration));
            //USBE0CSR0bits.STALL = 1; 
            break;

        case 0x0009: // Set Configuration
            usbConfiguration = ep0data[2]; // wValue
            break;

        case 0x000A: // Get Interface
            ep0BlockData = &usbInterface;
            txBlock_ep0(sizeof (usbInterface));
            //USBE0CSR0bits.STALL = 1;
            break;

        case 0x000B: // Set Interface
            usbInterface = ep0data[4]; // wIndex
            break;

        case 0x000C: // Synchronize Frame
            USBE0CSR0bits.STALL = 1;
            break;

        case 0x2100: // SEND_ENCAPSULATED_ COMMAND (Required for CDC ACM)
            //
            break;

        case 0x2101: // GET_ENCAPSULATED_ RESPONSE (Required for CDC ACM)
            //
            break;

        case 0x2120: // SET_LINE_CODING 
            // Configures baud rate, stop-bits, parity, and number of character bits.
            // May need to verify this later
            rxUSB_ep0(wLength);
            serialState = ep0data;
            dwDTERate = (serialState[3] << 24) | (serialState[2] << 16) | (serialState[1] << 8) | serialState[0];
            bCharFormat = serialState[4];
            bParityType = serialState[5];
            bDataBits = serialState[6];
            break;

        case 0x2121: // GET_LINE_CODING
            // Requests current DTE rate, stop-bits, parity, and number-of-character bits.
            // May need to verify this later
            ep0BlockData = serialState;
            txBlock_ep0(sizeof (serialState));
            break;

        case 0x2122: // SET_CONTROL_LINE_STATE
            // RS232 signal used to tell the DCE device the DTE device is now present.
            controlState = wValue;
            break;

        default:
            USBE0CSR0bits.STALL = 1;
            break;
    }
    return;
}

void serialStateNotification() {
    // Need to verify/correct this later
    if (sizeof (ep2BlockData) > 0) {
        UART_state = 0x03; // If TX data is available set DCD and DSR
    } else {
        UART_state = 0x00;
    }
    serialState[0] = 0xA1; //   bmRequestType
    serialState[1] = CDC_SERIAL_STATE_MASK; //   bNotification
    serialState[2] = 0x00; //   wValue (LSB) 
    serialState[3] = 0x00; //   wValue (MSB) 
    serialState[4] = 0x00; //   wIndex (LSB) 
    serialState[5] = 0x00; //   wIndex (MSB)
    serialState[6] = 0x02; //   wLength (LSB)
    serialState[7] = 0x00; //   wLength (MSB)                       
    serialState[8] = UART_state; //   UART state bitmap
    serialState[9] = 0x00;

    UART_state &= CDC_SERIAL_STATE_MASK;

    return;
}

int txBlock_ep0(uint16_t NumBytes_ep0) {

    volatile uint8_t *ep0fifo = NULL;
    ep0fifo = (uint8_t *) & USBFIFO0;
    uint16_t ep0ArraySize = sizeof (ep0BlockData);
    char setEnd = FALSE;

    packetSize_ep0 = NumBytes_ep0;
    if (packetSize_ep0 > EP0_MAX_PACKET_SIZE) {
        packetSize_ep0 = EP0_MAX_PACKET_SIZE;
    }
    ep0RemainingBytes = NumBytes_ep0;

    ep0ArrPos = 0; // Reset the array position.
    do {

        int timeout = 0;
        do {
            timeout++;
        } while (USBE0CSR0bits.TXRDY && timeout < 400);

        if (timeout >= 400) {
            return -1;
        }

        if (ep0RemainingBytes < packetSize_ep0) { // Check if this is  last packet
            packetSize_ep0 = ep0RemainingBytes;
            setEnd = TRUE;
        }

        for (i = 0; i < packetSize_ep0; i++) {
            *ep0fifo = ep0BlockData[ep0ArrPos++];
            ep0RemainingBytes--;
        }

        if (setEnd == TRUE) {
            USBIE0CSR0bits.DATAEND = 1; // End of Data Control bit (Device mode) 
            // The software sets this bit when:
            // * Setting TXPKTRDY for the last data packet
            // * Clearing RXPKTRDY after unloading the last data packet 
            // * Setting TXPKTRDY for a zero length data packet
            // Hardware clears this bit.
        }
        USBE0CSR0bits.TXRDY = 1;

    } while (ep0RemainingBytes != 0);

    return 0;
}

int txBlock_ep1(uint16_t NumBytes_ep1) {

    volatile uint8_t *ep1fifo = NULL;
    ep1fifo = (uint8_t *) & USBFIFO1;
    uint16_t ep1ArraySize = sizeof (ep1BlockData);
    char setEnd = FALSE;

    packetSize_ep1 = NumBytes_ep1;
    if (packetSize_ep1 > EP1_MAX_PACKET_SIZE) {
        packetSize_ep1 = EP1_MAX_PACKET_SIZE;
    }
    ep1RemainingBytes = NumBytes_ep1;

    ep1ArrPos = 0; // Reset the array position.
    do {

        int timeout = 0;
        do {
            timeout++;
        } while (USBE1CSR0bits.TXPKTRDY && timeout < 400);

        if (timeout >= 400) {
            return -1;
        }

        if (ep1RemainingBytes < packetSize_ep1) { // Check if this is  last packet
            packetSize_ep1 = ep1RemainingBytes;
            setEnd = TRUE;
        }

        for (i = 0; i < packetSize_ep1; i++) {
            *ep1fifo = ep1BlockData[ep1ArrPos++];
            ep1RemainingBytes--;
        }

        if (setEnd == TRUE && packetSize_ep1 == EP1_MAX_PACKET_SIZE) {
            // Set TXPKTRDY for a zero length data packet
            // when the last TX data equals the max. packet size
            // Hardware clears this bit.

            *ep1fifo = (uint32_t) NULL;
        }

        USBE1CSR0bits.TXPKTRDY = 1;



    } while (ep1RemainingBytes != 0);

    return 0;
}

void txByte_ep2(uint16_t ep2ArrPos) {
    volatile uint8_t *ep2fifo = NULL;
    ep2fifo = (uint8_t *) & USBFIFO2;

    // Uncomment the folllowing line to echo the same character back.
    //*ep2fifo = (uint8_t)ep2BlockData[ep2ArrPos]; // Send ONE byte from the array.
    // Comment out  the folllowing line if you uncommented the previous line above.
    *ep2fifo = (uint8_t) ep2BlockData[ep2ArrPos] + 1; // Increment (+1) then send the character.

    USBE2CSR0bits.TXPKTRDY = 1;
}

void rxUSB_ep0(volatile int ep0rbc) {
    memset(ep0data, 0, (EP3RXBUFFSIZE * 8));
    tempData = (uint8_t *) & USBFIFO0; // The FIFO is 32 bits wide, we only want 8 bits at a time.
    for (i = 0; i < ep0rbc; i++) {
        ep0data[i] = *(tempData + (i & 3)); // Store the received bytes in array ep0data[].
    }
    USBE0CSR0bits.RXRDYC = 0; // 0 = Written by software to clear this bit 
    // when the packet has been unloaded from the RX FIFO.
}

void rxUSB_ep3() {
    memset(ep3data, 0, (EP3RXBUFFSIZE * 8));
    ep3rbc = USBE3CSR2bits.RXCNT; // Endpoint 3 - Received Bytes Count
    tempData = (uint8_t *) & USBFIFO3; // The FIFO is 32 bits wide, we only want 8 bits at a time.
    for (i = 0; i < ep3rbc; i++) {
        ep3data[i] = *(tempData + (i & 3)); // Store the received bytes in array ep3data[].
    }
    USBE3CSR1bits.RXPKTRDY = 0; // 0 = Written by software to clear this bit 
    // when the packet has been unloaded from the RX FIFO.
}