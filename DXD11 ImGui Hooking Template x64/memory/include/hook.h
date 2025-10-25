#pragma once
#include <Windows.h>

class Hook {
public:
	bool detours32(BYTE* dst, BYTE* src, size_t size);
	BYTE* trampoline32(BYTE* dst, BYTE* src, size_t size);
	void* trampoline64(void* dst, void* src, size_t size);
	bool unHook(BYTE* dst, BYTE* src, size_t size);
};

extern Hook hook;