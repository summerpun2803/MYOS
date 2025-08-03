#include "mem.h"
#include "memory.h"

#define PAGE_SIZE 4096
#define BITMAP_ENTRY_BITS 32
#define OFFSET 0x100000

static uint32_t* bitmap = 0;
static uint64_t total_blocks = 0;
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
static inline void bitmap_allocate_addr(uint64_t start, uint64_t end) {

    for(uint64_t i=start; i<=end; i++){
        bitmap_set(i);
    }
}
static inline void bitmap_deallocate_addr(uint64_t start, uint64_t end) {

    for(uint64_t i=start; i<=end; i++){
        bitmap_clear(i);
    }
}

void* alloc_block(){

    int temp = first_free();
    if(temp == -1) return 0;

    bitmap_set(temp);

    uint32_t * addr = temp * PAGE_SIZE;
    // printf("allocating : %d\n", temp);
    return (void *)addr;
}

void dealloc_block(void* addr){

    int temp = (uint32_t)addr / PAGE_SIZE;    
    // printf("deallocating : %d\n", temp);
    bitmap_clear(temp);
}

int first_free(){

    for(uint32_t i=0; i<bitmap_size_in_bytes/32; i++){

        if(bitmap[i] != 0xffffffff){

            for(int j=0; j<32; j++){
                
                int temp = 1 << j;
                if(! (bitmap[i] & temp)){
                    return i*32 + j;
                }
            }
        }
    }

    return -1;
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

    memory_base = U_base + OFFSET;
    total_blocks =( U_length - OFFSET)/ PAGE_SIZE;
    bitmap_size_in_bytes = (total_blocks + 7) / 8;

    bitmap = (uint32_t *)( memory_base );
    bitmap_deallocate_addr(0, bitmap_size_in_bytes);

    uint32_t bitmap_metadata = (bitmap_size_in_bytes + PAGE_SIZE - 1) / PAGE_SIZE;
    bitmap_allocate_addr(0, bitmap_metadata);

    printf("Bitmap Allocator Init:\n");
    printf(" Usable memory base: 0x%l\n", U_base);
    printf(" Total pages: %d\n", total_blocks);
    printf(" Bitmap size: %d bytes\n", bitmap_size_in_bytes);
    printf(" Bitmap pages used: %d KB\n", bitmap_metadata);

}
