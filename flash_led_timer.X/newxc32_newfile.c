


#include "xc.h"

#include <stdbool.h>
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

bool flash_flag=false;

void init_timer2(void) {
    __builtin_disable_interrupts();
    T2CONbits.ON = 0;
    PR2 = 12500;
    TMR2 = 0;

    T2CONbits.TCKPS = 3; // divide by 8: PR = 100,000,000 / 1,000 / 8 = 12500.
    T2CONbits.TCS = 0; // internal peripheral clock
    T2CONbits.ON = 1;
    IPC2bits.T2IP = 1; //interrupt priority
    IPC2bits.T2IS = 0; //3 sub priority
    IFS0bits.T2IF = 0; //interrupt flag
    IEC0bits.T2IE = 1; //enable
    INTCONbits.MVEC = 1; //multi vector
    __builtin_enable_interrupts();
}

int counter = 0;

void __attribute__((vector(_TIMER_2_VECTOR), interrupt(ipl3soft), nomips16)) timer2_handler() {

    counter++;
    if (counter > 1000) {
        counter = 0;

        flash_flag = true;
        //LATHINV = 7; //
    }
    IFS0bits.T2IF = 0; // Clear flag
}

static void init(void) {
    // Set up all ports to be digital and not analog
    ANSELA = 0;
    ANSELB = 0;
    ANSELC = 0;
    ANSELD = 0;
    ANSELE = 0;
    ANSELF = 0;
    ANSELG = 0;
    ANSELH = 0;
    ANSELJ = 0;

    TRISHbits.TRISH0 = 0; //clear bit, make it digital output
    TRISHbits.TRISH1 = 0;
    TRISHbits.TRISH2 = 0;
}

static void turn_on_all(void) {
    LATHbits.LATH0 = 1;
    LATHbits.LATH1 = 1;
    LATHbits.LATH2 = 1;
}

static void turn_off_all(void) {
    LATHbits.LATH0 = 0;
    LATHbits.LATH1 = 0;
    LATHbits.LATH2 = 0;
}

void toggle_led(void) {
    static bool on = false;
    if (on == true) {
        turn_on_all();
        on = false;
    } else {
        turn_off_all();
        on = true;
    }

}

int main(void) {
    int prev_tick = 0;
    int cur_tick = 0;
    init();

    init_timer2();

    while (1) {
        if (flash_flag) {
            flash_flag = false;
            cur_tick++;
            toggle_led();
        }

        if (cur_tick != prev_tick) {
            prev_tick = cur_tick;
        }

        Nop();
    }
}
