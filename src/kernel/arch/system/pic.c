#include "pic.h"
#include "x86.h"

#define PIC_READ_IRR                0x0a    /* OCW3 irq ready next CMD read */
#define PIC_READ_ISR                0x0b    /* OCW3 irq service next CMD read */

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
    io_wait();
    x86_outb(PIC2_CMD_PORT, 0x11);
    io_wait();

    x86_outb(PIC1_DATA_PORT, offset1); // Master vector offset
    io_wait();
    x86_outb(PIC2_DATA_PORT, offset2); // Slave vector offset
    io_wait();

    x86_outb(PIC1_DATA_PORT, 0x04);    // Tell Master that Slave is at IRQ2
    io_wait();
    x86_outb(PIC2_DATA_PORT, 0x02);    // Tell Slave its cascade ID
    io_wait();

    x86_outb(PIC1_DATA_PORT, 0x01);    // 8086 mode
    io_wait();
    x86_outb(PIC2_DATA_PORT, 0x01);    // 8086 mx86_outb
    io_wait();

    x86_outb(PIC1_DATA_PORT, 0);
    io_wait();
	x86_outb(PIC2_DATA_PORT, 0);
    io_wait();

}

void mask_irq(uint8_t IRQline)
{
    uint16_t port;
    uint8_t value;
    
    if(IRQline < 8) port = PIC1_DATA_PORT;
    else{
        IRQline -= 8;
        port = PIC2_DATA_PORT;
    }

    value = x86_inb(port) | (1 << IRQline);
    x86_outb(port, value);
}

void unmask_irq(uint8_t IRQline)
{
    uint16_t port;
    uint8_t value;
    
    if(IRQline < 8) port = PIC1_DATA_PORT;
    else{
        IRQline -= 8;
        port = PIC2_DATA_PORT;
    }

    value = x86_inb(port) & ~(1 << IRQline);
    x86_outb(port, value);
}

void pic_disable()
{
    x86_outb(PIC1_DATA_PORT, 0xff);
    x86_outb(PIC2_DATA_PORT, 0xff);
}

uint16_t PIC_readRequestReg(int val)
{
    x86_outb(PIC1_CMD_PORT, val);
    x86_outb(PIC2_CMD_PORT, val);

    return (x86_inb(PIC2_DATA_PORT) << 8) | (x86_inb(PIC1_DATA_PORT));
}

uint16_t PIC_readISR_Request()
{
    return PIC_readRequestReg(PIC_READ_ISR);
}

uint16_t PIC_readIRR_Request()
{
    return PIC_readRequestReg(PIC_READ_IRR);
}