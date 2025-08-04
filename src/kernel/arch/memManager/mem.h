#ifndef BITMAP_H
#define BITMAP_H

#include <stdio.h>
#include "boot/bootparam.h"

void BitMap_Init(MemoryInfo* meminfo);
void* alloc_block();
void dealloc_block(void* addr);

#endif