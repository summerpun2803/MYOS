[bits 32]

%macro ISR_NOERR 1
global isr%1
extern isr%1_handler
isr%1:
    cli
    push dword 0        ; fake error code for uniformity
    push dword %1       ; push interrupt/vector number
    pushad
    push esp            ; pass pointer to regs (optional)
    call isr%1_handler
    add esp, 4          ; remove pushed esp argument
    popad
    add esp, 8          ; remove int_no + fake_err pushed earlier
    sti
    iret
%endmacro

%macro ISR_ERR 1
global isr%1
extern isr%1_handler
isr%1:
    cli
    push dword %1       ; push interrupt/vector number
    ; NOTE: CPU already pushed the real error code BEFORE ISR entry
    pushad
    push esp            ; pass pointer to regs (optional)
    call isr%1_handler
    add esp, 4
    popad
    add esp, 4          ; remove pushed int_no (error code remains where CPU put it, but our handler reads it from regs)
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
ISR_ERR 8
ISR_NOERR 9
ISR_NOERR 10
ISR_NOERR 11
ISR_NOERR 12
ISR_ERR 13
ISR_ERR 14
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


; global irq0
; extern timer_isr_handler
; irq0:
;     cli
;     pusha
;     call timer_isr_handler   ; Optional C handler
;     popa
;     sti
;     iret

global read_cr2
read_cr2:
    mov eax, cr2
    ret

%macro IRQ 2
global irq%1
irq%1:
    cli
    pushad
    push 0    
    push %1     
    jmp irq_common_stub
%endmacro

IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38 
IRQ 7, 39

irq_common_stub:

    extern irq_handler ; The C handler
    call irq_handler
    add esp, 8
    popad
    sti
    iret