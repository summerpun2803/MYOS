#include "stdio.h"
#include "x86.h"

void putc(char c){

    x86_Video_WriteCharTeletype(c, 0);

}

void puts(const char* str){
    
    while(*str){
        putc(*str);
        str++;
    }
}

#define PRINTF_STATE_NORMAL         0
#define PRINTF_STATE_LENGTH         1
#define PRINTF_STATE_LENGTH_SHORT   2
#define PRINTF_STATE_LENGTH_LONG    3
#define PRINTF_STATE_SPEC           4

#define PRINTF_LENGTH_DEFAULT       0
#define PRINTF_LENGTH_SHORT_SHORT   1
#define PRINTF_LENGTH_SHORT         2
#define PRINTF_LENGTH_LONG          3
#define PRINTF_LENGTH_LONG_LONG     4


void _cdecl printf(const char *fmt, ...) {
    
    int *argp = (int*) &fmt;

    int state = PRINTF_STATE_NORMAL;
    int length = PRINTF_LENGTH_DEFAULT;
    argp++;
    
    while(*fmt) {
        switch (state) {
            case PRINTF_STATE_NORMAL:
                switch (*fmt) {
                    case '%':
                        state = PRINTF_STATE_LENGTH;
                        break;
                    
                    default:
                        putc(*fmt);
                        break;
                }
                break;

            case PRINTF_STATE_LENGTH:
                switch (*fmt) {
                    case 'h':
                        state = PRINTF_STATE_LENGTH_SHORT;
                        length = PRINTF_LENGTH_SHORT;
                        break;
                    
                    case 'l':
                        state = PRINTF_STATE_LENGTH_LONG;
                        length = PRINTF_LENGTH_LONG;
                        break;

                    default:
                        goto PRINTF_STATE_SPEC_;
                        break;
                }
                break;

            case PRINTF_STATE_LENGTH_SHORT:
                switch(*fmt) {
                    case 'h':
                        length = PRINTF_LENGTH_SHORT_SHORT;
                        state = PRINTF_STATE_SPEC;
                        break;
                    
                    default:  
                        goto PRINTF_STATE_SPEC_;
                        break;
                }
                break;

            case PRINTF_STATE_LENGTH_LONG:
                switch(*fmt) {
                    case 'l':
                        length = PRINTF_LENGTH_LONG_LONG;
                        state = PRINTF_STATE_SPEC;
                        break;
                    
                    default:  
                        goto PRINTF_STATE_SPEC_;
                        break;
                }
                break;

            case PRINTF_STATE_SPEC:
            PRINTF_STATE_SPEC_:
                switch(*fmt) {
                    case 'c':
                        putc((char) *argp);
                        argp++;
                        break;

                    case 's':
                        puts(*(const char **)argp);
                        argp++;
                        break;
                    
                    case '%':
                        putc('%');
                        break;
                    
                    case 'd':
                        print_number(*argp);
                        argp++;
                        break;
                    
                    default:    break;
                }

                state = PRINTF_STATE_NORMAL;
                length = PRINTF_LENGTH_DEFAULT;
                break;
        }
    
        fmt++;
    }
}

void print_number(int num){
    char buffer[12];
    int i=0;

    if(num == 0){
        putc('0');
        return;
    }

    if(num < 0){
        putc('-');
        num = -num;
    }

    while(num > 0){
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    while(i--) putc(buffer[i]);
}

void hexdump(const uint8_t* data, uint32_t length) {
    for (uint32_t i = 0; i < length; i++) {
        // Print one byte in hex
        uint8_t byte = data[i];
        char hex[] = "0123456789ABCDEF";
        putc(hex[(byte >> 4) & 0xF]);
        putc(hex[byte & 0xF]);

        // Add space
        putc(' ');
    }
    puts("\r\n");
}