#include <stdint.h>
#include "stdio.h"
#include "disk.h"
#include "fat.h"

void __attribute__((cdecl)) _cstart_ (uint16_t bootDrive){

    clrscr();

    printf("STAGE 2: Hello\n");

    DISK disk;

    if(!Disk_Init(&disk, bootDrive)){
        
        printf("DISK: Initailization Error\n");
        goto end;
    }
    printf("STAGE 2: Disk Initailization Done\n");

    if(!FAT_Initialize(&disk)){
        printf("FAT: Initialization Error\n");
        goto end;
    }
    printf("STAGE 2: FAT Initailization Done\n");
    
    char buffer[100];
    uint32_t read;
    FAT_File* fd = FAT_Open(&disk, "mydir/test.txt");
    while ((read = FAT_Read(&disk, fd, sizeof(buffer), buffer)))
    {
        for (uint32_t i = 0; i < read; i++)
        {
            putc(buffer[i]);
        }
    }
    FAT_Close(fd);

end:
    for (;;);
}
