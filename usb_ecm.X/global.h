//https://www.beyondlogic.org/usbnutshell/usb4.shtml
///opt/microchip/xc32/v2.20/pic32mx/include/proc/p32mz2048efh144.h
#ifndef GLOBAL_H    /* Guard against multiple inclusion */
#define GLOBAL_H

#ifdef __cplusplus
extern "C" {
#endif

#define EP0BUFFSIZE     8   // In multiples of 8, e.g.- 8 (x8) equals a size of 64
#define EP1TXBUFFSIZE   1   // Also in multiples of 8, e.g.- 1 (x8) equals a size of 8
#define EP2TXBUFFSIZE   8   // Also in multiples of 8, e.g.- 8 (x8) equals a size of 64
#define EP3RXBUFFSIZE   8   // Also in multiples of 8, e.g.- 8 (x8) equals a size of 64
#define EP0_MAX_PACKET_SIZE  (EP0BUFFSIZE * 8)
#define EP1_MAX_PACKET_SIZE  (EP1TXBUFFSIZE * 8)
#define EP2_MAX_PACKET_SIZE  (EP2TXBUFFSIZE * 8)
#define EP3_MAX_PACKET_SIZE  (EP3RXBUFFSIZE * 8)

#define MAX_EVENTS 50
#define MAX_BUF_SIZE 120

#define GENERATE_ENUM(ENUM) ENUM,
#define GENERATE_STRING(STRING) #STRING,

#define FOREACH_EVENT(STATE) \
STATE(USB_START)\
    STATE(RESET)\
    STATE(EP0_RX_READY)\
    STATE(STRING_MSG)\
    STATE(TX_SENT)\
    STATE(RECV_DATA)\
    STATE(SET_ADDR)\
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
