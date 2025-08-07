#include "stdint.h"
#include "stdbool.h"

typedef struct 
{
    uint16_t Offset_1;
    uint16_t Segement_Selector;
    uint8_t Reserved;
    uint8_t Type;
    uint16_t Offset_2;

}__attribute__((packed)) IDT_GATE;

typedef struct 
{
    uint16_t size;
    IDT_GATE *base;

}__attribute__((packed)) IDT_Descriptor;


extern uint8_t Interrupt_gate;
extern uint8_t Trap_gate;
extern volatile bool g_fdc_irq_happened;

IDT_GATE createIDTEntry(uint32_t Offset, uint16_t Segement_Selector, uint8_t Type);
void __attribute__((cdecl)) x86_IDT_Setup(IDT_Descriptor *descriptor);
void init_idt();