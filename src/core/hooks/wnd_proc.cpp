#include "../hooks.h"
#include "../cheat.h"
#include "../../core/input.h"
#include "../../render/menu.h"
#include "../../valve/sdl/sdl.h"

LRESULT CALLBACK hooks::wnd_proc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    input::process(msg, wparam, lparam);

    if (input::is_key_active({ VK_INSERT, input::key_type::toggle })) {
        menu::toggle();
        hooks::relative_mouse_mode::original(interfaces::input_system, menu::is_open ? false : hooks::last_mouse_relative_mode);
        if (menu::is_open && hooks::last_mouse_relative_mode)
            sdl::set_mouse_warp_position(nullptr, cheat::screen_size.x / 2, cheat::screen_size.y / 2);
    }

    if (render::input(wnd, msg, wparam, lparam))
        return 1;

    return CallWindowProcA(original_wnd_proc, wnd, msg, wparam, lparam);
}
