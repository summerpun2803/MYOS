#include <stdint.h>

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

void* memset(void* ptr1, int c, uint32_t num) {
    uint8_t* U8ptr1 = (uint8_t*)ptr1;
    uint8_t value = (uint8_t)c;  // Cast once outside the loop
    
    for(uint32_t i = 0; i < num; i++) {
        U8ptr1[i] = value;
    }
    return ptr1;
}