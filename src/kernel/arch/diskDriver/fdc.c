#include "fdc.h"
#include "arch/system/idt.h"

static void fdc_wait_irq() {
    while (!g_fdc_irq_happened); 
    g_fdc_irq_happened = false;
}