#include "utils.h"
#include "arch/system/x86.h"
#include "stdio.h"
#include "arch/system/idt.h"

static inline uint8_t cmos_read(uint8_t reg)
{
    x86_outb(0x70, reg);
    return x86_inb(0x71);
}

void detect_floppies() 
{   
    uint8_t drives = cmos_read(0x10);

    uint8_t drive0 = (drives >> 4) & 0x0F;
    uint8_t drive1 = drives & 0x0F;

    // Example: type 4 = 1.44MB 3.5"
    printf("[Drive 0] type: %l\n", drive0);
    printf("[Drive 1] type: %l\n", drive1);
}

// void lba_2_chs(uint32_t lba, uint16_t* cyl, uint16_t* head, uint16_t* sector)
// {
//     *cyl    = lba / (2 * FLOPPY_144_SECTORS_PER_TRACK);
//     *head   = ((lba % (2 * FLOPPY_144_SECTORS_PER_TRACK)) / FLOPPY_144_SECTORS_PER_TRACK);
//     *sector = ((lba % (2 * FLOPPY_144_SECTORS_PER_TRACK)) % FLOPPY_144_SECTORS_PER_TRACK + 1);
// }

void wait_IRQ()
{   
    while(!g_fdc_irq_happened); 
    g_fdc_irq_happened = false;
    
}


void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms; i++) {
        for (uint32_t j = 0; j < 1000; j++) {
            io_wait();
        }
    }
}
