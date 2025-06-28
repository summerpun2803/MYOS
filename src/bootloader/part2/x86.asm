bits 16

section _TEXT class=CODE

; 
; Print function
; int 10h, ah=0Eh
; args: characters, page
;
global _x86_Video_WriteCharTeletype
_x86_Video_WriteCharTeletype:

    ; make a call frame
    push bp         ; save old base pointor
    mov bp, sp      ; save the top pointer

    push bx

    mov ah, 0Eh
    mov al, [bp + 4]
    mov bh, [bp + 6]
    int 10h

    pop bx

    mov sp, bp
    pop bp
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
global _x86_Disk_Read
_x86_Disk_Read:

    push bp
    mov bp, sp

    push es
    push bx

    mov ax, [bp + 6]    
    mov ch, al              ;   ch = cylinder 6 bits

    mov ax, [bp + 8]
    mov cl, al
    and cl, 3Fh

    shr ax, 2
    and al, 0xc0
    or cl, al               ;   cl = cylinder upper 2 bits

    mov ax, [bp + 16]       
    mov es, ax
    mov bx, [bp + 14]       ;   setting ES:BX to output buffer

    mov ah, 0x02
    mov al, [bp + 12]       ;   al = sector count

    mov dl, [bp + 4]        ;   dl = drive number

    mov dh, [bp + 10]       ;   dh = head
    stc
    int 0x13

    mov ax, 1
    sbb ax, 0   

    pop bx
    pop es

    mov sp, bp
    pop bp
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
global _x86_Disk_Param
_x86_Disk_Param:

    push bp
    mov bp, sp

    push es
    push bx
    push di
    push si

    mov ah, 08h
    mov dl, [bp + 4]
    mov di, 0
    mov es, di
    stc
    int 0x13

    mov ax, 1
    sbb ax, 0

    mov si, [bp + 6]
    mov [si], bl

    mov bl, ch 
    mov bh, cl
    shr bh, 6
    mov si, [bp + 8]
    mov [si], bx

    xor ch, ch 
    and cl, 3Fh
    mov si, [bp + 10]
    mov [si], cx

    mov cl, dh
    mov si, [bp + 12]
    mov [si], cx

    pop si
    pop di
    pop bx
    pop es
    
    mov sp, bp
    pop bp
    ret

;
; bool _cdecl x86_Disk_Reset(uint8_t drive);
;
global _x86_Disk_Reset
_x86_Disk_Reset:

    ; make new call frame
    push bp             ; save old call frame
    mov bp, sp          ; initialize new call frame

    mov ah, 0
    mov dl, [bp + 4]    ; dl - drive
    stc
    int 13h

    mov ax, 1
    sbb ax, 0           ; 1 on success, 0 on fail   

    ; restore old call frame
    mov sp, bp
    pop bp
    ret


global __U4D
__U4D:
    shl edx, 16         ; dx to upper half of edx
    mov dx, ax          ; edx - dividend
    mov eax, edx        ; eax - dividend
    xor edx, edx

    shl ecx, 16         ; cx to upper half of ecx
    mov cx, bx          ; ecx - divisor

    div ecx             ; eax - quot, edx - remainder
    mov ebx, edx
    mov ecx, edx
    shr ecx, 16

    mov edx, eax
    shr edx, 16

    ret

global __U4M
__U4M:
    shl edx, 16         ; dx to upper half of edx
    mov dx, ax          ; m1 in edx
    mov eax, edx        ; m1 in eax

    shl ecx, 16         ; cx to upper half of ecx
    mov cx, bx          ; m2 in ecx

    mul ecx             ; result in edx:eax (we only need eax)
    mov edx, eax        ; move upper half to dx
    shr edx, 16

    ret