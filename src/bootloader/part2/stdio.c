#include "stdio.h"
#include "x86.h"
#include <stdarg.h>
#include <stdbool.h>

#ifdef __INTELLISENSE__
#define va_arg(v, l) ((l)0)
#define va_start(v, l)
#define va_end(v)
#endif


const unsigned SCREEN_WIDTH = 80;
const unsigned SCREEN_HEIGHT = 25;
const uint8_t DEFAULT_COLOR = 0x7;

uint8_t* g_ScreenBuffer = (uint8_t *)0xB8000;
int g_ScreenX = 0, g_ScreenY = 0;

void putchr(int x, int y, char c)
{
    g_ScreenBuffer[2 * (y * SCREEN_WIDTH + x)] = c;
}

void putcolor(int x, int y, uint8_t color)
{
    g_ScreenBuffer[2 * (y * SCREEN_WIDTH + x) + 1] = color;
}

uint8_t getchr(int x, int y)
{
    return g_ScreenBuffer[2 * (y * SCREEN_WIDTH + x)];
}

uint8_t getcolor(int x, int y)
{
    return g_ScreenBuffer[2 * (y * SCREEN_WIDTH + x) + 1];
}

void scroll_back(uint16_t lines)
{
    for(int y=lines; y<SCREEN_HEIGHT; y++){
        for(int x=0; x<SCREEN_WIDTH; x++){
            putchr(x, y - lines, getchr(x, y));
            putcolor(x, y - lines, getcolor(x, y));
        }
    }

    for(int y=SCREEN_HEIGHT - lines; y<SCREEN_HEIGHT; y++){
        for(int x=0; x<SCREEN_HEIGHT; x++){
            putchr(x, y, '\0');
            putcolor(x, y, DEFAULT_COLOR);
        }
    }

    g_ScreenY -= lines; 
}
void putc(char c){

    switch (c)
    {
        case '\n':
            g_ScreenX = 0;
            g_ScreenY++;
            break;

        case '\t':
            for(int i=0; i<4 - (g_ScreenX % 4); i++){
                putc(' ');
            }
            break;

        case '\r':
            g_ScreenX = 0;
            break;
            
        default:
            putchr(g_ScreenX, g_ScreenY, c);
            g_ScreenX++;
            break;
    }
    if(g_ScreenX >= SCREEN_WIDTH){
        g_ScreenX = 0;
        g_ScreenY++;
    }
    if(g_ScreenY >= SCREEN_HEIGHT){
        scroll_back(1);
    }
    setcursor(g_ScreenX, g_ScreenY);
}

void puts(const char* str){
    
    while(*str){
        putc(*str);
        str++;
    }
}

void clrscr()
{
    for (int j=0; j < SCREEN_HEIGHT; j++){
        for(int i=0; i < SCREEN_WIDTH; i++){
            putchr(i, j, '\0');
            putcolor(i, j, DEFAULT_COLOR);
        }
    }
    setcursor(0, 0);
}

void setcursor(int x, int y){

    uint16_t pos = y * SCREEN_WIDTH + x;

    x86_outb(0x3D4, 0x0F);
	x86_outb(0x3D5, (uint8_t) (pos & 0xFF));
	x86_outb(0x3D4, 0x0E);
	x86_outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}


void print_longlong(long long num) {
    char buffer[21]; 
    int i = 0;

    if (num == 0) {
        putc('0');
        return;
    }

    if (num < 0) {
        putc('-');
        num = -num;
    }

    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }

    while (i--) {
        putc(buffer[i]);
    }
}

#define PRINTF_STATE_NORMAL         0
#define PRINTF_STATE_LENGTH         1
#define PRINTF_STATE_SPEC           2


void printf(const char *fmt, ...) {
    
    va_list args;
    va_start(args, fmt);

    int state = PRINTF_STATE_NORMAL;
    
    while(*fmt) {
        switch (state) {
            case PRINTF_STATE_NORMAL:
                switch (*fmt) {
                    case '%':
                        state = PRINTF_STATE_SPEC;
                        break;
                    
                    default:
                        putc(*fmt);
                        break;
                }
                break;

            case PRINTF_STATE_SPEC:
            PRINTF_STATE_SPEC_:
                switch(*fmt) {
                    case 'c':
                        putc((char) va_arg(args, int));
                        break;

                    case 's':
                        puts(va_arg(args, const char*));
                        break;
                    
                    case '%':
                        putc('%');
                        break;
                    
                    case 'd':
                        print_number(va_arg(args, int));
                        break;

                    case 'l':
                        print_number(va_arg(args, long long));
                        break;
                    
                    default:    break;
                }

                state = PRINTF_STATE_NORMAL;
                break;
        }
    
        fmt++;
    }
    va_end(args);
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
    puts("\n");
}

void Rmode_puts(const char* str){

    while(*str){

        x86_Rmode_putc(*str);
        str++;
    }
}