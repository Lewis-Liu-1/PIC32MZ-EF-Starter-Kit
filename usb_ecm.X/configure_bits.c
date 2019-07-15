#if 0
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
//24MHZ set to 3, get 8MHZ
#pragma config FPLLRNG = RANGE_5_10_MHZ // System PLL Input Range (5-10 MHz Input)
//8MHZ is nicely in 5~10MHZ
#pragma config FPLLICLK = PLL_POSC // System PLL Input Clock Selection (POSC is input to the System PLL)
#pragma config FPLLMULT = MUL_50 // System PLL Multiplier (PLL Multiply by 50)
//PLL - Phase Locked Loop, 
#pragma config FPLLODIV = DIV_2 // System PLL Output Clock Divider (2x Divider)

#pragma config UPLLFSEL = FREQ_12MHZ // USB PLL Input Frequency Selection (USB PLL input is 12 MHz)

/*** DEVCFG2 ***/
#pragma config FPLLIDIV =   DIV_2
#pragma config FPLLMUL =    MUL_15
#pragma config FPLLODIV =   DIV_1
#pragma config UPLLIDIV =   DIV_2
#pragma config UPLLEN =     ON

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
#endif

// <editor-fold defaultstate="collapsed" desc="Configuration Bits">

/*** DEVCFG0 ***/

#pragma config DEBUG =      OFF
#pragma config JTAGEN =     OFF
#pragma config ICESEL =     ICS_PGx2
#pragma config TRCEN =      OFF
#pragma config BOOTISA =    MIPS32
#pragma config FECCCON =    OFF_UNLOCKED
#pragma config FSLEEP =     OFF
#pragma config DBGPER =     PG_ALL
#pragma config SMCLR =      MCLR_NORM
#pragma config SOSCGAIN =   GAIN_2X
#pragma config SOSCBOOST =  ON
#pragma config POSCGAIN =   GAIN_2X
#pragma config POSCBOOST =  ON
#pragma config EJTAGBEN =   NORMAL
#pragma config CP =         OFF

/*** DEVCFG1 ***/

#pragma config FNOSC =      SPLL
#pragma config DMTINTV =    WIN_127_128
#pragma config FSOSCEN =    OFF
#pragma config IESO =       OFF
#pragma config POSCMOD =    EC
#pragma config OSCIOFNC =   OFF
#pragma config FCKSM =      CSDCMD
#pragma config WDTPS =      PS1048576
#pragma config WDTSPGM =    STOP
#pragma config FWDTEN =     OFF
#pragma config WINDIS =     NORMAL
#pragma config FWDTWINSZ =  WINSZ_25
#pragma config DMTCNT =     DMT31
#pragma config FDMTEN =     OFF
/*** DEVCFG2 ***/

#pragma config FPLLIDIV =   DIV_3
#pragma config FPLLRNG =    RANGE_5_10_MHZ
#pragma config FPLLICLK =   PLL_POSC
#pragma config FPLLMULT =   MUL_50
#pragma config FPLLODIV =   DIV_2
#pragma config UPLLFSEL =   FREQ_24MHZ
/*** DEVCFG3 ***/

#pragma config USERID =     0xffff
#pragma config FMIIEN =     OFF
#pragma config FETHIO =     OFF
#pragma config PGL1WAY =    OFF
#pragma config PMDL1WAY =   OFF
#pragma config IOL1WAY =    ON
#pragma config FUSBIDIO =   OFF

/*** BF1SEQ0 ***/

#pragma config TSEQ =       0xffff
#pragma config CSEQ =       0xffff
// </editor-fold>