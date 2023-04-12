#include "../hooks.h"
#include "../cheat.h"
#include "../../core/input.h"
#include "../../render/menu.h"

LRESULT CALLBACK hooks::wnd_proc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    input::process(msg, wparam, lparam);

    if (input::is_key_active({ VK_INSERT, input::key_type::toggle })) {
        menu::toggle();
    }

    if (render::input(wnd, msg, wparam, lparam))
        return 1;

    return CallWindowProcA(original_wnd_proc, wnd, msg, wparam, lparam);
}
