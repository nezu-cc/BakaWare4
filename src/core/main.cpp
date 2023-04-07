#include "cheat.h"

static DWORD WINAPI on_attach(LPVOID instance) noexcept
{
    cheat::initialize();

    while (!cheat::should_unhook)
        std::this_thread::sleep_for(100ms);

    return cheat::end(instance);
}


BOOL APIENTRY DllMain(HMODULE instance, DWORD call_reason, LPVOID)
{
    if (call_reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(instance);
        if (const auto thread = CreateThread(nullptr, 0, on_attach, instance, 0, nullptr))
            CloseHandle(thread);
    }

    return TRUE;
}
