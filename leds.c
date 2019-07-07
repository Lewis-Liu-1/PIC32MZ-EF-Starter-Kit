#include "xc.h"
#include "debug.h"

#include <stdbool.h>

void init_leds(void) {
    // Set up all ports to be digital and not analog
    ANSELH = 0;

    TRISHbits.TRISH0 = 0; //clear bit, make it digital output
    TRISHbits.TRISH1 = 0;
    TRISHbits.TRISH2 = 0;
}

void toggle_normal_led(void) {
    static bool on = false;
    if (on == true) {
        LATHbits.LATH0 = 1;
        on = false;
    } else {
        LATHbits.LATH0 = 0;
        on = true;
    }
}

void toggle_usb_rx_led(void) {
    static bool on = false;
    if (on == true) {
        LATHbits.LATH1 = 1;
        on = false;
    } else {
        LATHbits.LATH1 = 0;
        on = true;
    }
}

void turn_on_led(void) {
    LATHbits.LATH2 = 1;
    DEBUG_DATA("\r\nON LED");
}

void turn_off_led(void) {
    LATHbits.LATH2 = 0;
    DEBUG_DATA("\r\nOFF LED");

}