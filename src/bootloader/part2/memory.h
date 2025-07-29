#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include "x86.h"
#include <boot/bootparam.h>
#pragma once

void* memcpy(void* dst, const void* src, uint16_t num);
int memcmp(const void* ptr1, const void* ptr2, uint16_t num);
void* memset( void* ptr1, int c, uint64_t num);
void Memory_Detect(MemoryInfo* memoryInfo);

#endif