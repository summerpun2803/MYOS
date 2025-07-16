#include <stdint.h>
#include "stdio.h"
#include "memory.h"
#include "./arch/system/gdt.h"
#include "./arch/system/idt.h"

extern uint8_t __bss_start;
extern uint8_t __end;
void __attribute__((section(".entry"))) start (){

    memset(&__bss_start, 0, (&__end) - (&__bss_start));
    clrscr();
    init_gdt();
    init_idt();
    int x = 3/0;
    printf("Kernel: Hello");

end:
    for(;;);
    
}
