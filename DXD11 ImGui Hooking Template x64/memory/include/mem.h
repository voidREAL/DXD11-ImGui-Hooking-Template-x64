#pragma once
#include <Windows.h>

class MemoryControl {
public:
	bool patch(BYTE* dst, BYTE* src, size_t size);
	bool nop(BYTE* dst, size_t size);
};

extern MemoryControl memory;