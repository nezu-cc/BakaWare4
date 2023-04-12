#include "../hooks.h"
#include "../../core/input.h"
#include "../../core/cheat.h"
#include "../../core/features/features.h"

bool __fastcall hooks::mouse_input_enabled::fn(se::csgo_input* rcx) {
    if (input::cursor_unlocked)
        return false;

    return original(rcx);
}

bool __fastcall hooks::create_move::fn(se::csgo_input* cs_input, uint32_t split_screen_index, uint8_t a3) {
    bool ret = original(cs_input, split_screen_index, a3);

    cheat::local.update();

    auto user_cmd = cs_input->get_user_cmd(split_screen_index);
    if (!user_cmd)
        return ret;

    features::run(user_cmd);

    return ret;
}