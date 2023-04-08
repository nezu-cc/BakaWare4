#include "cheat.h"

static DWORD WINAPI on_attach(LPVOID instance) noexcept
{
    cheat::initialize(instance);

#ifdef _DEBUG
    while (!cheat::should_unhook)
        std::this_thread::sleep_for(100ms);

    cheat::end(instance);
#endif
    return TRUE;
}


BOOL APIENTRY DllMain(HMODULE instance, DWORD call_reason, LPVOID)
{
    if (call_reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(instance);
#ifdef _DEBUG
        if (const auto thread = CreateThread(nullptr, 0, on_attach, instance, 0, nullptr))
            CloseHandle(thread);
#else
        on_attach(instance);
#endif
    }

    return TRUE;
}
