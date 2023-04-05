#include "../hooks.h"
#include "../../core/cheat.h"

void __stdcall hooks::level_init::fn(se::client_mode* rcx, const char* newmap) {
    cheat::update_global_vars();
    original(rcx, newmap);
}