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
    for(int i=0; i<params->memory.RegionCount; i++){

        MemoryMap* region = &params->memory.Region[i];
        printf("Region ptr at [%d]: %l\n", i, (void*)&params->memory.Region[i]);
        printf("Region %d:\n", i);
        printf("  Base:   %l\n", region->Base);
        printf("  Length: %l\n", region->Length);
        printf("  Type:   %d\n", region->Type);

    }   
    BitMap_Init(&params->memory);

end:
    for(;;);
    
}
