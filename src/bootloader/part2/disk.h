#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint8_t id;
    uint16_t cylinders;
    uint16_t sectors;
    uint16_t heads;

} DISK;

bool Disk_Init(DISK* disk, uint8_t driveNumber);
bool Disk_Read_Sector(DISK* disk, uint32_t lba, uint8_t sectors, void* dataOut);
bool lba_to_chs(DISK* disk ,uint32_t lba, uint16_t* cylinderOut, uint16_t* sectorOut, uint16_t* headOut);
// bool Disk_Write_Sector();
