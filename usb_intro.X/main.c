#include "xc.h"

#include "global.h"
#include <stdint.h>

SYS_TIMER my_sys_timer;

int main(void)
{
    init_leds();
    init_timer2();
    init_uart();
    
    init_usb();
    print_ln("\r\n");
    while (1)
    {
        print_debug_buffer();
        
        if (my_sys_timer.tick>1000)
        {
            my_sys_timer.tick=0;
            my_sys_timer.timer_1s++;
            
            toggle_normal_led();
        }
        
        state_machine();
        
        usb_handler();
		Nop();
    }

}