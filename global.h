
///opt/microchip/xc32/v2.20/pic32mx/include/proc/p32mz2048efh144.h
#ifndef GLOBAL_H    /* Guard against multiple inclusion */
#define GLOBAL_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_EVENTS 20
#define MAX_BUF_SIZE 100

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

#define FOREACH_EVENT(STATE) \
STATE(USB_START)\
    STATE(RESET)\
    STATE(EP0)\
    STATE(EP0_RX_READY)\
    STATE(GET_STATUS_DEVICE)\
    STATE(GET_STATUS_INTERFACE)\
    STATE(GET_STATUS_ENDPOINT)\
    STATE(GET_DESCRIPTOR)\
    STATE(UNDEFINED)\
    STATE(GET_DEVICE_DESCRIPTOR)\
    STATE(SEND_DEVICE_DESCRIPTOR)\
    STATE(GET_CONFIG_DESCRIPTOR)\
    STATE(SEND_CONFIG_DESCRIPTOR)\
    STATE(GET_STRING_DESCRIPTOR)\
    STATE(SEND_LANGUAGE_ID)\
    STATE(SEND_MANUFACTURER)\
    STATE(SEND_PRODUCT)\
    STATE(SEND_SERIAL)\
    STATE(SEND_CONFIGURATION)\
    STATE(SEND_INTERFACE)\
    STATE(GET_CONFIGURATION)\
    STATE(GET_INTERFACE)\
    STATE(GET_STATUS)\
    STATE(CLEAR_FEATURE)\
    STATE(SET_FEATURE)\
    STATE(SET_ADDR)\
    STATE(CALL_SET_ADDR)\
    STATE(SET_CONFIGURATION)\
    STATE(EP1RX)\
    STATE(EP2TX)\
    STATE(USBIF)

    typedef enum STATE_TYPES { //
        FOREACH_EVENT(GENERATE_ENUM)
    } USB_EVENTS;

#define show_func_enter()\
DEBUG_DATA("\r\n~~~~~~~~~~~~~~~~: %s :~~~~~~~~~~~~~~~~~~\r\n", __func__)


typedef struct{
    unsigned tick;
    
    unsigned timer_1s;
    unsigned timer_1m;
    unsigned timer_1h;
    unsigned timer_1d;
}SYS_TIMER;

extern SYS_TIMER my_sys_timer;
    
#ifdef __cplusplus
}
#endif

#endif /* GLOBAL_H */
