#pragma once
#include "gdt.h"
#include<stdint.h>
#include<stdbool.h>

void __attribute__((cdecl)) x86_outb(uint16_t port, uint8_t value);
uint8_t __attribute__((cdecl)) x86_inb(uint16_t port);
void __attribute__((cdecl)) x86_GDT_Setup(GDTDescriptor descriptor, uint16_t code_seg, uint16_t data_seg);