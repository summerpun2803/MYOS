#pragma once

typedef struct regs {
    /* pushad order: EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI */
    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;

    uint32_t int_no;     /* pushed by stub */
    uint32_t err_code;   /* real CPU error code or fake 0 (for NOERR) */

    /* CPU pushed frame */
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
} regs_t;

void __attribute((cdecl)) isr0();
void __attribute((cdecl)) isr1();
void __attribute((cdecl)) isr2();
void __attribute((cdecl)) isr3();
void __attribute((cdecl)) isr4();
void __attribute((cdecl)) isr5();
void __attribute((cdecl)) isr6();
void __attribute((cdecl)) isr7();
void __attribute((cdecl)) isr8();
void __attribute((cdecl)) isr9();
void __attribute((cdecl)) isr10();
void __attribute((cdecl)) isr11();
void __attribute((cdecl)) isr12();
void __attribute((cdecl)) isr13();
void __attribute((cdecl)) isr14();
void __attribute((cdecl)) isr15();
void __attribute((cdecl)) isr16();
void __attribute((cdecl)) isr17();
void __attribute((cdecl)) isr18();
void __attribute((cdecl)) isr19();
void __attribute((cdecl)) isr20();
void __attribute((cdecl)) isr21();
void __attribute((cdecl)) isr22();
void __attribute((cdecl)) isr23();
void __attribute((cdecl)) isr24();
void __attribute((cdecl)) isr25();
void __attribute((cdecl)) isr26();
void __attribute((cdecl)) isr27();
void __attribute((cdecl)) isr28();
void __attribute((cdecl)) isr29();
void __attribute((cdecl)) isr30();
void __attribute((cdecl)) isr31();

void __attribute((cdecl)) irq0();
void __attribute((cdecl)) irq6();
void __attribute((cdecl)) timer_isr_handler();