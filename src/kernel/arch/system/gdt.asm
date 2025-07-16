[bits 32]

global x86_GDT_Setup
x86_GDT_Setup:

    push ebp
    mov ebp, esp

    mov eax, [ebp + 8]
    lgdt [eax]

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