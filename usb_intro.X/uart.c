#include "xc.h"

#include "debug.h"

#include <string.h>

void init_uart(void) {
    RPB14Rbits.RPB14R = 2; /* U2TX signal to pin RPB14, pin 61 on 144 pin package. */
    U2RXRbits.U2RXR = 1; /* RPG6 input to U2RX */
    LATDbits.LATD14 = 0; // Set RTS and CTS pins to 0
    LATDbits.LATD15 = 0;
    TRISHbits.TRISH0 = 0; /* LED1 */
    ANSELBbits.ANSB14 = 0; /* Digital mode. */
    ANSELGbits.ANSG6 = 0; /* Digital Input */
    TRISBbits.TRISB14 = 0;

    TRISGbits.TRISG6 = 1; /* U2RX must be Input. */
    U2MODEbits.ON = 0; /* Do Not enable peripheral until setup is complete. */
    U2MODEbits.BRGH = 0;
    //U2BRG = 129;
    int BAUD_RATE = 115200;
    //    U2BRG = (unsigned short int)(((float)PERIPHERAL_CLOCK / ((float)BAUD_RATE * (float)BAUD_CLOCK)) - (float)0.5);
    U2BRG = 53;

    U2MODEbits.STSEL = 0;
    U2MODEbits.PDSEL = 0b00;
    //	IEC4bits.U2TXIE = 1;				/* Not unless Interrupt Service Routine is available. */
    IPC36bits.U2TXIP = 0b111;
    IPC36bits.U2TXIS = 0b11;
    U2STAbits.UTXISEL = 0b10;
    U2STAbits.UTXEN = 1;
    U2MODEbits.UEN = 0b00;
    U2MODEbits.ON = 1;
    
    U2STAbits.URXEN =1;
    

}

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

