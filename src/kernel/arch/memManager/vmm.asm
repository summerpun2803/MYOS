[BITS 32]

global loadPageDirectory
global enablePaging
global flush_tlb_entry

; Loads the physical address of a page directory into CR3
loadPageDirectory:
    push ebp
    mov ebp, esp
    mov eax, [ebp + 8]  ; Get argument from stack
    mov cr3, eax
    pop ebp
    ret

; Enables the PG bit in CR0
enablePaging:
    ; This function takes no arguments, so no stack frame needed.
    mov eax, cr0
    or eax, 0x80000000 ; Set only the Paging (PG) bit
    mov cr0, eax
    ret

; Flushes a single page from the TLB cache
flush_tlb_entry:
    push ebp
    mov ebp, esp
    mov eax, [ebp + 8]  ; Get virtual address from stack
    invlpg [eax]
    pop ebp
    ret