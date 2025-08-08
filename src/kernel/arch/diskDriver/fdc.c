#include "fdc.h"
#include "arch/system/idt.h"
#include "arch/system/x86.h"

static void fdc_wait_irq() {
    while (!g_fdc_irq_happened); 
    g_fdc_irq_happened = false;
}

void FDC_Init_Floppy()
{
    x86_outb( 0x0a, MASK); 

    // Reset the master flip-flop
    x86_outb( 0x0c, FLIP_FLOP);  

    // Set the address for the transfer to 0x1000  
    x86_outb( 0x04, 0   );      
    x86_outb( 0x04, 0x10);      

    x86_outb( 0x0c, FLIP_FLOP);  

    x86_outb( 0x05, 0xFF);      
    x86_outb( 0x05, 0x23);      

    x86_outb( 0x81, 0   );      
    x86_outb( 0x0a, UNMASK);      

}

void FDC_Write_prep()
{
    x86_outb (0x0a, MASK); 
    x86_outb (0x0b, WRITE_MODE); 
    x86_outb (0x0a, UNMASK); 
}

void FDC_Read_Prep()
{
    x86_outb (0x0a, MASK); 
    x86_outb (0x0b, READ_MODE); 
    x86_outb (0x0a, UNMASK); 

}