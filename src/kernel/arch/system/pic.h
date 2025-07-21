#pragma once
#include "stdint.h"

#define PIC1_CMD_PORT   0x20
#define PIC1_DATA_PORT  0x21 
#define PIC2_CMD_PORT   0xA0
#define PIC2_DATA_PORT  0XA1

#define PIC_EOI		    0x20	

void PIC_sendEOI(uint8_t irq);
void io_wait();
void pic_remap(int offset1, int offset2);
void pic_disable();
