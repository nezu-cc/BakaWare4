#include "hooks.h"
#include "../memory/interfaces.h"
#include "../memory/hook_mgr.h"

void hooks::initialize() noexcept
{
    // D3DDEVICE_CREATION_PARAMETERS creation_params{ };
    // interfaces::dx9_device->GetCreationParameters(&creation_params);

    // ASSERT(creation_params.hFocusWindow);
    // game_window = creation_params.hFocusWindow;
    // original_wnd_proc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(game_window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(wnd_proc)));

    hook_func = dlls::game_overlay_renderer.find(PATTERN("48 89 5C 24 08 57 48 83 EC 30 33 C0")).cast<decltype(hook_func)>();
    unhook_func = dlls::game_overlay_renderer.find(PATTERN("E8 ? ? ? ? 0F 57 C0 0F 11 05")).absolute<decltype(unhook_func)>();


    LOG_INFO("Hooks initialized.");
}

void hooks::end() noexcept
{
    for (auto a : interfaces::hooked_tables)
        static_cast<interface_holder<void*>*>(a)->restore();

    for (auto& a : hooked_fns)
        unhook_func(a.second, false);

    // SetWindowLongPtr(game_window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(original_wnd_proc));
}
