#include "mem.h"
#include "memory.h"

#define PAGE_SIZE 4096
#define BITMAP_ENTRY_BITS 32

static uint32_t* bitmap = 0;
static uint32_t total_pages = 0;
static uint32_t bitmap_size_in_bytes = 0;
static uint32_t memory_base = 0; 


static inline void bitmap_set(uint32_t bit) {
    bitmap[bit / 32] |= (1 << (bit % 32));
}

static inline void bitmap_clear(uint32_t bit) {
    bitmap[bit / 32] &= ~(1 << (bit % 32));
}

static inline int bitmap_test(uint32_t bit) {
    return bitmap[bit / 32] & (1 << (bit % 32));
}


void BitMap_Init(MemoryInfo* meminfo){
    
    uint64_t U_base = 0;
    uint64_t U_length = 0;

    for(int i=0; i<meminfo->RegionCount; i++){
        if(meminfo->Region[i].Length > U_length){
            U_base = meminfo->Region[i].Base;
            U_length = meminfo->Region[i].Length;
        }
    }
    memory_base = (uint32_t) U_base;
    total_pages = (uint32_t)U_length / PAGE_SIZE;
    bitmap_size_in_bytes = (total_pages + 7) / 8;

    bitmap = (uint32_t *) memory_base;
    memset(bitmap, 0, bitmap_size_in_bytes);
    uint32_t bitmap_metadata = (bitmap_size_in_bytes + PAGE_SIZE - 1) / PAGE_SIZE;

    for(int i=0; i<bitmap_metadata; i++){
        bitmap_set(i);
    }

    printf("Bitmap Allocator Init:\n");
    printf(" Usable memory base: 0x%l\n", U_base);
    printf(" Total pages: %d\n", total_pages);
    printf(" Bitmap size: %d bytes\n", bitmap_size_in_bytes);
    printf(" Bitmap pages used: %d\n", bitmap_metadata);

}
