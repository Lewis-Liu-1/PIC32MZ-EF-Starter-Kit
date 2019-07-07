#include "xc.h"
#include "global.h"

#include <stdbool.h>

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

void __attribute__((vector(_TIMER_2_VECTOR), interrupt(ipl3soft), nomips16)) timer2_handler() {

    my_sys_timer.tick++;
    IFS0bits.T2IF = 0; // Clear flag
}
