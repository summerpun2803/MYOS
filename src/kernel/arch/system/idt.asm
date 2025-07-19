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