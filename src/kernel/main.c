#include <stdint.h>
#include "stdio.h"
#include "memory.h"
#include <hal/hal.h>
#include <boot/bootparam.h>


extern uint8_t __bss_start;
extern uint8_t __end;
void __attribute__((section(".entry"))) start (bootparam* params){

    memset(&__bss_start, 0, (&__end) - (&__bss_start));
    clrscr();
    Hal_init();
    printf("Kernel: Hello\n");
    printf("BootDevice: %d", params->BootDevice);

end:
    for(;;);
    
}
