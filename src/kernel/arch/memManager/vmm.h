#include <stdint.h>
#include "pte.h"
#include "pde.h"

#define PAGE_DIR_INDEX(x) ((x >> 22) & 0x3ff)
#define PAGE_TABLE_INDEX(x) ((x >> 12) & 0x3ff)
#define OFFSET(x) ((x) & ~0xfff)

#define PAGE_PER_TABLE 1024
#define TABLE_PER_DIR  1024
#define PAGE_SIZE 4096

typedef struct 
{
    pt_entry entries[PAGE_PER_TABLE];  

}__attribute__((aligned(4096))) ptable;

typedef struct 
{
   pd_entry entries[TABLE_PER_DIR];

} __attribute__((aligned(4096))) pdirectory;

bool vmmngr_alloc_page (pt_entry* e);
void vmmngr_free_page (pt_entry* e);
void vmmngr_initialize();
void __attribute__((cdecl)) enablePaging();
void __attribute__((cdecl)) loadPageDirectory(uint32_t physical_addr);
void __attribute__((cdecl)) flush_tlb_entry(uint32_t vitual_addr);
void trigger_page_fault();
void fault_handler(void* virt);