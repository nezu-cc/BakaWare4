#include "cheat.h"
#include "../render/menu.h"
#include "../valve/cs/cs.h"

LONG CALLBACK veh_handler(EXCEPTION_POINTERS* ExceptionInfo) {
	auto exception_code = ExceptionInfo->ExceptionRecord->ExceptionCode;
	auto exception_info0 = ExceptionInfo->ExceptionRecord->ExceptionInformation[0];
	auto exception_info1 = ExceptionInfo->ExceptionRecord->ExceptionInformation[1];

    if (exception_code == EXCEPTION_ACCESS_VIOLATION) {
        // DEP Violation
        if (exception_info0 == 8) {
            // ignore DEP violation above 0x7FFFFFFEFFFF
            if (exception_info1 >= 0x7FFFFFFEFFFF)
                return EXCEPTION_CONTINUE_EXECUTION;
        }
    }

	return EXCEPTION_CONTINUE_SEARCH;
}

void cheat::initialize(uintptr_t base) noexcept {
    dlls::initialize();
    logger::initialize(XOR("BakaWare"));
    dlls::add_to_trusted_list(base);
    memory::erase_pe_headers(base);
    interfaces::initialize();
    cheat::update_global_vars();
    render::initialize();
    input::initialize(menu::is_open);
    hooks::initialize();
    // cfg::initialize();

    // bypass DEP if mapped above 0x7FFFFFFEFFFF
    if (base >= 0x7FFFFFFEFFFF) {
        PVOID handle = AddVectoredExceptionHandler(FALSE, veh_handler);
        LOG_INFO(XOR("BEP bypass handler at: {}"), handle);
    }

    LOG_ERROR(XOR("BakaWare initialized. Base: {} Last full build: {} {}"), (void*)base, __DATE__, __TIME__);
}

DWORD cheat::end(LPVOID instance) noexcept {
    input::unlock_cursor(false);
    hooks::end();
    logger::end();

    // FreeLibraryAndExitThread(static_cast<HMODULE>(instance), EXIT_SUCCESS);
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
