
#include "xc.h"

#include "usb.h"

/* USB related values */
#define MAX_EP0_TX_SIZE   8   // In multiples of 8, e.g.- 8 (x8) equals a size of 64
#define EP1RXBUFFSIZE   1   // Also in multiples of 8, e.g.- 8 (x8) equals a size of 64
#define EP2TXBUFFSIZE   1   // Also in multiples of 8, e.g.- 8 (x8) equals a size of 64
#define EP3RXBUFFSIZE   8   // Also in multiples of 8, e.g.- 8 (x8) equals a size of 64
#define EP4TXBUFFSIZE   8   // Also in multiples of 8, e.g.- 8 (x8) equals a size of 64
#define EP0_MAX_PACKET_SIZE  (MAX_EP0_TX_SIZE * 8)
#define EP1_MAX_PACKET_SIZE  (EP1RXBUFFSIZE * 8)
#define EP2_MAX_PACKET_SIZE  (EP2TXBUFFSIZE * 8)
#define EP3_MAX_PACKET_SIZE  (EP3RXBUFFSIZE * 8)
#define EP4_MAX_PACKET_SIZE  (EP4TXBUFFSIZE * 8)

/*********** Variables ***********/
volatile int end_point0_rx_count; // Endpoint 0 - Received Bytes Count (USB).
volatile int ep1rbc; // Endpoint 1 - Received Bytes Count (USB).
int ep0Iterations; // Number of iterations of bytes sent via Endpoint 0.
uint16_t ep0RemainingBytes;
uint16_t packetSize;
int epBufSize[8]; // Endpoint Buffers Size.
volatile uint32_t i;
uint8_t *ep0usbData;
uint8_t *ep2usbData;
uint8_t endpoint;
volatile int usbReset;
volatile uint8_t end_point0_rx_buf[MAX_EP0_TX_SIZE * 8]; // USB end point 0 data - Array should be the same length as the largest USB FIFO size
volatile uint8_t ep1data[EP1RXBUFFSIZE * 8]; // USB end point 1 data - Array should be the same length as the largest USB FIFO size
volatile uint8_t ep2data[EP2TXBUFFSIZE * 8]; // USB end point 2 data - Array should be the same length as the largest USB FIFO size
volatile uint8_t *tempData;
volatile uint16_t bmbRequest; // bmRequestType, bRequest
volatile uint16_t wValue;
volatile uint16_t wIndex;
volatile uint16_t wLength;
volatile uint8_t usbAddress;
volatile uint8_t setDevAddr;
volatile uint32_t ep0ArrPos;
volatile uint32_t ep2ArrPos;

enum {
    FALSE, TRUE
};

int txBulk_ep0(uint16_t ep0TXLen);

//void __ISR(_USB_VECTOR, ipl4AUTO) _IntHandlerUSBInstance0(void) {
//void __attribute__((vector(_USB_VECTOR), interrupt(ipl3soft), nomips16)) usb_int_handler() {
    //DRV_USBHS_Tasks_ISR(sysObj.drvUSBObject);
//    int x=0;
 //   USBCRCONbits.USBIF=0;
//}

//void __ISR(_USB_DMA_VECTOR, ipl4AUTO) _IntHandlerUSBInstance0_USBDMA(void) {
void __attribute__((vector(_USB_DMA_VECTOR), interrupt(ipl3soft), nomips16)) usb_dma_int_handler() {
    //DRV_USBHS_Tasks_ISR_USBDMA(sysObj.drvUSBObject);
    int x=0;
}

void setAddress(volatile uint8_t usbAddress) {
    // The Set_Address request changes the address after the status stage. 
    // You must update the FUNC field only after you get the status interrupt.
    USBCSR0bits.FUNC = usbAddress & 0x7F;
}

void controlTrans() {

    switch (bmbRequest) {
        case 0x8000: // Get Status (Device)

            break;

        case 0x8100: // Get Status (Interface)

            break;

        case 0x8200: // Get Status (Endpoint)

            break;

        case 0x8006:
        { // Get Descriptor(s)

            switch (end_point0_rx_buf[3]) {
                case 0x00:
                { // Undefined
                    USBE0CSR0bits.STALL = 1;
                }
                    break;

                case 0x01:
                { // Device Descriptor
                    ep0usbData = dev_desc;
                    txBulk_ep0(sizeof (dev_desc));
                }
                    break;

                case 0x02:
                { // Configuration Descriptors
                    ep0usbData = configDescriptors;
                    txBulk_ep0(sizeof (configDescriptors));
                }
                    break;

                case 0x03:
                { // String Descriptors
                    switch (end_point0_rx_buf[2]) {
                        case 0x00:
                        { // Language ID
                            ep0usbData = string0;
                            txBulk_ep0(sizeof (string0));
                        }
                            break;

                        case 0x01:
                        { // Manufacturer
                            ep0usbData = string1;
                            txBulk_ep0(sizeof (string1));
                        }
                            break;

                        case 0x02:
                        { // Product
                            ep0usbData = string2;
                            txBulk_ep0(sizeof (string2));
                        }
                            break;

                        case 0x03:
                        { // Serial
                            ep0usbData = string3;
                            txBulk_ep0(sizeof (string3));
                        }
                            break;

                        case 0x04:
                        { // Configuration
                            ep0usbData = string4;
                            txBulk_ep0(sizeof (string4));
                        }
                            break;

                        case 0x05:
                        { // Interface
                            ep0usbData = string5;
                            txBulk_ep0(sizeof (string5));
                        }
                            break;

                    } // End of case end_point0_rx_buf[2] switch
                    break;

                } // End of case 0x03 switch - String Descriptors
                    break;

                default:
                    USBE0CSR0bits.STALL = 1;
                    break;
            } // End of switch end_point0_rx_buf[3].

        } // End of case 0x8006 switch - Get Descriptor(s).
            break;

        case 0x8008: // Get Configuration

            break;

        case 0x810A: // Get Interface

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
            break;

        case 0x0001: // Clear Feature
            break;

        case 0x0003: // Set Feature
            break;

        case 0x0005: // Save Address
            USBE0CSR0bits.RXRDYC = 1;
            USBE0CSR0bits.DATAEND = 1;
            usbAddress = end_point0_rx_buf[2];
            setDevAddr = TRUE;
            break;

        case 0x0006: // Get Descriptor
            break;

        case 0x0007: // Set Descriptor
            break;

        case 0x0008: // Get Configuration
            break;

        case 0x0009: // Set Configuration
            break;

        case 0x000A: // Get Interface
            break;

        case 0x000B: // Set Interface
            break;

        case 0x000C: // Synchronize Frame
            break;

        default:
            USBE0CSR0bits.STALL = 1;
            break;
    }
    return;
}

void txUSB_ep0(uint16_t ep0TXLen) {
    volatile uint8_t *ep0fifo = NULL;
    ep0fifo = (uint8_t *) & USBFIFO0;

    for (ep0ArrPos = 0; ep0ArrPos < ep0TXLen; ep0ArrPos++) {
        *ep0fifo = ep0usbData[ep0ArrPos];
    }

    USBE0CSR0bits.TXRDY = 1;
}

int txBulk_ep0(uint16_t ep0TXLen) {

    volatile uint8_t *ep0fifo = NULL;
    ep0fifo = (uint8_t *) & USBFIFO0;
    uint16_t ep0ArraySize = sizeof (ep0usbData);
    char setEnd = FALSE;

    packetSize = ep0TXLen;
    if (packetSize > EP0_MAX_PACKET_SIZE) {
        packetSize = EP0_MAX_PACKET_SIZE;
    }

    if (ep0Iterations < 1) { // Send for first few descriptor bytes on the first iteration.
        ep0RemainingBytes = ep0TXLen;
    } else { // Send all of the descriptor bytes on successive iterations.
        ep0RemainingBytes = ep0ArraySize;
    }

    ep0ArrPos = 0; // Reset the array position.
    do {

        int timeout = 0;
        do {
            timeout++;
        } while (USBE0CSR0bits.TXRDY && timeout < 400);

        if (timeout >= 400) {
            return -1;
        }

        if (ep0RemainingBytes < packetSize) { // Check if this is  last packet
            packetSize = ep0RemainingBytes;
            setEnd = TRUE;
        }

        for (i = 0; i < packetSize; i++) {
            *ep0fifo = ep0usbData[ep0ArrPos++];
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

void rxUSB_ep1() {
    ep1rbc = USBE1CSR2bits.RXCNT; // Endpoint 1 - Received Bytes Count
    tempData = (uint8_t *) & USBFIFO1; // The FIFO is 32 bits wide, we only want 8 bits at a time.
    for (i = 0; i < ep1rbc; i++) {
        ep1data[i] = *(tempData + (i & 3)); // Store the received bytes in array ep1data[].
    }
    USBE1CSR1bits.RXPKTRDY = 0; // 0 = Written by software to clear this bit 
    // when the packet has been unloaded from the RX FIFO.
}

void txUSB_ep2(uint32_t ep2TXLen) {
    volatile uint8_t *ep2fifo = NULL;
    ep2fifo = (uint8_t *) & USBFIFO2;

    for (ep2ArrPos = 0; ep2ArrPos < ep2TXLen; ep2ArrPos++) {
        *ep2fifo = ep2usbData[ep2ArrPos];
    }
    USBE2CSR0bits.TXPKTRDY = 1;
}

void __attribute__((vector(_USB_VECTOR), interrupt(ipl7soft), nomips16)) usb_int_handler() {

    toggle_usb_rx_led();
    
    if (USBCSR2bits.RESETIF) { // In Device mode, indicates reset signaling is detected on the bus.
        usbReset = TRUE; // Indicate the bus was reset and should be IDLE now.

        // Note that USB reset will reset a lot of the registers and you should 
        // do most initialization in the USB reset interrupt handler.
        USBE0CSR0bits.TXMAXP = MAX_EP0_TX_SIZE;
        USBCSR1bits.EP0IE = 1;
        USBE0CSR2bits.SPEED = 1; // Endpoint 0 Operating Speed Control bits
/*
 SPEED<1:0>: RX Endpoint Operating Speed Control bits
11 = Low-Speed
10 = Full-Speed
01 = Hi-Speed
00 = Reserved*/
        USBCSR2bits.RESETIF = 0;
    }

    /* Endpoint 0 Interrupt Handler */
    if (USBCSR0bits.EP0IF) {
        usbReset = FALSE;

        if (setDevAddr == TRUE) { // Set Address, upon first IN transaction
            setAddress(usbAddress);
            setDevAddr = FALSE;
        }

        if (USBE0CSR0bits.RXRDY) {
            uint8_t buff[8];
            uint8_t data;
            //0-6 bits max 128 bytes??
            end_point0_rx_count = USBE0CSR2bits.RXCNT; // Endpoint 0 - Received Bytes Count
            tempData = (uint8_t *) & USBFIFO0;

            DEBUG_DATA("RX: total %d", end_point0_rx_count);
            for (i = 0; i < end_point0_rx_count; i++) {
                end_point0_rx_buf[i] = *(tempData + (i & 3));
                buff[i]=*(tempData + (i & 7));
            }
            DEBUG_HEX("buff:", buff, end_point0_rx_count);
            
            USBE0CSR0bits.RXRDYC = 1;

            // The packet length of control transfers in low speed devices must be 
            // 8 bytes, high speed devices allow a packet size of 8, 16, 32 or 64 bytes 
            // and full speed devices must have a packet size of 64 bytes.
            bmbRequest = (end_point0_rx_buf[0] << 8) | end_point0_rx_buf[1]; // bmRequestType, bRequest
            wValue = (end_point0_rx_buf[3] << 8) | end_point0_rx_buf[2];
            wIndex = (end_point0_rx_buf[5] << 8) | end_point0_rx_buf[4];
            wLength = (end_point0_rx_buf[7] << 8) | end_point0_rx_buf[6];

            if (end_point0_rx_buf[0] & 0x80) { /* A "1" indicates IN from the host or "0" indicates OUT to the host */
                controlTrans(); // Process Control Transfers and OUT Packets.
            } else {
                inPacket(); // Process OUT packets.
            }
        }


        if (USBE0CSR0bits.SETEND) {
            USBE0CSR0bits.SETENDC = 1;
        }

        USBCSR0bits.EP0IF = 0; // Clear the USB EndPoint 0 Interrupt Flag.
    }

    /* Endpoint 1 RX Interrupt Handler */
    if (USBCSR1bits.EP1RXIF) { // Endpoint 1 Received A Packet.
        rxUSB_ep1();
        // Insert your function here to process received packets
        USBCSR1bits.EP1RXIF = 0; // Supposedly Cleared By Hardware (Clear Just In Case).
    }

    /* Endpoint 2 TX Interrupt Handler */
    if (USBCSR0bits.EP2TXIF) { // Endpoint 2 Transmit A Packet.
        // To send packets via EP2 call function txUSB_ep2()
        // see how packets are sent in function controlTrans() as an example
        // otherwise this interrupt handler may have limited use depending
        // upon your implementation.
        USBCSR0bits.EP2TXIF = 0; // Supposedly Cleared By Hardware (Clear Just In Case). 
    }



    IFS4bits.USBIF = 0; // Reset the USB Interrupt flag
}

void init_usb(void) {
    // Configure the USB hardware and registers.
    USBCSR0bits.SOFTCONN = 0; // 1 = The USB D+/D- lines are enabled and active, 
    // 0 = The USB D+/D- lines are disabled and are tri-stated.

    USBCSR2bits.RESETIE = 0; // Disabled. When enabled IN packets/Set Address 
    // aren't received (unknown reason).

    /* Clear the Endpoint interrupt flags */
    USBCSR0bits.EP0IF = 0;
    USBCSR1bits.EP1RXIF = 0;
    USBCSR0bits.EP2TXIF = 0;

    IPC33bits.USBIP = 7; // USB Interrupt Priority.
    IPC33bits.USBIS = 1; // USB Interrupt Sub-Priority.

    usbAddress = 0;
    USBCSR0bits.FUNC = 0; // Initially set the USB address to 0 until
    // later when the host assigns an address

    // Configure endpoint 0 first.
    USBE0CSR0bits.TXMAXP = MAX_EP0_TX_SIZE; // Set endpoint 0 buffer to 64 bytes (multiples of 8).

    USBE1CSR0bits.MODE = 0; // 0 = Endpoint is RX.
    USBE2CSR0bits.MODE = 1; // 1 = Endpoint is TX

    /* These are multiples of 8, e.g.- "1" is 8 bytes, "8" is 64 bytes */
    USBE1CSR1bits.RXMAXP = EP1RXBUFFSIZE; // Endpoint 1 - Maximum RX Payload Per Transaction Control bits
    USBE2CSR0bits.TXMAXP = EP2TXBUFFSIZE; // Endpoint 2 - Maximum TX Payload per transaction Control bits

    USBE1CSR3bits.PROTOCOL = 1; // Endpoint 1 - RX Endpoint Protocol Control bits 
    USBE2CSR2bits.PROTOCOL = 1; // Endpoint 2 - TX Endpoint Protocol Control bits  
    //PROTOCOL<1:0>: RX/TX Endpoint Protocol Control bits 
    //11 = Interrupt
    //10 = Bulk
    //01 = Isochronous
    //00 = Control

    USBCSR3bits.ENDPOINT = 1;
    USBFIFOAbits.RXFIFOAD = 0x0000; // Receive Endpoint FIFO Address bits

    USBCSR3bits.ENDPOINT = 2;
    USBFIFOAbits.TXFIFOAD = 0x0040; // Transmit Endpoint FIFO Address bits

    USBE1CSR3bits.RXFIFOSZ = 0x6; // Receive FIFO Size bits - 64 bytes
    USBE2CSR3bits.TXFIFOSZ = 0x6; // Transmit FIFO Size bits - 64 bytes

    USBE0CSR2bits.SPEED = 1; // Endpoint 0 Operating Speed Control bits
    USBE1CSR3bits.SPEED = 1; // Endpoint 1: RX Endpoint Operating Speed Control bits - High speed
    USBE2CSR2bits.SPEED = 1; // Endpoint 2: TX Endpoint Operating Speed Control bits - High speed

    // NOTE: We don't set the TX speed in device mode, as noted below from the 
    // data sheet (DS60001320D-page 223):
    // TX Endpoint Operating Speed Control bits (Host mode)
    // In HOST mode it would use CSR2 to set the TX speed, for example:
    // USBExCSR2bits.SPEED = z; 
    // Where x is the endpoint number and z is the speed.

    USBE1CSR1bits.ISO = 1; // Isochronous RX Endpoint Enable bit (Device mode).
    USBE2CSR0bits.ISO = 1; // Isochronous TX Endpoint Enable bit (Device mode).

    /* Enable Endpoint interrupts */
    USBCSR1bits.EP0IE = 1; // Endpoint 0 interrupt enable
    USBCSR2bits.EP1RXIE = 1; // Endpoint 1 RX interrupt enable
    USBCSR1bits.EP2TXIE = 1; // Endpoint 2 TX interrupt enable

    IFS4bits.USBIF = 0; // Clear the USB interrupt flag.
    IEC4bits.USBIE = 1; // Enable the USB interrupt.
    USBCRCONbits.USBIE = 1; // 1 = Enables general interrupt from USB module

    USBCSR0bits.HSEN = 1; // 1 = Enable High Speed (480Mbps) USB mode.
    USBCSR0bits.SOFTCONN = 1; // 1 = The USB D+/D- lines are enabled and active, 0 = The USB D+/D- lines are disabled and are tri-stated.

}

void usb_handler(void) {

}