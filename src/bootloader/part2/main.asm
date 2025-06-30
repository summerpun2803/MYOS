bits 16

%define ENDL 0x0D, 0x0A

section .entry

extern __bss_start
extern __end

extern _cstart_
global entry

entry:
    cli 

    mov [g_bootDrive], dl
    mov ax, ds
    mov ss, ax
    mov sp, 0xFFF0
    mov bp, sp

    call EnableA20
    lgdt [gdt_desc]

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Far jump to flush prefetch queue and load CS
    jmp 08h:PMode

print:
    push ax
    push bx

.loop:
    lodsb
    or al, al
    jz .print_done

    mov ah, 0x0E
    int 0x10
    jmp .loop

.print_done:
    pop bx
    pop ax
    ret

EnableA20:

    call waitInput
    mov al, KbdDisable
    out KbdCtrlCmdPort, al

    call waitInput
    mov al, KbdReadOutPort
    out KbdCtrlCmdPort, al

    call waitOutput
    in al, KbdCtrlDataPort
    push eax

    call waitInput
    mov al, KbdWriteOutPort
    out KbdCtrlCmdPort, al

    call waitInput
    pop eax
    or al, 2
    out KbdCtrlDataPort, al

    call waitInput
    mov al, KbdEnable
    out KbdCtrlCmdPort, al

    ret

waitOutput:
    in al, KbdCtrlCmdPort
    test al, 1
    jz waitOutput
    ret

waitInput:
    in al, KbdCtrlCmdPort
    test al, 2
    jnz waitInput
    ret

gdt:

gdt_null:
    dq 0

gdt_table:
    ;   code 32 bit
    dw 0FFFFh           ; Limit low
    dw 0                ; Base low
    db 0                ; Base middle
    db 10011010b        ; Access byte: code segment
    db 11001111b        ; Flags and limit high
    db 0                ; Base high

    ;   data 32 bit
    dw 0FFFFh
    dw 0
    db 0
    db 10010010b       ; Access byte: data segment
    db 11001111b
    db 0

    ;   code 16 bit
    dw 0FFFFh           ; Limit low
    dw 0                ; Base low
    db 0                ; Base middle
    db 10011010b        ; Access byte: code segment
    db 00001111b        ; Flags and limit high
    db 0                ; Base high

    ;   data 16 bit
    dw 0FFFFh
    dw 0
    db 0
    db 10010010b       ; Access byte: data segment
    db 00001111b
    db 0

gdt_end:

gdt_desc:
    dw gdt_end - gdt - 1  ; GDT size (limit = size - 1)
    dd gdt                ; GDT base (32-bit)


PMode:
    [bits 32]
    mov ax, 0x10          ; Data segment selector (3rd GDT entry)
    mov ds, ax
    mov ss, ax

    mov edi, __bss_start
    mov ecx, __end
    sub ecx, edi
    mov al, 0
    cld
    rep stosb

    mov esi, p_Hello
    mov edi, ScreenBuffer

.loop:
    lodsb
    or al, al
    jz .done
    mov [edi], al
    inc edi
    inc edi
    jmp .loop

.done:
    xor edx, edx
    mov dl, [g_bootDrive]
    push edx
    call _cstart_


KbdCtrlDataPort     equ 0x60
KbdCtrlCmdPort      equ 0x64
KbdDisable          equ 0xAD
KbdEnable           equ 0xAE
KbdReadOutPort      equ 0xD0
KbdWriteOutPort     equ 0xD1
ScreenBuffer        equ 0xB8000

p_Hello:    db "Protected Mode Enabled", 0

g_bootDrive:    db 0
