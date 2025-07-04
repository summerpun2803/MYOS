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
    [bits 16]
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

; Convert linear address to segment:offset address
; Args:
;    1 - linear address
;    2 - (out) target segment (e.g. es)
;    3 - target 32-bit register to use (e.g. eax)
;    4 - target lower 16-bit half of #3 (e.g. ax)

%macro LinearToSegOffset 4

    mov %3, %1
    shr %3, 4
    mov %2, %4
    mov %3, %1
    and %3, 0xf

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


;
;Getting disk Parameters
;
;bool _cdecl x86_Disk_Param(uint8_t driveNumber,
;                            uint8_t* driveTypeOut,
;                            uint16_t* cylinderOut,
;                            uint16_t* SectorOut,
;                            uint16_t *headsOut);
;
global x86_Disk_Param
x86_Disk_Param:
    [bits 32]
    push ebp
    mov ebp, esp

    x86_EnterRealMode
    [bits 16]
    push es
    push bx
    push di
    push si
    
    mov ah, 08h
    mov dl, [ebp + 8]
    mov di, 0
    mov es, di
    stc
    int 0x13

    mov eax, 1
    sbb eax, 0

    mov si, [bp + 12]
    mov [si], bl

    mov bl, ch 
    mov bh, cl
    shr bh, 6
    mov si, [bp + 16]
    mov [si], bx

    xor ch, ch 
    and cl, 3Fh
    mov si, [bp + 20]
    mov [si], cx

    mov cl, dh
    mov si, [bp + 24]
    mov [si], cx

    pop si
    pop di
    pop bx
    pop es
    
    push eax

    x86_EnterProtectedMode
    [bits 32]

    pop eax
    mov esp, ebp
    pop ebp
    ret


;
; bool _cdecl x86_Disk_Reset(uint8_t drive);
;
global x86_Disk_Reset
x86_Disk_Reset:

    ; make new call frame
    push ebp             ; save old call frame
    mov ebp, esp          ; initialize new call frame

    x86_EnterRealMode

    mov ah, 0
    mov dl, [bp + 4]    ; dl - drive
    stc
    int 13h

    mov eax, 1
    sbb eax, 0           ; 1 on success, 0 on fail   

    push eax
    x86_EnterProtectedMode

    ; restore old call frame
    pop eax
    mov esp, ebp
    pop ebp
    ret

;
; Reading the Sector
;
;bool _cdecl x86_Disk_Read(
;    uint8_t drive,
;    uint16_t cylinder,
;    uint16_t sector,
;    uint16_t head,
;    uint8_t count,
;    void* bufferOut
; );
;
global x86_Disk_Read
x86_Disk_Read:

    ; make new call frame
    push ebp             ; save old call frame
    mov ebp, esp          ; initialize new call frame

    x86_EnterRealMode

    ; save modified regs
    push ebx
    push es

    ; setup args
    mov dl, [bp + 8]    ; dl - drive

    mov ch, [bp + 12]    ; ch - cylinder (lower 8 bits)
    mov cl, [bp + 13]    ; cl - cylinder to bits 6-7
    shl cl, 6
    
    mov al, [bp + 16]    ; cl - sector to bits 0-5
    and al, 3Fh
    or cl, al

    mov dh, [bp + 20]   ; dh - head

    mov al, [bp + 24]   ; al - count

    LinearToSegOffset [bp + 28], es, ebx, bx

    ; call int13h
    mov ah, 02h
    stc
    int 13h

    ; set return value
    mov eax, 1
    sbb eax, 0           ; 1 on success, 0 on fail   

    ; restore regs
    pop es
    pop ebx

    push eax

    x86_EnterProtectedMode

    pop eax

    ; restore old call frame
    mov esp, ebp
    pop ebp
    ret
