// void isr0_handler()  { printf("ISR 0: Divide by Zero\n"); }
// void isr1_handler()  { printf("ISR 1: Debug Exception\n"); }
// void isr2_handler()  { printf("ISR 2: Non Maskable Interrupt\n"); }
// void isr3_handler()  { printf("ISR 3: Breakpoint\n"); }
// void isr4_handler()  { printf("ISR 4: Overflow\n"); }
// void isr5_handler()  { printf("ISR 5: Bound Range Exceeded\n"); }
// void isr6_handler()  { printf("ISR 6: Invalid Opcode\n"); }
// void isr7_handler()  { printf("ISR 7: Device Not Available\n"); }
// void isr8_handler()  { printf("ISR 8: Double Fault\n"); }
// void isr9_handler()  { printf("ISR 9: Coprocessor Segment Overrun\n"); }
// void isr10_handler() { printf("ISR 10: Invalid TSS\n"); }
// void isr11_handler() { printf("ISR 11: Segment Not Present\n"); }
// void isr12_handler() { printf("ISR 12: Stack Segment Fault\n"); }
// void isr13_handler() { printf("ISR 13: General Protection Fault\n"); }
// void isr14_handler() { printf("ISR 14: Page Fault\n"); }
// void isr15_handler() { printf("ISR 15: Reserved\n"); }
// void isr16_handler() { printf("ISR 16: x87 Floating-Point Exception\n"); }
// void isr17_handler() { printf("ISR 17: Alignment Check\n"); }
// void isr18_handler() { printf("ISR 18: Machine Check\n"); }
// void isr19_handler() { printf("ISR 19: SIMD Floating-Point Exception\n"); }
// void isr20_handler() { printf("ISR 20: Virtualization Exception\n"); }
// void isr21_handler() { printf("ISR 21: Control Protection Exception\n"); }
// void isr22_handler() { printf("ISR 22: Reserved\n"); }
// void isr23_handler() { printf("ISR 23: Reserved\n"); }
// void isr24_handler() { printf("ISR 24: Reserved\n"); }
// void isr25_handler() { printf("ISR 25: Reserved\n"); }
// void isr26_handler() { printf("ISR 26: Reserved\n"); }
// void isr27_handler() { printf("ISR 27: Reserved\n"); }
// void isr28_handler() { printf("ISR 28: Hypervisor Injection Exception\n"); }
// void isr29_handler() { printf("ISR 29: VMM Communication Exception\n"); }
// void isr30_handler() { printf("ISR 30: Security Exception\n"); }
// void isr31_handler() { printf("ISR 31: Reserved\n"); }

#include <stdio.h>
#include "pic.h"
#include "x86.h"
extern uint32_t read_cr2();

void isr0_handler()  { printf("ISR 0: Divide by Zero\n"); while (1); }
void isr1_handler()  { printf("ISR 1: Debug Exception\n"); while (1); }
void isr2_handler()  { printf("ISR 2: Non Maskable Interrupt\n"); while (1); }
void isr3_handler()  { printf("ISR 3: Breakpoint\n"); while (1); }
void isr4_handler()  { printf("ISR 4: Overflow\n"); while (1); }
void isr5_handler()  { printf("ISR 5: Bound Range Exceeded\n"); while (1); }
void isr6_handler()  { printf("ISR 6: Invalid Opcode\n"); while (1); }
void isr7_handler()  { printf("ISR 7: Device Not Available\n"); while (1); }
void isr8_handler()  { printf("ISR 8: Double Fault\n"); while (1); }
void isr9_handler()  { printf("ISR 9: Coprocessor Segment Overrun\n"); while (1); }
void isr10_handler() { printf("ISR 10: Invalid TSS\n"); while (1); }
void isr11_handler() { printf("ISR 11: Segment Not Present\n"); while (1); }
void isr12_handler() { printf("ISR 12: Stack Segment Fault\n"); while (1); }
void isr13_handler() { 
    printf("ISR 13: General Protection Fault\n"); 
    panic();
    while (1); 
    panic();
}
void isr14_handler() { 

    printf("PAGE FAULT");
    panic();
    while (1); 
    panic();
}
void isr15_handler() { printf("ISR 15: Reserved\n"); while (1); }
void isr16_handler() { printf("ISR 16: x87 Floating-Point Exception\n"); while (1); }
void isr17_handler() { printf("ISR 17: Alignment Check\n"); while (1); }
void isr18_handler() { printf("ISR 18: Machine Check\n"); while (1); }
void isr19_handler() { printf("ISR 19: SIMD Floating-Point Exception\n"); while (1); }
void isr20_handler() { printf("ISR 20: Virtualization Exception\n"); while (1); }
void isr21_handler() { printf("ISR 21: Control Protection Exception\n"); while (1); }
void isr22_handler() { printf("ISR 22: Reserved\n"); while (1); }
void isr23_handler() { printf("ISR 23: Reserved\n"); while (1); }
void isr24_handler() { printf("ISR 24: Reserved\n"); while (1); }
void isr25_handler() { printf("ISR 25: Reserved\n"); while (1); }
void isr26_handler() { printf("ISR 26: Reserved\n"); while (1); }
void isr27_handler() { printf("ISR 27: Reserved\n"); while (1); }
void isr28_handler() { printf("ISR 28: Hypervisor Injection Exception\n"); while (1); }
void isr29_handler() { printf("ISR 29: VMM Communication Exception\n"); while (1); }
void isr30_handler() { printf("ISR 30: Security Exception\n"); while (1); }
void isr31_handler() { printf("ISR 31: Reserved\n"); while (1); }


void __attribute((cdecl)) timer_isr_handler() {
    printf(".");

    PIC_sendEOI(0);
}
