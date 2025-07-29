#include "mem.h"
#include "memory.h"

#define PAGE_SIZE 4096
#define BITMAP_ENTRY_BITS 32

static uint32_t* bitmap = 0;
static uint64_t total_pages = 0;
static uint64_t bitmap_size_in_bytes = 0;
static uint64_t memory_base = 0; 


static inline void bitmap_set(uint32_t bit) {
    bitmap[bit / 32] |= (1 << (bit % 32));
}

static inline void bitmap_clear(uint32_t bit) {
    bitmap[bit / 32] &= ~(1 << (bit % 32));
}

static inline int bitmap_test(uint32_t bit) {
    return bitmap[bit / 32] & (1 << (bit % 32));
}
static inline void bitmap_allocate(uint64_t start, uint64_t end) {

    for(uint64_t i=start; i<=end; i++){
        bitmap_set(i);
    }
}
static inline void bitmap_deallocate(uint64_t start, uint64_t end) {

    for(uint64_t i=start; i<=end; i++){
        bitmap_clear(i);
    }
}


void BitMap_Init(MemoryInfo* meminfo){
    
    uint64_t U_base = meminfo->Region[0].Base;
    uint64_t U_length = meminfo->Region[0].Length;

    for(int i=0; i<meminfo->RegionCount; i++){
        if(meminfo->Region[i].Length > U_length && meminfo->Region[i].Type == 1){
            U_base = meminfo->Region[i].Base;
            U_length = meminfo->Region[i].Length;
        }
    }

    memory_base = U_base;
    total_pages =( U_length - 0x100000)/ PAGE_SIZE;
    bitmap_size_in_bytes = (total_pages + 7) / 8;

    bitmap = (uint32_t *)( memory_base  + 0x100000);
    bitmap_deallocate(0, bitmap_size_in_bytes);
    // printf("DEBUG: Small memset completed\n");
    uint32_t bitmap_metadata = (bitmap_size_in_bytes + PAGE_SIZE - 1) / PAGE_SIZE;
    bitmap_allocate(0, bitmap_metadata);

    if(bitmap_test(bitmap_metadata + 1)) {
        printf("Bit is SET (page is allocated)\n");
    } else {
        printf("Bit is CLEAR (page is free)\n");
    }

    printf("Bitmap Allocator Init:\n");
    printf(" Usable memory base: 0x%l\n", U_base);
    printf(" Total pages: %d\n", total_pages);
    printf(" Bitmap size: %d bytes\n", bitmap_size_in_bytes);
    printf(" Bitmap pages used: %d KB\n", bitmap_metadata);

}
