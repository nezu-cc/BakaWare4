#include "cheat.h"
#include "../render/menu.h"
#include "../valve/cs/cs.h"
#include "../base/crash_handler.h"
#include "entity_cache.h"
#include "features/features.h"

#include <sol/sol.hpp>

void test() noexcept {
    sol::state lua;
    lua.open_libraries(sol::lib::base);
    lua.open_libraries(sol::lib::string);
    lua.open_libraries(sol::lib::math);
    lua.open_libraries(sol::lib::bit32);
    lua.open_libraries(sol::lib::ffi);
    lua.open_libraries(sol::lib::jit);
    lua.set_function("test", [](std::string message) {
        LOG_INFO(XOR("Hello from lua: {}"), message);
    });
    const auto res = lua.script("test(type(jit))");
    for (auto &&i : res)
    {
        if (i.is<std::string>())
            LOG_INFO(XOR("Lua result: {}, s: {}"), (int)res.status(), i.as<std::string>());
        else
            LOG_INFO(XOR("Lua result: {}, {}"), (int)res.status(), (int)i.get_type());
    }
}


void cheat::initialize(uintptr_t base) noexcept {
    cheat::base = base;

    dlls::initialize();
    logger::initialize(XOR("BakaWare"));
    crash_handler::initialize(base);
    dlls::add_to_trusted_list(base);
    memory::erase_pe_headers(base);
    interfaces::initialize();
    cheat::update_global_vars();
    render::initialize();
    input::initialize(menu::is_open);
    hooks::initialize();
    entity_cache::initialize();
    features::initialize();
    // config::initialize();

    LOG_ERROR(XOR("BakaWare initialized. Base: {} Last full build: {} {}"), (void*)base, __DATE__, __TIME__);
    test();
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
