#include "../hooks.h"
#include "../../render/menu.h"

void* __stdcall hooks::relative_mouse_mode::fn(se::input_system* rcx, bool enable) {
    hooks::last_mouse_relative_mode = enable;

    if (menu::is_open)
        return original(rcx, false);

    return original(rcx, enable);
}