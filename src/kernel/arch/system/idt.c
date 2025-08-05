#include "idt.h"
#include "stdio.h"
#include "isr.h"

#define IDT_SIZE    256
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
    idt[8]  = createIDTEntry((uint32_t)&isr8,  0x08, Interrupt_gate);
    idt[13] = createIDTEntry((uint32_t)&isr13, 0x08, Interrupt_gate);  // General Protection Fault
    idt[14] = createIDTEntry((uint32_t)&isr14, 0x08, Interrupt_gate);  // Page Fault

}

void hardware_interupts()
{   
    idt[0x20] = createIDTEntry((uint32_t)&irq0, 0x08, Interrupt_gate);
}
void init_idt()
{
    software_interupts();
    hardware_interupts();
    x86_IDT_Setup(&descriptor);
}

