#include "stdint.h"
#pragma once

int memcpy(const void far* dst, const void far* src, uint16_t num);
int memcmp(const void far* ptr1, const void far* ptr2, uint16_t num);
