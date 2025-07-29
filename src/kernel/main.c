#include <stdint.h>
#include "stdio.h"
#include "memory.h"
#include <hal/hal.h>
#include <boot/bootparam.h>
#include <arch/memManager/mem.h>


extern uint8_t __bss_start;
extern uint8_t __end;
void __attribute__((section(".entry"))) start (bootparam* params){

    memset(&__bss_start, 0, (&__end) - (&__bss_start));
    clrscr();
    Hal_init();
    printf("Kernel: Hello\n");
    printf("BootDevice: %d \n", params->BootDevice);
    printf("Memory Entries: %d \n", params->memory.RegionCount);
    

    for(int i=0; i<params->memory.RegionCount; i++){
        printf("Base: %l Len: %l Type: %l\n",   params->memory.Region[i].Base,
                                                params->memory.Region[i].Length,
                                                params->memory.Region[i].Type);
    }
    BitMap_Init(&params->memory);

end:
    for(;;);
    
}
