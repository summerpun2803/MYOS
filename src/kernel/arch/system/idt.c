#include "idt.h"
#include "stdio.h"

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

IDT_GATE idt[1];
IDT_Descriptor descriptor={sizeof(idt)-1, idt};

void isr0_handler() {
    printf("Interrupt: Divide by zero!\n");
    for (;;);
}

void init_idt()
{
    idt[0] = createIDTEntry((uint32_t)&isr0, 0x08, Interrupt_gate);
    x86_IDT_Setup(&descriptor);
}

