#include "../hooks.h"
#include "../../render/menu.h"
#include "../../core/cheat.h"

bool __fastcall hooks::mouse_input_enabled::fn(se::csgo_input* rcx) {
    if (menu::is_open)
        return false;

    return original(rcx);
}

bool __fastcall hooks::create_move::fn(se::csgo_input* cs_input, uint32_t split_screen_index, uint8_t a3) {
    bool ret = original(cs_input, split_screen_index, a3);

    cheat::local.update();

    return ret;
}