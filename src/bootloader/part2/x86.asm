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

    push ebp
    mov ebp, esp

    x86_EnterRealMode

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

    mov ax, 1
    sbb ax, 0

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
    
    x86_EnterProtectedMode

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

    mov ax, 1
    sbb ax, 0           ; 1 on success, 0 on fail   

    x86_EnterProtectedMode

    ; restore old call frame
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

    push ebp
    mov ebp, esp

    x86_EnterRealMode

    push es
    push bx

    mov ax, [ebp + 12]    
    mov ch, al              ;   ch = cylinder 6 bits

    mov ax, [bp + 16]
    mov cl, al
    and cl, 3Fh

    shr ax, 2
    and al, 0xc0
    or cl, al               ;   cl = cylinder upper 2 bits

    mov ax, [bp + 32]       
    mov es, ax
    mov bx, [bp + 28]       ;   setting ES:BX to output buffer

    mov ah, 0x02
    mov al, [bp + 24]       ;   al = sector count

    mov dl, [bp + 8]        ;   dl = drive number

    mov dh, [bp + 20]       ;   dh = head
    stc
    int 0x13

    mov ax, 1
    sbb ax, 0   

    pop bx
    pop es

    x86_EnterRealMode
    mov esp, ebp
    pop ebp
    ret