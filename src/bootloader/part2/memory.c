#include "stdint.h"
#include "memory.h"
#include "stdio.h"

MemoryMap G_memory_info[256];
void* memcpy(void* dst, const void* src, uint16_t num)
{
    uint8_t* Dst = (uint8_t*) dst;
    const uint8_t* Src = (const uint8_t*) src;

    for(uint16_t i=0; i<num; i++){
        Dst[i] = Src[i];
    }
    return dst;
}   

int memcmp(const void* ptr1, const void* ptr2, uint16_t num)
{
    const uint8_t* U8ptr1 = (const uint8_t*)ptr1;
    const uint8_t* U8ptr2 = (const uint8_t*)ptr2;
    
    for(uint16_t i=0; i<num; i++){
        if(U8ptr1[i] != U8ptr2[i]) return 1;
    }
    return 0;
}

void* memset( void* ptr1, int c, uint16_t num){
    uint8_t* U8ptr1 = (uint8_t*)ptr1;

    for(uint16_t i=0; i<num; i++){
        U8ptr1[i] = (uint8_t) c;
    }

    return  ptr1;
}

void Memory_Detect(MemoryInfo* memoryInfo)
{
    MemBlock block;
    uint32_t cont_id = 0;
    uint32_t cnt = 0;
    int ret = x86_MemoryMap(&block, &cont_id);
    while(ret > 0 & cont_id != 0){
        G_memory_info[cnt].Base = block.Base;
        G_memory_info[cnt].Length = block.Length;
        G_memory_info[cnt].Type = block.Type;
        G_memory_info[cnt].ACPI = block.ACPI;
        printf("Base: %d Len: %d ACPI: %d Type: %d\n", block.Base, block.Length, block.ACPI, block.Type);

        cnt++;
        ret = x86_MemoryMap(&block, &cont_id);
        
    }
    memoryInfo->Region = G_memory_info;
    memoryInfo->RegionCount = cnt;
}