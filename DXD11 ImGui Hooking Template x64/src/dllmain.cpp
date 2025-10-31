#include <Windows.h>
#include <iostream>

#include "../include/hooking.h"
#include "../render/include/rendercore.h"

DWORD WINAPI HackThread(HMODULE hModule) {
#ifdef _DEBUG
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
#endif

    //Init here
	Hooking::implementHooking();

	//Write your hack here
	while (!GetAsyncKeyState(VK_END)) {
	}

	//Cleanup here
	Hooking::unHook();
	Sleep(500);
	render.cleanup();
	Sleep(500);
	Hooking::freeCOM();
	//Sleep(500);
	//Hooking::freeGateway();

    Sleep(1000);
#ifdef _DEBUG
	FreeConsole();
	if (f) {
		fclose(f);
	}
#endif
	FreeLibraryAndExitThread(hModule, 0);
	CloseHandle(hModule);
	return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
		HANDLE thread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr);
		if (thread) {
			CloseHandle(thread);
		}
		break;
    }
        
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

