#include "../global.h"
#include "../debug.h"

#include <stddef.h>
#include <stdint.h>
#include <string.h>

struct usb_event {
    USB_EVENTS event;
    int len;
    uint8_t buff[MAX_BUF_SIZE];
};

struct usb_event events[MAX_EVENTS];
int event_in_index = 0;
int event_out_index = 0;

int prev;

const char * events_strings[] = {FOREACH_EVENT(GENERATE_STRING)};

static const char * get_event_name(int st) {
    int count;

    count = sizeof (events_strings) / sizeof (char *);
    if (st < count) {
        return events_strings[st];
    } else {

        return "UNKNOWN";
    }
}

void push_event(int ev, uint8_t *buf, int len) {
    events[event_in_index].event = ev;
    events[event_in_index].len = len;
    if (len > 0) {
        if (len > MAX_BUF_SIZE) len = MAX_BUF_SIZE;
        if (buf != NULL) {
            memcpy(events[event_in_index].buff, buf, len);
        }
    }
    event_in_index++;
    if (event_in_index >= MAX_EVENTS) {
        event_in_index = 0;
    }
}

void on_event(char * msg) {
    push_event(STRING_MSG, msg, strlen(msg));
}

void in_interrupt(void) {
    prev = event_in_index;
    push_event(USB_START, NULL, 0);
}

void out_interrupt(void) {
    if (prev == event_in_index) {
        push_event(USBIF, NULL, 0);
    }
}

static struct usb_event * get_next_event(void) {
    struct usb_event *ev = NULL;
    if (event_out_index != event_in_index) {
        ev = &events[event_out_index++];
    }

    if (event_out_index >= MAX_EVENTS) {
        event_out_index = 0;
    }
    return ev;
}

static void debug_event(struct usb_event *event) {
    char ret[100];
    strcpy(ret, get_event_name(event->event));
    DEBUG_DATA(ret);
    if (event->len > 0) {
        snprintf(ret, 100, "\t%d bytes \r\n\t", event->len);
        DEBUG_DATA(ret);
        char temp[4];
        int i;
        strcpy(ret, "");
        for (i = 0; i < event->len; i++) {
            snprintf(temp, 4, "%02X ", (event->buff[i]));
            strcat(ret, temp);
        }
        DEBUG_DATA(ret);
    }

}

void debug_usb_events(void) {
    struct usb_event *ev;
    while (1) {
        ev = get_next_event();
        if (ev == NULL) break;

        if (ev->event == STRING_MSG) {
            DEBUG_DATA(ev->buff);
            DEBUG_DATA("\r\n");
            continue;
        }

        if (ev->event == USB_START) {
            DEBUG_DATA("--------------");
        } else {
            debug_event(ev);
        }
        DEBUG_DATA("\r\n");
    }
}