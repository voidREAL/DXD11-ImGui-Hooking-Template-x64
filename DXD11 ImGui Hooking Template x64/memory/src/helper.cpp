#include "../include/helper.h"

#include <sstream>

void* Helper::patternScan(const char* pattern, const char* moduleName)
{
	if (!pattern) return nullptr;

	HMODULE hModule = GetModuleHandleA(moduleName);
	if (!hModule) return nullptr;

	MODULEINFO modInfo{};
	if (!GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(modInfo)))
		return nullptr;

	uintptr_t moduleStart = reinterpret_cast<uintptr_t>(modInfo.lpBaseOfDll);
	uintptr_t moduleEnd = moduleStart + static_cast<uintptr_t>(modInfo.SizeOfImage);

	std::vector<int> patternArray;
	{
		std::stringstream ss(pattern);
		std::string token;
		while (ss >> token) {
			if (token == "?" || token == "??") patternArray.push_back(-1);
			else patternArray.push_back(static_cast<int>(std::stoi(token, nullptr, 16)));
		}
	}
	if (patternArray.empty()) return nullptr;

	std::string mask;
	mask.reserve(patternArray.size());
	for (int b : patternArray) mask.push_back(b == -1 ? '?' : 'x');
	const size_t patternLen = mask.length();

	uintptr_t addr = moduleStart;
	MEMORY_BASIC_INFORMATION mbi{};
	while (addr < moduleEnd) {
		SIZE_T ret = VirtualQuery(reinterpret_cast<LPCVOID>(addr), &mbi, sizeof(mbi));
		if (ret == 0) break;

		uintptr_t regionStart = reinterpret_cast<uintptr_t>(mbi.BaseAddress);
		uintptr_t regionEnd = regionStart + static_cast<uintptr_t>(mbi.RegionSize);

		if (regionEnd <= moduleStart) { addr = regionEnd; continue; }
		if (regionStart < moduleStart) regionStart = moduleStart;
		if (regionEnd > moduleEnd) regionEnd = moduleEnd;

		bool isCommitted = (mbi.State == MEM_COMMIT);
		bool hasGuard = (mbi.Protect & PAGE_GUARD);
		bool noAccess = (mbi.Protect & PAGE_NOACCESS);
		bool readable =
			isCommitted && !hasGuard && !noAccess &&
			((mbi.Protect & PAGE_READONLY) ||
				(mbi.Protect & PAGE_READWRITE) ||
				(mbi.Protect & PAGE_WRITECOPY) ||
				(mbi.Protect & PAGE_EXECUTE_READ) ||
				(mbi.Protect & PAGE_EXECUTE_READWRITE) ||
				(mbi.Protect & PAGE_EXECUTE_WRITECOPY));

		if (readable && (regionEnd - regionStart) >= patternLen) {
			const uint8_t* base = reinterpret_cast<const uint8_t*>(regionStart);
			size_t last = (regionEnd - regionStart) - patternLen;
			for (size_t i = 0; i <= last; ++i) {
				const uint8_t* p = base + i;
				bool found = true;
				for (size_t j = 0; j < patternLen; ++j) {
					if (mask[j] != '?' && static_cast<uint8_t>(patternArray[j]) != p[j]) { found = false; break; }
				}
				if (found) return const_cast<void*>(reinterpret_cast<const void*>(p));
			}
		}

		addr = reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
	}

	return nullptr;
}