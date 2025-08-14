#include "hal.h"
#include <arch/system/gdt.h>
#include <arch/system/idt.h>
#include <arch/system/pic.h>
#include <arch/system/x86.h>
#include <arch/memManager/vmm.h>
#include <arch/diskDriver/utils.h>
#include <arch/diskDriver/fdc.h>

void Hal_init()
{
    init_gdt();
    init_idt();
    pic_remap(0x20, 0x28);
    // x86_test();  
    vmmngr_initialize();
    // trigger_page_fault();
    detect_floppies();
    x86_test();  
    FDA_Init();
    DMA_Init();
    
}