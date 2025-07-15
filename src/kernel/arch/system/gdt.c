#include "gdt.h"
#include "x86.h"


const uint8_t access_code_kernel = 0x9A; // P=1, DPL=0, S=1, Type=1010
const uint8_t access_data_kernel = 0x92; // P=1, DPL=0, S=1, Type=0010
const uint8_t access_code_user = 0xFA;   // P=1, DPL=3, S=1, Type=1010
const uint8_t access_data_user = 0xF2;   // P=1, DPL=3, S=1, Type=0010


const uint8_t flags_4k_32bit = 0xC0; // G=1, D=1, L=0, AVL=0
const uint8_t flags_byte_32bit = 0x40; // G=0, D=1, L=0, AVL=0
const uint8_t flags_4k_16bit = 0x80; // G=1, D=0, L=0, AVL=0
const uint8_t flags_byte_16bit = 0x00; // G=0, D=0, L=0, AVL=0


GDTEntry createEntry(uint32_t base, uint16_t limit, uint8_t access, uint8_t flags)
{
    GDTEntry entry;
    entry.Limit_Low = (uint16_t)(limit & 0xFFFF);
    entry.Base_Low = (uint16_t)(base & 0xFFFF);
    entry.Base_Middle = (uint8_t)((base >> 16) & 0xFF);
    entry.Base_High = (uint8_t)((base >> 24) & 0xFF);
    entry.Flags = (uint8_t)(((limit >> 16) & 0x0F) | (flags & 0xF0));
    entry.Access_Byte = (uint8_t)(access & 0xFF);

    return entry;
}

GDTEntry GDT_entry[3];

void init_gdt()
{
    GDT_entry[0] = createEntry(0,0,0,0);
    GDT_entry[1] = createEntry(0, 0xFFFF, access_code_kernel, flags_4k_32bit);
    GDT_entry[2] = createEntry(0, 0xFFFF, access_data_kernel, flags_4k_32bit);

    GDTDescriptor g_GDTDescriptor = {sizeof(GDTDescriptor) - 1, GDT_entry};

    x86_GDT_Setup(g_GDTDescriptor, 0x08, 0x10);
}