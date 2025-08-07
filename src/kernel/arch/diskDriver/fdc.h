
static const int floppy_base = 0x03f0;
static const int floppy_irq = 6;

enum FDC_commands{

    RECALLIBRATE    =   0x07,
    SEEK            =   0x0F,
    SENSE_INTERRUPT =   0x08,
    READ_DATA       =   0x06,
    WRITE_DATA      =   0x05,
    SPECIFY         =   0x03
};

enum floppy_registers {

   FLOPPY_DOR  = 2,  
   FLOPPY_MSR  = 4,  
   FLOPPY_FIFO = 5,  
   FLOPPY_CCR  = 7   
};

