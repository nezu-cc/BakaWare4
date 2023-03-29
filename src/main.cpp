#include <Windows.h>
#include "cheat/bakaware.h"

DWORD Init(LPVOID module) {
    bakaware::Setup();
    return 0;
}

// resize: `48 83 C4 30 41 5F 41 5E 5F 48 FF 25` + 0xC (relative)
// swap: `48 FF 25 ?? ?? ?? ?? 48 89 5C 24 30` + 0x3 (relative)

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        const HANDLE hThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Init, NULL, NULL, NULL);
        if (hThread)
            CloseHandle(hThread);
    }
    return TRUE;
}
