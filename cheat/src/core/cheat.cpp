#include "cheat.h"
#include "../render/menu.h"
#include "../valve/cs/cs.h"
#include "crash_handler.h"

void cheat::initialize(uintptr_t base) noexcept {
    cheat::base = base;

    dlls::initialize();
    logger::initialize(XOR("BakaWare"));
    crash_handler::initialize();
    dlls::add_to_trusted_list(base);
    memory::erase_pe_headers(base);
    interfaces::initialize();
    cheat::update_global_vars();
    render::initialize();
    input::initialize(menu::is_open);
    hooks::initialize();
    // config::initialize();

    LOG_ERROR(XOR("BakaWare initialized. Base: {} Last full build: {} {}"), (void*)base, __DATE__, __TIME__);
}

DWORD cheat::end(LPVOID instance) noexcept {
    input::unlock_cursor(false);
    hooks::end();
    logger::end();
    crash_handler::end();

    return EXIT_SUCCESS;
}

void cheat::update_global_vars() noexcept {
    SIG(globals_ptr, dlls::client, "48 89 15 ? ? ? ? 48 8D 05 ? ? ? ? 48 85")
    auto global_vars = globals_ptr.absolute<se::global_vars**>(0x3);
    if (cheat::global_vars != *global_vars) {
        cheat::global_vars = *global_vars;
        LOG_INFO(XOR("Global vars updated: {}"), (void*)cheat::global_vars);
    }
}

void cheat::local_player::update() noexcept {
    controller = cs::get_local_player_controller();
    if (!controller) {
        pawn = nullptr;
        return;
    }
    pawn = controller->m_hPawn().get();
}

void cheat::local_player::reset() noexcept {
    controller = nullptr;
    pawn = nullptr;
}
