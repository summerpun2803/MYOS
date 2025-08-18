#include "fdc.h"
#include "arch/system/x86.h"
#include "arch/system/idt.h"
#include "utils.h"
#include "stdio.h"

void flpydsk_disable_controller () {
 
	x86_outb (DIGITAL_OUTPUT_REGISTER, 0);
}

void flpydsk_enable_controller () {
 
	x86_outb (DIGITAL_OUTPUT_REGISTER, FLPYDSK_DOR_MASK_DMA | FLPYDSK_DOR_MASK_RESET);
}

void lba_to_chs(uint32_t lba, uint8_t* cyl, uint8_t* head, uint8_t* sector) {
    *cyl = lba / (18 * 2);
    uint32_t temp = lba % (18 * 2);
    *head = temp / 18;
    *sector = (temp % 18) + 1;
}

void FDC_read(uint32_t lba)
{   
    uint8_t cyl, head, sector;
    lba_to_chs(lba, &cyl, &head, &sector);
    fdc_seek(head, cyl, 0);

    prepare_DMA_read();

    fdc_write(FDC_CMD_READ_SECT);
    fdc_write(head << 2 | 0);
    fdc_write(cyl);
    fdc_write(head);
    fdc_write(sector);

    fdc_write(2);
    fdc_write(18);
    fdc_write(0x1b);
    fdc_write(0xff);
    
    wait_IRQ();

    uint8_t status[7];
    for (int i = 0; i < 7; i++) {
        status[i] = flpydsk_read_data();
    }

    if (status[0] & 0xC0) {
        printf("[FDC]: Read Error! ST0=%x, ST1=%x, ST2=%x\n", status[0], status[1], status[2]);
    }
}

void fdc_configure()
{   
    uint8_t ok = 1;
    ok &= fdc_write(FDC_CMD_CONFIGURE);
    ok &= fdc_write(0);
    ok &= fdc_write(0x57);
    ok &= fdc_write(0);
    if(!ok)
        printf("[FDC]: Configure Error");
}

void fdc_seek(uint8_t head, uint8_t cylinder, uint8_t drive)
{   
    uint32_t st0, cyl;
    uint8_t ok = 1;
    ok &= fdc_write(FDC_CMD_SEEK);
    ok &= fdc_write((head<<2) | drive);
    ok &= fdc_write(cylinder);

    if(!ok)
        printf("[FDC]: SEEK Error");

    wait_IRQ();
    flpydsk_check_int(&st0, &cyl);
    if(cyl != cylinder) 
        printf("[FDC]: Seek failed! (st0: 0x%x, Target: %d, Actual: %d)\n",st0 ,cylinder, cyl);
}   

int FDC_recalibrate(uint8_t drive)
{
    uint32_t st0, cyl;
    uint32_t timeout = 10;
    while(timeout--){
        uint8_t ok = 1;
        ok &= fdc_write(FDC_CMD_CALIBRATE);
        ok &= fdc_write(drive);

        wait_IRQ();
        flpydsk_check_int(&st0, &cyl);
        if(cyl == 0 && ok) return 1;
    }   
    printf("[FDC]: Recalbration Timeout");
    return 0;
}

void FDC_reset()
{
    flpydsk_disable_controller();
    delay_ms(100);

    flpydsk_enable_controller();
    delay_ms(100);

    
    wait_IRQ();
    
    uint32_t st0, cyl;
    for(int i=0; i<4; i++)
        flpydsk_check_int(&st0, &cyl);
    
    fdc_specify(0xDF, 0x02);
    fdc_configure();


}

void FDA_Init()
{   
    DMA_Init();
    FDC_reset();
    
    start_motor(0);
    delay_ms(100);

    FDC_recalibrate(0);
    printf("\nhere\n");

}

void DMA_Init()
{
    x86_outb (0x0a, 0x06);  // mask DMA channel 2 and 0 (assuming 0 is already masked)
    x86_outb (0x0c, 0xFF);  // reset the master flip-flop
    x86_outb (0x04, 0   );  // address to 0 (low byte)
    x86_outb (0x04, 0x10);  // address to 0x10 (high byte)
    x86_outb (0x0c, 0xFF);  // reset the master flip-flop (again!!!)
    x86_outb (0x05, 0xFF);  // count to 0x23ff (low byte)
    x86_outb (0x05, 0x23);  // count to 0x23ff (high byte),
    x86_outb (0x81, 0   );  // external page register to 0 for total address of 00 10 00
    x86_outb (0x0a, 0x02);  // unmask DMA channel 2
}


void prepare_DMA_write()
{
    x86_outb (0x0a, 0x06);  //mask DMA channel 2 and 0 (assuming 0 is already masked)
    x86_outb (0x0b, 0x5A);  //01011010     
    x86_outb (0x0a, 0x02);  //unmask DMA channel 2
}

void prepare_DMA_read()
{
    x86_outb (0x0a, 0x06);  //mask DMA channel 2 and 0 (assuming 0 is already masked)
    x86_outb (0x0b, 0x56);  //01010110
    x86_outb (0x0a, 0x02);  //unmask DMA channel 2
}

void flpydsk_check_int (uint32_t* st0, uint32_t* cyl) {
 
	fdc_write (8);
 
	*st0 = flpydsk_read_data ();
	*cyl = flpydsk_read_data ();
}

void fdc_specify(uint8_t srt_hut, uint8_t hlt_nd) 
{
    fdc_write(FDC_CMD_SPECIFY);     
    fdc_write(srt_hut);   
    fdc_write(hlt_nd);    
}

int fdc_write(uint8_t cmd)
{   
    uint32_t timeout = 10000;
    while (timeout--) {
        uint8_t msr = x86_inb(MAIN_STATUS_REGISTER);
        if ((msr & 0x80) && !(msr & 0x40)) { // RQM=1, DIO=0
            x86_outb(DATA_FIFO, cmd);
            return 1;
        }
    }
    printf("[FDC]: write timeout!\n");
    return 0;
}

uint8_t flpydsk_read_data () {
    
    uint32_t timeout = 10000;
    while (timeout--) {
        uint8_t msr = x86_inb(MAIN_STATUS_REGISTER);
        if ((msr & 0x80) && (msr & 0x40)) { // RQM=1, DIO=1
            return x86_inb(DATA_FIFO);
        }
    }
    printf("[FDC]: read timeout!\n");
}

void start_motor(uint8_t drive) {
    uint8_t dor_value = 0x0C;           // Enable FDC + DMA/IRQ (bits 2 and 3 set)
    dor_value |= (drive & 0x03);        // Select drive 0-3
    dor_value |= (1 << (4 + drive));    // Turn on that drive's motor
    x86_outb(DIGITAL_OUTPUT_REGISTER, dor_value);
}

void test_floppy_read() {
    printf("Attempting to read sector 0 from floppy...\n");

    FDC_read(0);
    printf("Read success!\n");

    unsigned char* dma_buffer = (unsigned char*)0x1000;
    
    for (int i = 500; i < 512; i++) 
        printf("%x ", dma_buffer[i]);

    printf("\n");
}