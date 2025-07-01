#pragma once
#include <stdint.h>

void putc(char c);
void puts(const char* str);
void printf(const char* fmt, ...);
void hexdump(const uint8_t* data, uint32_t length);
void print_number(int num);
void clrscr();
void setcursor();
void Rmode_puts(const char* str);