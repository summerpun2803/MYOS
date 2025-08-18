#include "fs.h"
#include "arch/diskDriver/fdc.h"

#define BUFFER  0x1000

void test_floppy_read() {
    printf("Attempting to read sector 19 from floppy...\n");

    FDC_read(19);
    printf("Read success!\n");

    DirectoryEntry* dma_buffer = (DirectoryEntry *)BUFFER;
    
    for (int i = 0; i < 10; i++) 
        printf("%s ", (dma_buffer[i].Name));

    printf("\n");
}