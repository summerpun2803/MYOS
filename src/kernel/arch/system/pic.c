#include "pic.h"
#include "x86.h"


void PIC_sendEOI(uint8_t irq)
{   
    if(irq >= 8)    x86_outb(PIC2_CMD_PORT, PIC_EOI);

    x86_outb(PIC1_CMD_PORT, PIC_EOI); 
}

void io_wait()
{
    x86_outb(0x80, 0);
}

void pic_remap(int offset1, int offset2) 
{

    x86_outb(PIC1_CMD_PORT, 0x11);
    x86_outb(PIC2_CMD_PORT, 0x11);
    x86_outb(PIC1_DATA_PORT, offset1); // Master vector offset
    x86_outb(PIC2_DATA_PORT, offset2); // Slave vector offset
    x86_outb(PIC1_DATA_PORT, 0x04);    // Tell Master that Slave is at IRQ2
    x86_outb(PIC2_DATA_PORT, 0x02);    // Tell Slave its cascade ID
    x86_outb(PIC1_DATA_PORT, 0x01);    // 8086 mode
    x86_outb(PIC2_DATA_PORT, 0x01);    // 8086 mx86_outb
    x86_outb(PIC1_DATA_PORT, 0);
	x86_outb(PIC2_DATA_PORT, 0);

}

void pic_disable()
{
    x86_outb(PIC1_DATA_PORT, 0xff);
    x86_outb(PIC2_DATA_PORT, 0xff);
}