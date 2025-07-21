[bits 32]

%macro ISR_NOERR 1
global isr%1
extern isr%1_handler
isr%1:
    cli
    pusha
    call isr%1_handler
    popa
    sti
    iret
%endmacro

global x86_IDT_Setup
x86_IDT_Setup:

    push ebp
    mov ebp, esp

    mov eax, [ebp + 8]
    lidt [eax]

    mov esp, ebp
    pop ebp
    ret

ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_NOERR 8
ISR_NOERR 9
ISR_NOERR 10
ISR_NOERR 11
ISR_NOERR 12
ISR_NOERR 13
ISR_NOERR 14
ISR_NOERR 15
ISR_NOERR 16
ISR_NOERR 17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_NOERR 30
ISR_NOERR 31


global irq0
extern timer_isr_handler
irq0:
    cli
    call timer_isr_handler   ; Optional C handler
    sti
    iret
