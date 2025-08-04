#include "hal.h"
#include <arch/system/gdt.h>
#include <arch/system/idt.h>
#include <arch/system/pic.h>
#include <arch/system/x86.h>
#include <arch/memManager/vmm.h>

void Hal_init()
{
    init_gdt();
    init_idt();
    pic_remap(0x20, 0x28);
    // x86_test();  
}