#include "stdint.h"

void far* memcpy(void far* dst, const void far* src, uint16_t num)
{
    uint8_t far* Dst = (uint8_t far*) dst;
    const uint8_t far* Src = (const uint8_t far*) src;

    for(uint16_t i=0; i<num; i++){
        Dst[i] = Src[i];
    }
    return dst;
}   

int memcmp(const void far* ptr1, const void far* ptr2, uint16_t num)
{
    const uint8_t far* U8ptr1 = (const uint8_t far*)ptr1;
    const uint8_t far* U8ptr2 = (const uint8_t far*)ptr2;
    
    for(uint16_t i=0; i<num; i++){
        if(U8ptr1[i] != U8ptr2[i]) return 1;
    }
    return 0;
}

void far *memset( void far* ptr1, int c, uint16_t num){
    uint8_t far* U8ptr1 = (uint8_t far*)ptr1;

    for(uint16_t i=0; i<num; i++){
        U8ptr1[i] = (uint8_t) c;
    }

    return  ptr1;
}