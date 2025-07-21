#include "idt.h"
#include "stdio.h"
#include "isr.h"

#define IDT_SIZE    2
uint8_t Interrupt_gate=0x8E;
uint8_t Trap_gate=0x8F;

IDT_GATE createIDTEntry(uint32_t Offset, uint16_t Segement_Selector, uint8_t Type){

    IDT_GATE gate;
    gate.Offset_1 = (uint16_t)(Offset & 0xFFFF);
    gate.Offset_2 = (uint16_t)((Offset >> 16) & 0xFFFF);
    gate.Reserved = 0;
    gate.Type = Type;
    gate.Segement_Selector = Segement_Selector;
    
    return gate;
}

IDT_GATE idt[IDT_SIZE];
IDT_Descriptor descriptor={sizeof(idt)-1, idt};

void software_interupts()
{
    idt[0]  = createIDTEntry((uint32_t)&isr0,  0x08, Interrupt_gate);
    idt[1]  = createIDTEntry((uint32_t)&isr1,  0x08, Interrupt_gate);

}

// void hardware_inturupts()
// {
//     idt[32] = createIDTEntry((uint32_t)&irq0, 0x08, Interrupt_gate); // IRQ0 - Timer
// }

void init_idt()
{
    software_interupts();
    // hardware_inturupts();

    // uint8_t mask = x86_inb(0x21);
    // mask &= ~(1 << 0); // Unmask IRQ0 (Timer)
    // mask &= ~(1 << 1); // Unmask IRQ1 (Keyboard)
    // x86_outb(0x21, mask);

    x86_IDT_Setup(&descriptor);
}

