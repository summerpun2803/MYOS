
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

global x86_GDT_Setup
x86_GDT_Setup:
    [bits 32]

    push ebp
    mov ebp, esp

    mov eax, [ebp + 8]
    lgdt[eax]

    mov eax, [ebp + 12]
    push eax
    push .reload_cs
    retf

.reload_cs

    mov eax, [ebp + 16]
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp, ebp
    pop ebp
    ret