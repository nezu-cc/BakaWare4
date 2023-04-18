#include "hooks.h"
#include "../memory/interfaces.h"
#include "../memory/hook_mgr.h"
#include "../core/hooks.h"
#include "../render/render.h"

void hooks::initialize() noexcept
{
    if (render::game_window)
        original_wnd_proc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(render::game_window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(wnd_proc)));
    else
        LOG_ERROR(XOR("Failed to hook window proc, game window HWND missing!"));

    // hook_func = dlls::cs2.find(PATTERN("48 8B C4 48 89 58 08 48 89 70 10 57")).cast<decltype(hook_func)>();
    // unhook_func = dlls::cs2.find(PATTERN("48 85 C9 0F 84 94")).cast<decltype(unhook_func)>();

    auto present_ptr = dlls::game_overlay_renderer64.find(PATTERN("48 FF 25 ? ? ? ? 48 89 5C 24 30")).absolute<void**>(0x3);
    auto resize_buffers_ptr = dlls::game_overlay_renderer64.find(PATTERN("48 83 C4 30 41 5F 41 5E 5F 48 FF 25")).absolute<void**>(0xC);

    SET_PTR_HOOK(present_ptr, present);
    SET_PTR_HOOK(resize_buffers_ptr, resize_buffers);

    SET_VT_HOOK(interfaces::csgo_input, mouse_input_enabled, 10);
    SET_VT_HOOK(interfaces::csgo_input, create_move, 5);
    SET_VT_HOOK(interfaces::client_mode, level_init, 23);
    SET_VT_HOOK(interfaces::client_mode, level_shutdown, 24);
    SET_VT_HOOK(interfaces::view_render, on_render_start, 4);

    LOG_INFO(XOR("Hooks initialized."));
}

void hooks::end() noexcept {
    for (auto a : interfaces::hooked_tables)
        static_cast<interface_holder<void*>*>(a)->restore();

    // for (auto& a : hooked_fns)
    //     unhook_func(a.second);

    for (auto& a : hooked_ptrs)
        *a.first = a.second;

    if (original_wnd_proc && render::game_window)
        SetWindowLongPtr(render::game_window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(original_wnd_proc));
}
