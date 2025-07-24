#pragma once
#include<stdint.h>
#include<stdbool.h>

void __attribute__((cdecl)) x86_outb(uint16_t port, uint8_t value);
uint8_t __attribute__((cdecl)) x86_inb(uint16_t port);
void __attribute__((cdecl)) x86_Rmode_putc(char c);

bool __attribute__((cdecl)) x86_Disk_Param( uint8_t driveNumber,
                                            uint8_t* driveTypeOut,
                                            uint16_t* cylinderOut,
                                            uint16_t* SectorOut,
                                            uint16_t *headsOut);

bool __attribute__((cdecl)) x86_Disk_Reset(uint8_t drive);

bool __attribute__((cdecl)) x86_Disk_Read(  uint8_t drive,
                                            uint16_t cylinder,
                                            uint16_t sector,
                                            uint16_t head,
                                            uint8_t count,
                                            void* bufferOut);
                                            
typedef struct {
    
    uint64_t Base;
    uint64_t Length;
    uint32_t Type;
    uint32_t ACPI;

} MemBlock;

uint32_t __attribute__((cdecl)) x86_MemoryMap(  MemBlock* buffer24Bytes, 
                                                uint32_t* continuationEbxbuff);                                            