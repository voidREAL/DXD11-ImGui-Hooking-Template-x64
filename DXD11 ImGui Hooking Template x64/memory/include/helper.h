#pragma once
#include <Windows.h>
#include <cstdint>
#include <psapi.h>
#include <cstring>
#include <vector>

#pragma comment(lib, "Psapi.lib")

class Helper { 
public:
	void* patternScan(const char* pattern, const char* moduleName = NULL);
};