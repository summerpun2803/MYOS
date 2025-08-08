#include "vmm.h"
#include "mem.h"
#include "memory.h"

pdirectory *global_dir = 0;

bool vmmngr_alloc_page (pt_entry* e) 
{
    void *frame = alloc_block();
    if(!frame) return false;

    pt_entry_set_frame(e, (uint32_t)frame);
    pt_entry_add_attrib(e, I86_PTE_PRESENT);

    return true;
}

void vmmngr_free_page (pt_entry* e)
{
    void *frame = (void *)pt_entry_pfn(e);

    if(frame) dealloc_block(frame);

    pt_entry_del_attrib(e, I86_PTE_PRESENT);
    return true;
}

static inline pt_entry* get_ptable_entry(ptable *pt, uint32_t v_addr)
{
    return &pt->entries[PAGE_TABLE_INDEX(v_addr)];
}

static inline pd_entry* get_pdir_entry(pdirectory* pd, uint32_t v_addr)
{
    return &pd->entries[PAGE_DIR_INDEX(v_addr)];
}

bool switch_page_directory (pdirectory* directory) {
 
	if (!directory)
		return false;
 
	global_dir = directory;
	loadPageDirectory ((uint32_t)directory);
	return true;
}


 
pdirectory* get_directory () {
 
	return global_dir;
}

void vmmngr_map_page (void* phys, void* virt) 
{
    pdirectory *pageDir = get_directory();
    pd_entry* e = get_pdir_entry(pageDir, (uint32_t)virt);
    
    if((*e & I86_PTE_PRESENT) != I86_PTE_PRESENT){
        
        ptable* table = (ptable *)alloc_block();
        if(!table){
            printf("[MAP]: ERROR IN ALLOCATION");
            return;
        }
        memset(table, 0, PAGE_SIZE);
        pd_entry_set_frame(e, (uint32_t)table);
        pd_entry_add_attrib(e, I86_PTE_PRESENT | I86_PTE_WRITABLE);

    }

    ptable* table = (ptable *) pd_entry_pfn(e);
    pt_entry* page = get_ptable_entry(table, (uint32_t)virt);

    pt_entry_set_frame(page, (uint32_t) phys);
    pt_entry_add_attrib(page, I86_PTE_PRESENT | I86_PTE_WRITABLE);

}
void fault_handler(void* virt){
    printf("in the handlier \n");
    void* phys = alloc_block();
    if (!phys) {
        printf("No physical memory!\n");
        while(1);
    }
    vmmngr_map_page(phys, virt);
}
static inline set_page(ptable* table, uint32_t v_addr, pt_entry *page)
{
    table->entries[PAGE_TABLE_INDEX(v_addr)] = *page;
}
void vmmngr_initialize() 
{   
    ptable* first_table = (ptable *) alloc_block();
    memset(first_table, 0, PAGE_SIZE);

    ptable* second_table = (ptable *) alloc_block();
    memset(second_table, 0, PAGE_SIZE);

    for(int i=0, p=0x0, v=0x00000000; i<1024; i++, p += PAGE_SIZE, v+= PAGE_SIZE){
        pt_entry page = 0;
        pt_entry_add_attrib(&page, I86_PTE_PRESENT);
        pt_entry_set_frame(&page, (uint32_t) p);
        set_page(first_table, v, &page);
    }

    for (int i=0, frame=0x100000, virt=0xC0000000; i<1024; i++, frame+=4096, virt+=4096) {
        pt_entry page = 0;
        pt_entry_add_attrib(&page, I86_PTE_PRESENT | I86_PTE_WRITABLE);
        pt_entry_set_frame(&page, frame);
        set_page(second_table, virt, &page);
    }

    global_dir = (pdirectory*) alloc_block();
    memset(global_dir, 0, 1024);

    pd_entry* entry2 = &global_dir->entries[PAGE_DIR_INDEX(0xC0000000)];
    pd_entry_set_frame(entry2, (uint32_t)second_table);
    pd_entry_add_attrib(entry2, I86_PDE_PRESENT | I86_PDE_WRITABLE);


    pd_entry* entry1 = &global_dir->entries[PAGE_DIR_INDEX (0x00000000)];
    pd_entry_set_frame(entry1, (uint32_t)first_table);
    pd_entry_add_attrib(entry1, I86_PDE_PRESENT | I86_PDE_WRITABLE);

    // pd_entry* recursive_entry = &global_dir->entries[1023];
    // pd_entry_set_frame(recursive_entry, (uint32_t)global_dir);
    // pd_entry_add_attrib(recursive_entry, I86_PDE_PRESENT | I86_PDE_WRITABLE);
    

    switch_page_directory(global_dir);
    enablePaging();
    printf("[VMM] : Initialized\n");
}
void trigger_page_fault() {
    printf("Attempting to write to unmapped address 0x500000...\n");

    // Your vmmngr_initialize() only mapped 0x0-0x400000 and 0xC0000000+.
    // Therefore, the address 0x500000 is guaranteed to be unmapped.
    uint32_t* ptr = (uint32_t*)0x500000;

    // This is the instruction that will cause the fault.
    *ptr = 0xDEADBEEF;

    // This line should NEVER be reached.
    printf("Error: Page fault was not caught!\n");
}

