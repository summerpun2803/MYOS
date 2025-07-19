#include "hal.h"
#include <arch/system/gdt.h>
#include <arch/system/idt.h>

void Hal_init()
{
    init_gdt();
    init_idt();
}