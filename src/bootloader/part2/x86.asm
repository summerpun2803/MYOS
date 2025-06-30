%macro x86_EnterRealMode 0
    [bits 32]
    jmp word 18h:.pmode16

.pmode16:
    [bits 16]
    mov eax, cr0
    and al, ~1
    mov cr0, eax

    jmp word 00h:.rmode

.rmode:
    mov ax, 0
    mov ds, ax
    mov ss, ax

    sti

%endmacro


%macro x86_EnterProtectedMode 0

    cli

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Far jump to flush prefetch queue and load CS
    jmp 08h:.PMode

.PMode:
    [bits 32]
    mov ax, 0x10          ; Data segment selector (3rd GDT entry)
    mov ds, ax
    mov ss, ax

%endmacro

; void x86_outb(uint16_t port, uint8_t value);
global x86_outb
x86_outb:
    [bits 32]

    mov dx, [esp + 4]
    mov al, [esp + 8]
    out dx, al

    ret

; uint8_t x86_inb(uint16_t port);
global x86_inb
x86_inb:
    [bits 32]

    xor eax, eax
    mov dx, [esp + 4]      ; Load port into edx
    in al, dx              ; Read byte from port DX into AL

    ret

global x86_Rmode_putc
x86_Rmode_putc:

    push ebp
    mov ebp, esp
    x86_EnterRealMode

    mov al, [bp + 8]
    mov ah, 0xe
    int 10h
    
    x86_EnterProtectedMode
    mov esp, ebp
    pop ebp
    ret