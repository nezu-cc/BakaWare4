#include "cheat.h"

void cheat::initialize() noexcept {
    logger::initialize(L"csgo");
    dlls::initialize();
    interfaces::initialize();
    cheat::update_global_vars();
    render::initialize();
    hooks::initialize();
    // cfg::initialize();

    LOG_SUCCESS("Cheat initialized. Last full build: {} {}", __DATE__, __TIME__);
}

DWORD cheat::end(LPVOID instance) noexcept {
    hooks::end();
    logger::end();

    // FreeLibraryAndExitThread(static_cast<HMODULE>(instance), EXIT_SUCCESS);
    return EXIT_SUCCESS;
}

void cheat::update_global_vars() noexcept {
    static auto global_vars = dlls::client.find(PATTERN("48 89 15 ? ? ? ? 48 8D 05 ? ? ? ? 48 85")).absolute<se::global_vars**>(0x3);
    if (cheat::global_vars != *global_vars) {
        cheat::global_vars = *global_vars;
        LOG_INFO("Global vars updated: {}", (void*)cheat::global_vars);
    }
}
