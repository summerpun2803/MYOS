#include "stdint.h"
#include "stdio.h"
#include "disk.h"
#include "fat.h"

void _cdecl cstart_ (uint16_t bootDrive){
    DISK disk;
    printf("STAGE 2: Hello\r\n");

    if(!Disk_Init(&disk, bootDrive)){
        printf("DISK: Initailization Error");
    }
    printf("STAGE 2: Disk Initailization Done\r\n");

    if(!FAT_Initialize(&disk)){
        printf("FAT: Initialization Error \r\n");
    }
    // FAT_File far* fd = FAT_Open(&disk, "/");
    DirectoryEntry entry;
    // int i=0;
    // while (ReadFileEntry(&disk, fd, &entry) && i++ < 5){
    //     for (int i = 0; i < 11; i++)
    //         putc(entry.Name[i]);
    //     printf("\r\n");
    // }
    // FAT_Close(fd);

    char buffer[100];
    uint32_t read;
    FAT_File far* fd = FAT_Open(&disk, "mydir/test.txt");
    while ((read = FAT_Read(&disk, fd, sizeof(buffer), buffer)))
    {
        for (uint32_t i = 0; i < read; i++)
        {
            if (buffer[i] == '\n')
                putc('\r');
            putc(buffer[i]);
        }
    }
    FAT_Close(fd);
    for(;;);
}
