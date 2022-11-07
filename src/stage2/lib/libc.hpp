#pragma once

#include <cstdint>
#include <cstddef>

void* memset(void* src, int c, size_t bytes);

char* itoa(int32_t value, char* str, int base);
void printf(const char* fmt, ...);