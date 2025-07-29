#include "stdint.h"
#pragma once

int memcpy(const void* dst, const void* src, uint16_t num);
int memcmp(const void* ptr1, const void* ptr2, uint16_t num);
void* memset(void* ptr1, int c, uint32_t num);