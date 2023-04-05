#include "../hooks.h"
#include "../../render/menu.h"

bool __fastcall hooks::mouse_input_enabled::fn(void* rcx) {
    if (menu::is_open)
        return false;

    return original(rcx);
}