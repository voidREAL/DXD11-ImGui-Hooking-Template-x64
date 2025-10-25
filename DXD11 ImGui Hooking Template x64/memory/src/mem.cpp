#include "../include/mem.h"

MemoryControl memory;

bool MemoryControl::patch(BYTE* dst, BYTE* src, size_t size)
{
	DWORD currentProtect;
	if (!VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &currentProtect)) {
		return false;
	}

	memcpy(dst, src, size);

	if (!VirtualProtect(dst, size, currentProtect, &currentProtect)) {
		return false;
	}

	return true;
}

bool MemoryControl::nop(BYTE* dst, size_t size)
{
	DWORD currentProtect;
	if (!VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &currentProtect)) {
		return false;
	}

	memset(dst, 0x90, size);

	if (!VirtualProtect(dst, size, currentProtect, &currentProtect)) {
		return false;
	}

	return true;
}

