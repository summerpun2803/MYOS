#include "disk.h"
#include "x86.h"
#include "stdio.h"

bool lba_to_chs(DISK *disk, uint32_t lba, uint16_t* cylinderOut, uint16_t* sectorOut, uint16_t* headOut){

    if (!disk || !cylinderOut || !sectorOut || !headOut || disk->sectors == 0 || disk->heads == 0)
        return false;

    *sectorOut = (lba % disk->sectors) + 1;
    *headOut = (lba/ disk->sectors) % disk->heads;
    *cylinderOut = (lba/ disk->sectors) / disk->heads;
    
    return true;
}

bool Disk_Init(DISK* disk, uint8_t driveNumber){
    uint8_t driveType;
    uint16_t cylinders, sectors, heads;
    // x86_Disk_Param(0, &driveType, &cylinders, &sectors, &heads);
    if(!x86_Disk_Param(0, &driveType, &cylinders, &sectors, &heads)){
        printf("here\n");
        return false;
    }

    disk->id =  driveNumber;
    disk->cylinders = cylinders + 1;
    disk->heads = heads + 1;
    disk->sectors = sectors;
    printf("Drive ID: %d\n", disk->id);
    printf("Drive Type: %d\n", driveType);
    printf("Cylinders: %d\n", disk->cylinders);
    printf("Heads: %d\n", disk->heads);
    printf("Sectors: %d\n", disk->sectors);


    return true;
}

bool Disk_Read_Sector(DISK* disk, uint32_t lba, uint8_t sectors, void* dataOut){

    uint16_t cylinder, head, sector;
    if(!lba_to_chs(disk, lba, &cylinder, &sector, &head)){
        printf("Error: LBA 2 CHS \n");
    }
    printf("LBA: %d\n" , lba);
    printf("CHS: C=%d H=%d S=%d\n", cylinder, head, sector);


    for (int i = 0; i < 3; i++)
    {
        if (x86_Disk_Read(disk->id, cylinder, sector, head, sectors, dataOut))
            return true;

        x86_Disk_Reset(disk->id);
    }
    return false;
}

