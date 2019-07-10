#include "xc.h"

#include "debug.h"

#include <string.h>


#define MAX_DEBUG_SIZE 4098
uint8_t debug_buf[MAX_DEBUG_SIZE];
uint16_t in_index = 0;
uint16_t out_index = 0;

void print_debug_buffer(void) {
    int i;
    uint8_t ch;
    if (in_index == out_index) return;

    while (in_index != out_index) {
        ch = debug_buf[out_index++];
        while (U2STAbits.UTXBF); // Wait while buffer is full
        U2TXREG = ch;
        if (out_index >= MAX_DEBUG_SIZE) {
            out_index = 0;
        }
    }

}

void push_ch(char c) {
    debug_buf[in_index++] = c;
    if (in_index >= MAX_DEBUG_SIZE) {
        in_index = 0;
    }
}

char buffer[1024];
int cmd_index=0;
void get_next_cmd(void)
{
    if (!U2STAbits.URXDA)
    {
        return;
    }
    char x = U2RXREG;
    if (x==0x0d || x==0xa)
    {
        buffer[cmd_index++]=0;
        on_cmd(buffer,index);
        cmd_index=0;
        push_ch('\n');
        push_ch('\r');
        
        print_prompt();
    }
    else
    {
        buffer[cmd_index++]=x;
        push_ch(x);
    }
    
}

