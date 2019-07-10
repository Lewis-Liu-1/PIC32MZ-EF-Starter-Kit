#include <stdbool.h>

void on_cmd(char *cmd, int len) {
    if (!strcmp(cmd, "on")) {
        turn_on_led();
    } else if (!strcmp(cmd, "off")) {
        turn_off_led();
    } else if (!strcmp(cmd, "usb log on")) {
        //enable_usb_log(true);
    } else if (!strcmp(cmd, "usb log off")) {
        //enable_usb_log(false);
    }
}