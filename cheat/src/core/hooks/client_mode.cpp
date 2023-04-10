#include "../hooks.h"
#include "../../core/cheat.h"

void __stdcall hooks::level_init::fn(se::client_mode* rcx, const char* newmap) {
    cheat::update_global_vars();
    original(rcx, newmap);
}

void __stdcall hooks::level_shutdown::fn(se::client_mode* rcx) {
    cheat::local.reset();
    original(rcx);
}