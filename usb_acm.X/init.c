#include "xc.h"

#include "global.h"

static void init_leds(void) {
    // Set up all ports to be digital and not analog
    ANSELH = 0;

    TRISHbits.TRISH0 = 0; //clear bit, make it digital output
    TRISHbits.TRISH1 = 0;
    TRISHbits.TRISH2 = 0;
}

static void init_timer2(void) {
    T2CONbits.ON = 0;
    PR2 = 12500;
    TMR2 = 0;

    T2CONbits.TCKPS = 3; // divide by 8: PR = 100,000,000 / 1,000 / 8 = 12500.
    T2CONbits.TCS = 0; // internal peripheral clock
    T2CONbits.ON = 1;
    IPC2bits.T2IP = 7; //interrupt priority
    IPC2bits.T2IS = 0; //3 sub priority
    IFS0bits.T2IF = 0; //interrupt flag
    IEC0bits.T2IE = 1; //enable
}

static void init_uart(void) {
    RPB14Rbits.RPB14R = 2; /* U2TX signal to pin RPB14, pin 61 on 144 pin package. */
    U2RXRbits.U2RXR = 1; /* RPG6 input to U2RX */
    LATDbits.LATD14 = 0; // Set RTS and CTS pins to 0
    LATDbits.LATD15 = 0;
    TRISHbits.TRISH0 = 0; /* LED1 */
    ANSELBbits.ANSB14 = 0; /* Digital mode. */
    ANSELGbits.ANSG6 = 0; /* Digital Input */
    TRISBbits.TRISB14 = 0;

    TRISGbits.TRISG6 = 1; /* U2RX must be Input. */
    U2MODEbits.ON = 0; /* Do Not enable peripheral until setup is complete. */
    U2MODEbits.BRGH = 0;
    //U2BRG = 129;
    int BAUD_RATE = 115200;
    //    U2BRG = (unsigned short int)(((float)PERIPHERAL_CLOCK / ((float)BAUD_RATE * (float)BAUD_CLOCK)) - (float)0.5);
    U2BRG = 53;

    U2MODEbits.STSEL = 0;
    U2MODEbits.PDSEL = 0b00;
    //	IEC4bits.U2TXIE = 1;				/* Not unless Interrupt Service Routine is available. */
    IPC36bits.U2TXIP = 0b111;
    IPC36bits.U2TXIS = 0b11;
    U2STAbits.UTXISEL = 0b10;
    U2STAbits.UTXEN = 1;
    U2MODEbits.UEN = 0b00;
    U2MODEbits.ON = 1;

    U2STAbits.URXEN = 1;
}

static void init_usb(void) {
    // Default CDC ACM settings

    CNPUFbits.CNPUF3 = 1; // Pull-up enables on RPF3 (To set USB as a B-Device )

    // Configure the USB hardware and registers.
    USBCSR0bits.SOFTCONN = 0; // 1 = The USB D+/D- lines are enabled and active, 
    // 0 = The USB D+/D- lines are disabled and are tri-stated.

    USBE1CSR0bits.MODE = 1; // 1 = Endpoint is TX.
    USBE2CSR0bits.MODE = 1; // 1 = Endpoint is TX
    USBE3CSR0bits.MODE = 0; // 0 = Endpoint is RX.

    IFS4bits.USBIF = 0; // Clear the USB interrupt flag.
    /* Clear the Endpoint interrupt flags */
    USBCSR0bits.EP0IF = 0;
    USBCSR0bits.EP1TXIF = 0;
    USBCSR0bits.EP2TXIF = 0;
    USBCSR1bits.EP3RXIF = 0;

    IPC33bits.USBIP = 4; // USB Interrupt Priority.
    IPC33bits.USBIS = 0; // USB Interrupt Sub-Priority.

    USBCSR0bits.FUNC = 0; // Initially set the USB address to 0 until
    // later when the host assigns an address

    // Configure endpoint 0 first.
    USBE0CSR0bits.TXMAXP = EP0BUFFSIZE; // Set endpoint 0 buffer to 64 bytes (multiples of 8).



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

    // See datasheet DS60001320E-page 228:
    USBE1CSR1bits.PIDERR = 1; // See DISNYET (same bit as PIDERR).
    USBE2CSR1bits.PIDERR = 1; // See DISNYET (same bit as PIDERR).
    USBE3CSR1bits.PIDERR = 1; // See DISNYET (same bit as PIDERR).
    // DISNYET: Disable NYET Handshakes Control/PID Error Status bit (Device mode)
    // 1 = In Bulk/Interrupt transactions, disables the sending of NYET handshakes. 
    // All successfully received RX packets are ACKed including at the point at 
    // which the FIFO becomes full. 
    // 0 = Normal operation.
    // In Bulk/Interrupt transactions, this bit only has any effect in Hi-Speed mode, 
    // in which mode it should be set for all Interrupt endpoints.
    //
    // PIDERR: PID Error Status bit (Host mode)
    // 1 = In ISO transactions, this indicates a PID error in the received packet. 
    // 0 = No error

    USBCSR3bits.ENDPOINT = 1;
    USBFIFOAbits.TXFIFOAD = 0x0000; // Receive Endpoint FIFO Address bits

    USBCSR3bits.ENDPOINT = 2;
    USBFIFOAbits.TXFIFOAD = 0x0040; // Transmit Endpoint FIFO Address bits

    USBCSR3bits.ENDPOINT = 3;
    USBFIFOAbits.RXFIFOAD = 0x0080; // Receive Endpoint FIFO Address bits

    // See data sheet DS60001320E-page 232
    USBE1CSR3bits.TXFIFOSZ = 0x4; // Transmit FIFO Size bits - 16 bytes
    USBE2CSR3bits.TXFIFOSZ = 0x6; // Transmit FIFO Size bits - 64 bytes
    USBE3CSR3bits.RXFIFOSZ = 0x6; // Receive FIFO Size bits - 64 bytes
    // 1101 = 8192 bytes 
    // 1100 = 4096 bytes
    // . 
    // . 
    // .
    // 0011 = 8 bytes
    // 0010 = Reserved
    // 0001 = Reserved
    // 0000 = Reserved or endpoint has not been configured

    USBE0CSR2bits.SPEED = 1; // Endpoint 0 Operating Speed Control bits
    USBE1CSR2bits.SPEED = 1; // Endpoint 1: TX Endpoint Operating Speed Control bits - High speed
    USBE2CSR2bits.SPEED = 1; // Endpoint 2: TX Endpoint Operating Speed Control bits - High speed
    USBE3CSR3bits.SPEED = 1; // Endpoint 3: RX Endpoint Operating Speed Control bits - High speed

    // NOTE: We don't set the TX speed in device mode, as noted below from the 
    // data sheet (DS60001320D-page 223):
    // TX Endpoint Operating Speed Control bits (Host mode)
    // In HOST mode it would use CSR2 to set the TX speed, for example:
    // USBExCSR2bits.SPEED = z; 
    // Where x is the endpoint number and z is the speed.

    USBE1CSR0bits.ISO = 0; // Isochronous TX Endpoint Disable bit (Device mode).
    USBE2CSR0bits.ISO = 0; // Isochronous TX Endpoint Disable bit (Device mode).
    USBE3CSR1bits.ISO = 0; // Isochronous RX Endpoint Disable bit (Device mode).

    /* Enable Endpoint interrupts */
    USBCSR1bits.EP0IE = 1; // Endpoint 0 interrupt enable
    USBCSR1bits.EP1TXIE = 1; // Endpoint 1 TX interrupt enable
    USBCSR1bits.EP2TXIE = 1; // Endpoint 2 TX interrupt enable
    USBCSR2bits.EP3RXIE = 1; // Endpoint 3 RX interrupt enable

    USBCSR2bits.RESETIE = 1; // Disabled. When enabled IN packets/Set Address 
    // aren't received (unknown reason).

    IEC4bits.USBIE = 1; // Enable the USB interrupt.
    USBCRCONbits.USBIE = 1; // 1 = Enables general interrupt from USB module

    // USBHS_DeviceAttach_Default
    USBCSR0bits.HSEN = 1; // 1 = Enable High Speed (480Mbps) USB mode.
    USBCSR0bits.SOFTCONN = 1; // 1 = The USB D+/D- lines are enabled and active, 0 = The USB D+/D- lines are disabled and are tri-stated.

}

static void init_board(void) {
    ANSELB = 0x7FFFFFD0; // Disable analog functions on B0, B1, B2, B3, B5 and B15   
    ANSELD = 0x00000000; // Disable analog functions on Port D (digital only).   

    TRISA = 0x0000; // Set all port A bits as outputs
    TRISB = 0x000C; // Set all port B bits as outputs except B2, B3
    TRISC = 0x0008; // Set all port C bits as outputs except C3 (U2RX)
    TRISD = 0x4208; // Set all port D bits as outputs except D14 (SDI1/DO), D9 (switch "-") and D3 (SDO1).
    TRISE = 0x0206; // Set all port E bits as outputs except E1, E2, E9
    TRISF = 0x0128; // Set all port F bits as outputs except F3 (USBID),F5 (switch "EN"), F8 (switch "+")
    TRISG = 0x1000; // Set port G12 as an input - from switch ES1

    /* Configure SPI1 ports */
    SDI1Rbits.SDI1R = 0xB; // Set RPD14 as SDI1
    RPD3Rbits.RPD3R = 0x5; // Set RPD3 as  SDO1

    /* Set Open Drain outputs for I2C busses. */
    ODCAbits.ODCA14 = 1; // Enable Open Drain on Port A14 for SCL1
    ODCAbits.ODCA15 = 1; // Enable Open Drain on Port A15 for SDA1
    ODCAbits.ODCA2 = 1; // Enable Open Drain on Port A2 for SCL2
    ODCAbits.ODCA3 = 1; // Enable Open Drain on Port A3 for SDA2

    /* Enable pull-up resistor on MISO/DO on SPI-1 */
    //CNPUDbits.CNPUD14 = 1;  // Pull-up enabled on RPD14
    CNPUFbits.CNPUF3 = 1; // Pull-up enables on RPF3 (To set USB as a B-Device )

    /* Configure PBCLK2 which is used by the UART and SPI Baud Rate Generators (BRG) */
    PB2DIVbits.ON = 0; // Disable PBCLK2
    T2CONbits.TCS = 0; // Use system clock for PBCLK2 for clock source.
    PB2DIVbits.PBDIV = 0x04; // Set the divisor for PBCLK2 (SYSCLK / 5).
    PB2DIVbits.ON = 1; // Enable PBCLK2

    /* Configure PBCLK3 which is used by the timers */
    PB3DIVbits.ON = 0; // Disable PBCLK3.
    T3CONbits.TCS = 0; // Use system clock PBCLK3 for clock source.
    PB3DIVbits.PBDIV = 0x63; // Set the divisor for PBCLK3 (SYSCLK / 100).
    PB3DIVbits.ON = 1; // Enable PBCLK3.

    /* Configure interrupts */
    PRISSbits.PRI1SS = 0x1; //PRIORITY 1 use Shadow Set 1
    PRISSbits.PRI2SS = 0x2; //.
    PRISSbits.PRI3SS = 0x3; //.
    PRISSbits.PRI4SS = 0x4; //.
    PRISSbits.PRI5SS = 0x5; //.
    PRISSbits.PRI6SS = 0x6; //.
    PRISSbits.PRI7SS = 0x7; //PRIORITY 7 use Shadow Set 7
    PRISSbits.SS0 = 0;
}

void initialize(void) {
    __builtin_disable_interrupts();
    init_board();
    init_leds();
    init_timer2();
    init_uart();
    init_usb();
    INTCONbits.MVEC = 1; //multi vector
    __builtin_enable_interrupts();
}