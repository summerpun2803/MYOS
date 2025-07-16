[bits 32]

global x86_IDT_Setup
x86_IDT_Setup:

    push ebp
    mov ebp, esp

    mov eax, [ebp + 8]
    lidt [eax]

    mov esp, ebp
    pop ebp
    ret

global isr0
extern isr0_handler
isr0:
    cli
    pusha
    call isr0_handler
    popa
    sti
    iret