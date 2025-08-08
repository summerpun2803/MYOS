#include "vmm.h"
#include "mem.h"
#include "memory.h"

pdirectory *current_pd_dir = 0;

static inline pt_entry* get_ptable_entry(ptable *pt, uint32_t v_addr)
{   
    if(pt) 
        return &pt->entries[PAGE_TABLE_INDEX(v_addr)];
    return 0;
}

static inline pd_entry* get_pdir_entry(pdirectory* pd, uint32_t v_addr)
{
    if(pd)
        return &pd->entries[PAGE_DIR_INDEX(v_addr)];
    return 0;
}

pt_entry *get_page(uint32_t virt)
{
    pdirectory *pd = current_pd_dir;

    pd_entry* entry = &pd->entries[PAGE_DIR_INDEX(virt)];
    ptable* table = (ptable *) PAGE_GET_PHYSICAL_ADDRESS(entry);

    pt_entry* page = &table->entries[PAGE_TABLE_INDEX(virt)];
    return page;
}

void *alloc_page(pt_entry *page)
{
    void *block = alloc_block();
    if(block){
        pt_entry_set_frame(page, (uint32_t) block);
        pt_entry_add_attrib(page, I86_PTE_PRESENT);
    }
    return block;
}
void free_page(pt_entry *page)
{
    void *addr = (void *)PAGE_GET_PHYSICAL_ADDRESS(page);
    if(addr) dealloc_block(addr);

    pt_entry_del_attrib(page, I86_PTE_PRESENT);
}

bool switch_page_directory (pdirectory* directory) {
 
	if (!directory)
		return false;
 
	current_pd_dir = directory;
	loadPageDirectory ((uint32_t)directory);
	return true;
}


 
pdirectory* get_directory () {
 
	return current_pd_dir;
}

bool vmmngr_map_page (void* phys, void* virt) 
{
    pdirectory *pageDir = current_pd_dir;
    pd_entry* entry = &pageDir->entries [PAGE_DIR_INDEX ((uint32_t) virt) ];
    
    if((*entry & I86_PTE_PRESENT) != I86_PTE_PRESENT){
        printf("entry not present \n");
        ptable* table = (ptable *)alloc_block();
        if(!table){
            printf("[MAP]: ERROR IN ALLOCATION");
            return false;
        }
        memset(table, 0, sizeof(ptable));
        pd_entry_add_attrib(entry, I86_PDE_PRESENT | I86_PDE_WRITABLE);
        pd_entry_set_frame(entry, (uint32_t)table);
    }

    ptable* table = (ptable *) PAGE_GET_PHYSICAL_ADDRESS (entry);
    pt_entry* page = &table->entries [ PAGE_TABLE_INDEX ( (uint32_t) virt) ];
    
    pt_entry_add_attrib(page, I86_PTE_PRESENT);
    pt_entry_add_attrib(page ,I86_PTE_WRITABLE);
    pt_entry_set_frame(page, (uint32_t) phys);
    flush_tlb_entry((uint32_t) virt);
    printf("Done\n");
}
void vmmngr_unmap_page(void* virt)
{
    pt_entry *page = get_page((uint32_t) virt);
    pt_entry_set_frame(page, 0);
    pt_entry_del_attrib(page, I86_PTE_PRESENT);
}

bool fault_handler(void* virt){
    printf("in the handler \n");
    void* phys = (void *) alloc_block();
    return vmmngr_map_page(phys, virt);
}

static inline set_page(ptable* table, uint32_t v_addr, pt_entry *page)
{
    table->entries[PAGE_TABLE_INDEX(v_addr)] = *page;
}

void vmmngr_initialize() 
{   
    pdirectory *dir = (pdirectory*) alloc_block();
    memset(dir, 0, sizeof(pdirectory));

    for(uint32_t i=0; i<1024; i++) 
        dir->entries[i] = 0x02;

    ptable* first_table = (ptable *) alloc_block();
    memset(first_table, 0, sizeof(ptable));

    ptable* second_table = (ptable *) alloc_block();
    memset(second_table, 0, sizeof(ptable));

    for(int i=0, p=0x0, v=0x0; i<1024; i++, p += PAGE_SIZE, v+= PAGE_SIZE){
        pt_entry page = 0;
        pt_entry_add_attrib(&page, I86_PTE_PRESENT);
        pt_entry_add_attrib(&page, I86_PTE_WRITABLE);
        pt_entry_set_frame(&page, p);
        
        second_table->entries[PAGE_TABLE_INDEX(v)] = page;
    }

    for (int i=0, frame=0x100000, virt=0xC0000000; i<1024; i++, frame+=4096, virt+=4096) {
        pt_entry page = 0;
        pt_entry_add_attrib(&page, I86_PTE_PRESENT | I86_PTE_WRITABLE);
        pt_entry_set_frame(&page, frame);
        first_table->entries[PAGE_TABLE_INDEX(virt)] = page;
    }

    pd_entry* entry2 = &dir->entries[PAGE_DIR_INDEX(0xC0000000)];
    pd_entry_add_attrib(entry2, I86_PDE_PRESENT | I86_PDE_WRITABLE);
    pd_entry_set_frame(entry2, (uint32_t)first_table);

    pd_entry* entry1 = &dir->entries[PAGE_DIR_INDEX (0x00000000)];
    pd_entry_add_attrib(entry1, I86_PDE_PRESENT | I86_PDE_WRITABLE);
    pd_entry_set_frame(entry1, (uint32_t)second_table);

    // pd_entry* recursive_entry = &global_dir->entries[1023];
    // pd_entry_set_frame(recursive_entry, (uint32_t)global_dir);
    // pd_entry_add_attrib(recursive_entry, I86_PDE_PRESENT | I86_PDE_WRITABLE);
    

    switch_page_directory(dir);
    enablePaging();
    printf("[VMM] : Initialized\n");
}
void trigger_page_fault() {
    printf("Attempting to write to unmapped address 0x500000...\n");
    // Your vmmngr_initialize() only mapped 0x0-0x400000 and 0xC0000000+.
    // Therefore, the address 0x500000 is guaranteed to be unmapped.
    uint32_t* ptr = (uint32_t*)0x500000;
    // void *phys = (void *) alloc_block();
    // vmmngr_map_page(phys, 0x500000);
    // This is the instruction that will cause the fault.
    *ptr = 0xDEADBEEF;

    // This line should NEVER be reached.
    printf("Error: Page fault was not caught!\n");
}

