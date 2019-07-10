
#include <stdio.h> 
#include <stdarg.h>						
#include <string.h>

static void print_f(const char *str) {
    int i;
    for (i = 0; i < strlen(str); i++) {
        push_ch((char) str[i]);
    }

}

static void print_ln(char *str) {
    char *s = "\r\n";
    print_f(str);
    print_f(s);
}

static char *convert(unsigned int num, int base) {
    static char Representation[] = "0123456789ABCDEF";
    static char buffer[50];
    char *ptr;

    ptr = &buffer[49];
    *ptr = '\0';

    do {
        *--ptr = Representation[num % base];
        num /= base;
    } while (num != 0);

    return (ptr);
}

void my_printf(char* format, ...) {
    char *traverse;
    unsigned int i;
    char *s;

    //Module 1: Initializing Myprintf's arguments 
    va_list arg;
    va_start(arg, format);

    for (traverse = format; *traverse != '\0'; traverse++) {
        while (*traverse != '%') {
            if (*traverse == '\0') {
                return;
            }
            push_ch(*traverse);

            traverse++;
        }

        if (*traverse == '\0') {
            return;
        }
        traverse++;
        if (*traverse == '\0') {
            return;
        }

        //Module 2: Fetching and executing arguments
        switch (*traverse) {
            case 'c': i = va_arg(arg, int); //Fetch char argument
                push_ch(i);
                break;

            case 'd': i = va_arg(arg, int); //Fetch Decimal/Integer argument
                if (i < 0) {
                    i = -i;
                    push_ch('-');
                }
                print_f(convert(i, 10));
                break;

            case 'o': i = va_arg(arg, unsigned int); //Fetch Octal representation
                print_f(convert(i, 8));
                break;

            case 's': s = va_arg(arg, char *); //Fetch string
                print_f(s);
                break;

            case 'x': i = va_arg(arg, unsigned int); //Fetch Hexadecimal representation
                print_f(convert(i, 16));
                break;
        }
    }

    //Module 3: Closing argument list to necessary clean-up
    va_end(arg);
}

void DEBUG_HEX(char *caption, uint8_t *buf, uint8_t len) {
    int i;
    print_f(caption);
    for (i = 0; i < len; i++) {
        int hi = (buf[i] >> 4)&0xf;
        int low = buf[i]&0xf;
        print_f(convert(hi, 16));
        print_f(convert(low, 16));
        push_ch(' ');
    }
    print_ln("");
}

void print_prompt(void) {
    print_f("cmd>");
}
