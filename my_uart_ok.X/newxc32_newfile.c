// PIC32MZ2048EFM144 Configuration Bit Settings
// DEVCFG3
#pragma config USERID = 0xFFFF // Enter Hexadecimal value (Enter Hexadecimal value)
#pragma config FMIIEN = OFF 
#pragma config FETHIO = OFF 
#pragma config PGL1WAY = OFF 
#pragma config PMDL1WAY = OFF
#pragma config IOL1WAY = OFF
#pragma config FUSBIDIO = OFF

// DEVCFG2
										/* There is some inconsistency in Oscillator configuration: 
										 * If actual oscillator frequency is 12 MHz, 
										 * then frequency after PLL Input divider is 4 MHz
										 * which is below specification minimum of 5 MHz.
										 * If actual oscillator frequency is 24 MHz,
										 * then frequency after input divider is 8 MHz, 
										 * and frequency after VCO is 800 MHz,
										 * which is outside specification maximum 700 MHz,
										 * and with Output divider 2 would give too high system clock frequency */
#pragma config FPLLIDIV = DIV_3 // System PLL Input Divider (3x Divider)
#pragma config FPLLRNG = RANGE_5_10_MHZ // System PLL Input Range (5-10 MHz Input)
#pragma config FPLLICLK = PLL_POSC // System PLL Input Clock Selection (POSC is input to the System PLL)
#pragma config FPLLMULT = MUL_50 // System PLL Multiplier (PLL Multiply by 100)
#pragma config FPLLODIV = DIV_2 // System PLL Output Clock Divider (2x Divider)

#pragma config UPLLFSEL = FREQ_12MHZ // USB PLL Input Frequency Selection (USB PLL input is 12 MHz)

// DEVCFG1
#pragma config FNOSC = SPLL // Oscillator Selection Bits (System PLL)
#pragma config DMTINTV = WIN_127_128 // DMT Count Window Interval (Window/Interval value is 127/128 counter value)
#pragma config FSOSCEN = OFF // Secondary Oscillator Enable (Disable SOSC)
#pragma config IESO = OFF // Internal/External Switch Over (Disabled)
#pragma config POSCMOD = EC      // Primary Oscillator Configuration (External Clock signal)
#pragma config OSCIOFNC = OFF // CLKO Output Signal Active on the OSCO Pin (Disabled)
#pragma config FCKSM = CSDCMD // Clock Switching and Monitor Selection (Clock Switch Disabled, FSCM Disabled)
#pragma config WDTPS = PS1048576 // Watchdog Timer Postscaler (1:1048576)
#pragma config WDTSPGM = STOP // Watchdog Timer Stop During Flash Programming (WDT stops during Flash programming)
#pragma config WINDIS = NORMAL // Watchdog Timer Window Mode (Watchdog Timer is in non-Window mode)
#pragma config FWDTEN = OFF // Watchdog Timer Enable (WDT Disabled)
#pragma config FWDTWINSZ = WINSZ_25 // Watchdog Timer Window Size (Window size is 25%)
#pragma config DMTCNT = DMT31 // Deadman Timer Count Selection (2^31 (2147483648))
#pragma config FDMTEN = OFF // Deadman Timer Enable (Deadman Timer is disabled)

// DEVCFG0
#pragma config DEBUG = OFF // Background Debugger Enable (Debugger is disabled)
#pragma config JTAGEN = OFF // JTAG Enable (JTAG Disabled)
#pragma config ICESEL = ICS_PGx2 // ICE/ICD Comm Channel Select (Communicate on PGEC2/PGED2)
#pragma config TRCEN = OFF // Trace Enable (Trace features in the CPU are disabled)
#pragma config BOOTISA = MIPS32 // Boot ISA Selection (Boot code and Exception code is MIPS32)
#pragma config FECCCON = OFF_UNLOCKED // Dynamic Flash ECC Configuration (ECC and Dynamic ECC are disabled (ECCCON bits are writable))
#pragma config FSLEEP = OFF // Flash Sleep Mode (Flash is powered down when the device is in Sleep mode)
#pragma config DBGPER = PG_ALL // Debug Mode CPU Access Permission (Allow CPU access to all permission regions)
#pragma config SMCLR = MCLR_NORM // Soft Master Clear Enable bit (MCLR pin generates a normal system Reset)
#pragma config SOSCGAIN = GAIN_2X // Secondary Oscillator Gain Control bits (2x gain setting)
#pragma config SOSCBOOST = ON // Secondary Oscillator Boost Kick Start Enable bit (Boost the kick start of the oscillator)
#pragma config POSCGAIN = GAIN_2X // Primary Oscillator Gain Control bits (2x gain setting)
#pragma config POSCBOOST = ON // Primary Oscillator Boost Kick Start Enable bit (Boost the kick start of the oscillator)
#pragma config EJTAGBEN = NORMAL // EJTAG Boot (Normal EJTAG functionality)

// DEVCP0
#pragma config CP = OFF // Code Protect (Protection Disabled)

#include <xc.h>

void main()
{
	RPB14Rbits.RPB14R = 2;        /* U2TX signal to pin RPB14, pin 61 on 144 pin package. */
	U2RXRbits.U2RXR   = 1;			/* RPG6 input to U2RX */
	LATDbits.LATD14 = 0; // Set RTS and CTS pins to 0
	LATDbits.LATD15 = 0;
	TRISHbits.TRISH0 = 0;				/* LED1 */
	ANSELBbits.ANSB14 = 0;				/* Digital mode. */
	ANSELGbits.ANSG6 = 0;				/* Digital Input */
	TRISBbits.TRISB14 = 0;

	TRISGbits.TRISG6 = 1;				/* U2RX must be Input. */
	U2MODEbits.ON = 0;					/* Do Not enable peripheral until setup is complete. */
	U2MODEbits.BRGH = 0;
	//U2BRG = 129;
    int BAUD_RATE=115200;
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


	int i;
    char *hello="\r\nhello world\r\n";
	for (i = 0; i<strlen(hello); i++)
	{
		while (U2STAbits.UTXBF); // Wait while buffer is full
        U2TXREG = (char) hello[i];
	}
	while (1)
		Nop();
}