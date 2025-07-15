#pragma once
#include<stdint.h>

typedef struct
{
    uint16_t Limit_Low;
    uint16_t Base_Low;
    uint8_t Base_Middle;
    uint8_t Access_Byte;
    uint8_t Flags;              // Flags and limit high
    uint8_t Base_High;

}__attribute__((packed)) GDTEntry;

typedef struct 
{
    uint16_t size;
    GDTEntry *ptr;

}__attribute__((packed)) GDTDescriptor;


// Access bytes
extern const uint8_t access_code_kernel;
extern const uint8_t access_data_kernel;
extern const uint8_t access_code_user;
extern const uint8_t access_data_user;

// Flags
extern const uint8_t flags_4k_32bit;
extern const uint8_t flags_byte_32bit;
extern const uint8_t flags_4k_16bit;
extern const uint8_t flags_byte_16bit;

GDTEntry createEntry( uint32_t base, 
                      uint16_t limit, 
                      uint8_t access, 
                      uint8_t flags );