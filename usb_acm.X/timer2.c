#include "xc.h"
#include "global.h"

#include <stdbool.h>



void __attribute__((vector(_TIMER_2_VECTOR), interrupt(ipl7soft), nomips16)) timer2_handler() {

    my_sys_timer.tick++;
    IFS0bits.T2IF = 0; // Clear flag
}
