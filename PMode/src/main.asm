bits 16
org 0x7C00

%define ENDL 0x0D, 0x0A

jmp entry

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

entry:
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    call BIOSDisableA20
    call CheckA20
    call EnableA20
    call CheckA20

    cli
    lgdt [gdt_desc]

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Far jump to flush prefetch queue and load CS
    jmp 08h:PMode


CheckA20:
    pushf
    push ax
    push bx
    push si
    push di
    push ds
    push es
    cli

    xor ax, ax
    mov ds, ax
    mov si, 0x7DFE
    mov ax, [ds:si]
    mov bx, ax

    mov ax, 0xFFFF
    mov es, ax
    mov di, 0x7E0E
    cmp word [es:di], bx
    jne .enabled 

    rol bx, 1
    mov [ds:si], bx
    cmp word [es:di], bx
    jne .enabled
    
    

.disabled:
    mov si, msg_disabled
    call print
    jmp .done

.enabled:
    mov si, msg_enabled
    call print 

.done:
    sti
    pop es
    pop ds
    pop di
    pop si
    pop bx
    pop ax
    popf

BIOSEnableA20:
    push ax
    mov ax, 0x2401 
    int 0x15 
    pop ax
    ret

BIOSDisableA20:
    push ax
    mov ax, 0x2400 
    int 0x15 
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

print:
    push ax
    push bx

.loop:
    lodsb
    or al, al
    jz .done

    mov ah, 0x0E
    int 0x10
    jmp .loop

.done:
    pop bx
    pop ax
    ret


[bits 32]
PMode:
    mov ax, 0x10          ; Data segment selector (3rd GDT entry)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

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
    jmp word 18h:.pmode16

; .hang:
;     jmp .hang

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
    mov si, r_Hello
    call print

.hang:
    jmp .hang

msg_enabled: db 'A20 is enabled', ENDL, 0
msg_disabled: db 'A20 is disabled', ENDL, 0
p_Hello:    db "Protected Mode Enabled", 0
r_Hello:    db "Real Mode Enabled", 0

; Pad to 510 bytes
KbdCtrlDataPort     equ 0x60
KbdCtrlCmdPort      equ 0x64
KbdDisable          equ 0xAD
KbdEnable           equ 0xAE
KbdReadOutPort      equ 0xD0
KbdWriteOutPort     equ 0xD1
ScreenBuffer        equ 0xB8000

times 510 - ($ - $$) db 0
dw 0xAA55
