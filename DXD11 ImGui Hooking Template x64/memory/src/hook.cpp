#include "../include/hook.h"
#include "../include/mem.h"

Hook hook;

bool Hook::detours32(BYTE* dst, BYTE* src, size_t size)
{
	if (size < 5) {
		return false;
	}

	DWORD currentProtect;
	if (VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &currentProtect)) {
		return false;
	}

	memset(dst, 0x90, size);
	memset(dst, 0xE9, 1);

	uintptr_t relativeAddress = dst - src - 5;

	*(uintptr_t*)(dst + 1) = relativeAddress;
	if (VirtualProtect(dst, size, currentProtect, &currentProtect)) {
		return false;
	}

	return true;
}

BYTE* Hook::trampoline32(BYTE* dst, BYTE* src, size_t size)
{
	if (size < 5) {
		return 0;
	}

	BYTE* trampoline = (BYTE*)VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	memcpy_s(trampoline, size, dst, size);

	*(trampoline + size) = 0xE9;

	uintptr_t jmpBackAddress = dst - trampoline - 5;

	*(uintptr_t*)((uintptr_t)trampoline + size + 1) = jmpBackAddress;

	detours32(dst, src, size);

	return trampoline;
}

void* Hook::trampoline64(void* dst, void* src, size_t size)
{
	if (size < 14) {
		return nullptr;
	}

	BYTE stub[14] = {
	0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	void* trampoline = VirtualAlloc(0, size + sizeof(stub), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	if (!trampoline) {
		return nullptr;
	}

	DWORD currentProtect = 0;
	VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &currentProtect);

	uintptr_t jmpBackAddress = (uintptr_t)dst + size;

	memcpy(stub + 6, &jmpBackAddress, 8);
	memcpy((void*)(trampoline), dst, size);
	memcpy((void*)((uintptr_t)trampoline + size), stub, sizeof(stub));

	memcpy(stub + 6, &src, 8);
	memcpy(dst, stub, sizeof(stub));

	for (int i = 14; i < size; i++) {
		*(BYTE*)((uintptr_t)dst + i) = 0x90;
	}

	VirtualProtect(dst, size, currentProtect, &currentProtect);
	return (void*)(trampoline);
}

bool Hook::unHook(BYTE* dst, BYTE* src, size_t size)
{
	memory.patch(dst, src, size);
	return true;
}

