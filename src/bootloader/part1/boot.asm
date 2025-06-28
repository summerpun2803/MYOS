
org 0x7C00
bits 16

%define ENDL 0x0D, 0x0A
%define BASE 0x7C00

main:

    jmp short start_boot
    nop

    ; ---------- BIOS Parameter Block (BPB) ----------
    oem_name:              db 'mkfs.fat'         ; 8 bytes
    bytes_per_sector:      dw 512                ; 0x00B
    sectors_per_cluster:   db 1                  ; 0x00D
    reserved_sectors:      dw 1                  ; 0x00E
    num_fats:              db 2                  ; 0x010
    max_root_dir_entries:  dw 224                ; 0x011
    total_sectors:         dw 2880               ; 0x013
    media_descriptor:      db 0xF0               ; 0x015
    sectors_per_fat:       dw 9                  ; 0x016
    sectors_per_track:     dw 18                 ; 0x018
    number_of_heads:       dw 2                  ; 0x01A
    hidden_sectors:        dd 0                  ; 0x01C
    total_sectors_32:      dd 0                  ; 0x020

    ; ---------- Extended BPB ----------
    drive_number:          db 0                  ; 0x024
    reserved1:             db 0                  ; 0x025
    boot_signature:        db 0x29               ; 0x026
    volume_id:             dd 0x4E16299F         ; 0x027
    volume_label:          db 'BOS       '       ; 11 bytes, 0x02B
    fs_type:               db 'FAT12   '         ; 8 bytes, 0x036

start_boot:
    ; setting up segements
    mov ax, 0
    mov ds, ax
    mov es, ax

    ; setting up the stack pointers
    mov ss, ax
    mov sp, 0x7C00            

    mov [drive_number], dl

    push es
    mov ah, 08h
    int 13h
    jc read_fail
    pop es

    and cl, 0x3F                        ; remove top 2 bits
    xor ch, ch
    mov [sectors_per_track], cx         ; sector count

    inc dh
    mov [number_of_heads], dh           ; head count

    mov si, msg_hello
    call print

    ; compute LBA of root directories
    ; reserved + number of fats * sector per fat

    mov ax, [sectors_per_fat]
    mov bl, [num_fats]
    xor bh, bh
    mul bx                              ; ax = fats * sector per fat
    add ax, [reserved_sectors]          ; ax = LBA
    push ax                             ; pushing ax to stack 

    ; compute the size of root dirs
    ; 32 * number of entries / bytes per sector

    mov ax, [max_root_dir_entries]      ; ax = number of entries
    shl ax, 5                           ; ax = 32 * number of entries
    xor dx, dx                          ; dx = 0
    div word [bytes_per_sector]         ; ax = number of sector
                                        ; dx = remainder
    test dx, dx                         ; if dx != 0, add 1
    jz .root_dir_after
    inc ax                              ; division remainder != 0, add 1

.root_dir_after:

    mov cl, al                          ; cl = number of sectors
    pop ax                              ; ax = LBA
    mov dl, [drive_number]              ; dl = drive number
    mov bx, buffer                      ; es:bx = buffer
    call read_disk

    xor bx, bx
    mov di, buffer

.search_kernel:

    mov si, file_kernel_bin
    mov cx, 11
    push di
    cmpsb                               ; searching for kernel.bin
    pop di
    je .found_kernel

    add di, 32                          ; moving to next directory
    inc bx                              
    cmp bx, [max_root_dir_entries]      ; if all the directories is not checked repeat the 
    jl .search_kernel

    jmp kernel_not_found_error

.found_kernel:

    ; di should have the address to the entry
    mov ax, [di + 26]                   ; first logical cluster field (offset 26)
    mov [kernel_cluster], ax

    ; load FAT from disk into memory
    mov ax, [reserved_sectors]
    mov bx, buffer
    mov cl, [sectors_per_fat]
    mov dl, [drive_number]
    call read_disk

    ; read kernel and process FAT chain
    mov bx, KERNEL_LOAD_SEGMENT
    mov es, bx
    mov bx, KERNEL_LOAD_OFFSET

.load_kernel_loop:

    mov ax, [kernel_cluster]

    add ax, 31
    mov cl, 1
    mov dl, [drive_number]
    call read_disk

    add bx, [bytes_per_sector]

    mov ax, [kernel_cluster]
    mov cx, 3
    mul cx
    mov cx, 2
    div cx

    mov si, buffer
    add si, ax
    mov ax, [ds:si]

    or dx, dx
    jz .even

.odd:
    shr ax, 4
    jmp .next_cluster_after

.even:
    and ax, 0x0FFF

.next_cluster_after:
    cmp ax, 0x0FF8
    jae .finish_read

    mov [kernel_cluster], ax
    jmp .load_kernel_loop

.finish_read:

    mov dl, [drive_number]

    mov ax, KERNEL_LOAD_SEGMENT
    mov ds, ax
    mov es, ax

    jmp KERNEL_LOAD_SEGMENT:KERNEL_LOAD_OFFSET


    ; printing whatever that si register points to
print:
    push ax
    push bx

.loop:
    lodsb 
    or al, al
    jz .done

    mov ah, 0x0E
    mov bh, 0
    int 0x10

    jmp .loop

.done:
    pop bx
    pop ax


;
; Disk routines
;

;
; Converts an LBA address to a CHS address
; Parameters:
;   - ax: LBA address
; Returns:
;   - cx [bits 0-5]: sector number
;   - cx [bits 6-15]: cylinder
;   - dh: head
;
lba_to_chs:

    push ax
    push dx

    xor dx, dx                          ; dx = 0
    div word [sectors_per_track]        ; ax = (LBA / sectors_per_track)
                                        ; dx = LBA %  sectors_per_track
    inc dx                              ; dx++
    mov cx, dx                          ; sector = cx = dx

    xor dx, dx                          ; dx = 0

    div word [number_of_heads]          ; ax = (LBA / sectors_per_track) /  heads
                                        ; dx = (LBA / sectors_per_track) % heads
    mov dh, dl                          ; dh = head
    mov ch, al                          ; ch = cylinder (lower 8 bits)
    shl ah, 6                           ; ah = left shift by 6 
    or cl, ah                           ; cl = cylinder (only upper 2 bits)

    pop ax
    mov dl, al                          ; restore only dl
    pop ax
    ret


;  Reads from Disk
;  Params:
;       ax: LBA adress
;       cl: number of sectors to read
;       dl: drive number
;       ES:BX : memory address to store the data
;

read_disk:
    push ax                             
    push bx
    push cx
    push dx
    push di                 ; save registers we will modify

    push cx

    call lba_to_chs

    pop ax
    mov ah, 0x02
    int 0x13

    jc read_fail            ; if carry set => failure

    pop di
    pop dx
    pop cx
    pop bx
    pop ax 
    ret

read_fail:
    mov si, read_fail_msg
    call print

kernel_not_found_error:
    mov si, msg_kernel_not_found
    call print


msg_hello:              db 'Hello from Bootloader!', ENDL, 0
read_fail_msg:          db 'Disk Read Failed!', ENDL, 0
msg_kernel_not_found:   db 'STAGE2.BIN file not found!', ENDL, 0
file_kernel_bin:        db 'STAGE2  BIN'
kernel_cluster:         dw 0

KERNEL_LOAD_SEGMENT     equ 0x2000
KERNEL_LOAD_OFFSET      equ 0

times 510 - ($ - $$) db 0
dw 0AA55h

buffer: