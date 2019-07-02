#include "xc.h"
#include "global.h"

#include <stdint.h>

enum {
    START,
    INIT,
    RUNNING,
};
uint8_t state = START;
uint8_t state_timer = 0;

void state_machine(void) {
    static uint8_t prev_time = 0;
    if (prev_time != my_sys_timer.timer_1s) {
        prev_time = my_sys_timer.timer_1s;
        if (state_timer) {
            state_timer--;
        }
    }

    if (state_timer) {
        return;
    }

    switch (state) {
        case START:
            state_timer = 2;
            state = INIT;
            print_ln("start");
            break;
        case INIT:
            print_ln("init");
            state = RUNNING;
            break;
        case RUNNING:
            break;
        default:
            break;
    }
}

void sys_normal(void) {

}