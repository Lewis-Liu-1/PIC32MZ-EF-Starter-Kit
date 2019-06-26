
#include "xc.h"

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
}

static void turn_on_all(void) {
    TRISHbits.TRISH0 = 0; //digital output
    LATHbits.LATH0 = 1;
    PORTHbits.RH0 = 1;

    TRISHbits.TRISH1 = 0;
    LATHbits.LATH1 = 1;

    TRISHbits.TRISH2 = 0;
    LATHbits.LATH2 = 1;
}

static void turn_off_all(void) {
    TRISHbits.TRISH0 = 0; //digital output
    LATHbits.LATH0 = 1;
    PORTHbits.RH0 = 0;

    TRISHbits.TRISH1 = 0;
    LATHbits.LATH1 = 0;

    TRISHbits.TRISH2 = 0;
    LATHbits.LATH2 = 0;
}

#define CORE_TIMER_MILLISECONDS 10

unsigned int __attribute__((nomips16)) ReadCoreTimer(void) {
    unsigned int timer;

    // get the count reg
    asm volatile("mfc0   %0, $9" : "=r"(timer));

    return timer;
}

void CoreT_DelayMs(unsigned int delayMs) {
    unsigned int delayStart;

    delayStart = ReadCoreTimer();

    while ((ReadCoreTimer() - delayStart) < (delayMs * CORE_TIMER_MILLISECONDS));
}

int main(void) {
    int i, j;
    init();
    while (1) {
        turn_on_all();
        for (i = 0; i < 40000; i++)
            for (j = 0; j < 400; j++);
        //CoreT_DelayMs(1000);
        turn_off_all();
        for (i = 0; i < 40000; i++)
            for (j = 0; j < 400; j++);
        //CoreT_DelayMs(1000);
        Nop();
    }
}
