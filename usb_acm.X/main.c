#include "xc.h"

#include "debug.h"
#include "global.h"
#include <stdint.h>

SYS_TIMER my_sys_timer;

int rx_size;
char buf[1024];

int main(void) {
    
    initialize();
    
    char data[] = {0x11, 0x22, 0x33, 0x44};
    DEBUG_DATA("\r\n=================");
    DEBUG_HEX("\r\ntest: ", data, sizeof (data));

    print_prompt();
    while (1) {
        print_debug_buffer();

        if (my_sys_timer.tick > 1000) {
            my_sys_timer.tick = 0;
            my_sys_timer.timer_1s++;

            toggle_normal_led();
        }

        state_machine();

        debug_usb_events();

        get_next_cmd();
        Nop();
    }

}