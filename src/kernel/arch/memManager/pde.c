#include "pde.h"

void pd_entry_add_attrib (pd_entry* e, uint32_t attrib)
{
    *e |= attrib;
}
void pd_entry_del_attrib (pd_entry* e, uint32_t attrib)
{
    *e &= ~(attrib);
}
void pd_entry_set_frame (pd_entry* e, uint32_t addr)
{
    *e = (*e & ~(I86_PDE_FRAME)) | (addr & I86_PDE_FRAME);
}
bool pd_entry_is_present (pd_entry e)
{
    return (e & I86_PDE_PRESENT) != 0;
}
bool pd_entry_is_user (pd_entry e)
{
    return (e & I86_PDE_USER) != 0;
}
bool pd_entry_is_4mb (pd_entry e)
{
    return (e & I86_PDE_4MB) != 0;
}
bool pd_entry_is_writable (pd_entry e)
{               
    return (e & I86_PDE_WRITABLE) != 0;
}
uint32_t pd_entry_pfn (pd_entry e)
{
    return e & I86_PDE_FRAME; 
}
void pd_entry_enable_global (pd_entry *e)
{
    *e |= I86_PDE_CPU_GLOBAL;
    *e |= I86_PDE_LV4_GLOBAL;
}