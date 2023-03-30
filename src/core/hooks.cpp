#include "hooks.h"
#include "../memory/interfaces.h"
#include "../memory/hook_mgr.h"
#include "../render/render.h"

void hooks::initialize() noexcept
{
    if (render::game_window)
        original_wnd_proc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(render::game_window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(wnd_proc)));
    else
        LOG_ERROR("Failed to hook window proc, game window HWND missing!");

    hook_func = dlls::game_overlay_renderer.find(PATTERN("48 89 5C 24 08 57 48 83 EC 30 33 C0")).cast<decltype(hook_func)>();
    unhook_func = dlls::game_overlay_renderer.find(PATTERN("E8 ? ? ? ? 0F 57 C0 0F 11 05")).absolute<decltype(unhook_func)>();

    auto present_ptr = dlls::game_overlay_renderer.find(PATTERN("48 FF 25 ? ? ? ? 48 89 5C 24 30")).absolute<void**>(0x3);
    auto resize_buffers_ptr = dlls::game_overlay_renderer.find(PATTERN("48 83 C4 30 41 5F 41 5E 5F 48 FF 25")).absolute<void**>(0xC);

    SET_PTR_HOOK(present_ptr, present);
    SET_PTR_HOOK(resize_buffers_ptr, resize_buffers);

    LOG_INFO("Hooks initialized.");
}

void hooks::end() noexcept
{
    for (auto a : interfaces::hooked_tables)
        static_cast<interface_holder<void*>*>(a)->restore();

    for (auto& a : hooked_fns)
        unhook_func(a.second, false);

    for (auto& a : hooked_ptrs)
        *a.first = a.second;

    // SetWindowLongPtr(game_window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(original_wnd_proc));
}
