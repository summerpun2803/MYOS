#ifndef BOOTPARAM_H
#define BOOTPARAM_H
#include <stdint.h>

typedef struct 
{
    uint64_t Base;
    uint64_t Length;
    uint32_t Type;
} __attribute__((packed)) MemoryMap;

typedef struct
{
    MemoryMap *Region;
    int RegionCount;
} MemoryInfo;

typedef struct 
{
    MemoryInfo memory;
    uint8_t BootDevice;    
}bootparam;

#endif
