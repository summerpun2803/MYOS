#include "stdint.h"

void _cdecl x86_Video_WriteCharTeletype(char c, uint8_t page);

bool _cdecl x86_Disk_Param( uint8_t driveNumber,
                            uint8_t* driveTypeOut,
                            uint16_t* cylinderOut,
                            uint16_t* SectorOut,
                            uint16_t *headsOut);

bool _cdecl x86_Disk_Read(
    uint8_t drive,
    uint16_t cylinder,
    uint16_t sector,
    uint16_t head,
    uint8_t count,
    void far* bufferOut
);

bool _cdecl x86_Disk_Reset(uint8_t drive);