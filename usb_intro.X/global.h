#ifndef GLOBAL_H
#define GLOBAL_H

typedef struct{
    unsigned tick;
    
    unsigned timer_1s;
    unsigned timer_1m;
    unsigned timer_1h;
    unsigned timer_1d;
}SYS_TIMER;

extern SYS_TIMER my_sys_timer;

#endif