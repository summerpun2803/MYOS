#include "gdt.h"


const uint8_t access_code_kernel = 0x9A; // P=1, DPL=0, S=1, Type=1010
const uint8_t access_data_kernel = 0x92; // P=1, DPL=0, S=1, Type=0010
const uint8_t access_code_user = 0xFA;   // P=1, DPL=3, S=1, Type=1010
const uint8_t access_data_user = 0xF2;   // P=1, DPL=3, S=1, Type=0010


const uint8_t flags_4k_32bit = 0xC0; // G=1, D=1, L=0, AVL=0
const uint8_t flags_byte_32bit = 0x40; // G=0, D=1, L=0, AVL=0
const uint8_t flags_4k_16bit = 0x80; // G=1, D=0, L=0, AVL=0
const uint8_t flags_byte_16bit = 0x00; // G=0, D=0, L=0, AVL=0


GDTEntry createGDTEntry(uint32_t base, uint16_t limit, uint8_t access, uint8_t flags)
{
    GDTEntry entry;
    entry.Limit_Low = (uint16_t)(limit & 0xFFFF);
    entry.Base_Low = (uint16_t)(base & 0xFFFF);
    entry.Base_Middle = (uint8_t)((base >> 16) & 0xFF);
    entry.Base_High = (uint8_t)((base >> 24) & 0xFF);
    entry.Flags = (uint8_t)(((limit >> 16) & 0xF) | (flags & 0xF0));
    entry.Access_Byte = access;

    return entry;
}

GDTEntry GDT_entry[3];

void init_gdt()
{
    GDT_entry[0] = createGDTEntry(0, 0, 0, 0); // null
    GDT_entry[1] = createGDTEntry(0, 0xFFFF, access_code_kernel, flags_4k_32bit); // code
    GDT_entry[2] = createGDTEntry(0, 0xFFFF, access_data_kernel, flags_4k_32bit); // data

    GDTDescriptor g_GDTDescriptor = {
        sizeof(GDT_entry) - 1, 
        GDT_entry
    };
    // printf("GDT at: %l", &g_GDTDescriptor);
    x86_GDT_Setup(&g_GDTDescriptor, 0x08, 0x10); // selector 1 and 2
}