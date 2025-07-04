#include <stdint.h>
#include "stdio.h"
#include "disk.h"
#include "fat.h"
#include "memory.h"

// void *g_data = (void *)0x20000;
uint8_t *KernelLoadBufer = (uint8_t *) 0x30000;
uint8_t *KernelAddr = (uint8_t *) 0x100000;

typedef void (*KernelStart)();
void __attribute__((cdecl)) _cstart_ (uint16_t bootDrive){

    clrscr();

    printf("STAGE 2: Hello\n");

    DISK disk;

    if(!Disk_Init(&disk, bootDrive)){
        
        printf("DISK: Initailization Error\n");
        goto end;
    }
    printf("STAGE 2: Disk Initailization Done\n");

    // Disk_Read_Sector(&disk, 0, 1, g_data);

    // hexdump(g_data, 512);


    if(!FAT_Initialize(&disk)){
        printf("FAT: Initialization Error\n");
        goto end;
    }
    printf("STAGE 2: FAT Initailization Done\n");

    FAT_File* fd = FAT_Open(&disk, "/kernel.bin");
    uint32_t size;
    uint8_t *Kernel = KernelAddr;
    while((size = FAT_Read(&disk, fd, 0x0010000,KernelLoadBufer))){
        memcpy(KernelAddr, KernelLoadBufer, size);
        KernelAddr += size;
    }
    FAT_Close(fd);
    printf("STAGE 2: Moving to Kernel");

    KernelStart kernelStart = (KernelStart) Kernel;
    kernelStart();

end:
    for (;;);
}
