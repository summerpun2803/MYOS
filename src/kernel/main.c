#include <stdint.h>
#include "stdio.h"
#include "memory.h"
#include <hal/hal.h>


extern uint8_t __bss_start;
extern uint8_t __end;
void __attribute__((section(".entry"))) start (){

    memset(&__bss_start, 0, (&__end) - (&__bss_start));
    clrscr();
    Hal_init();
    int x = 3/0;
    printf("Kernel: Hello");

end:
    for(;;);
    
}
