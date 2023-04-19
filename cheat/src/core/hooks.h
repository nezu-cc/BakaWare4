#pragma once

#include "../base/base.h"
#include <Windows.h>
#include <d3d11.h>

#include "../valve/se/se.h"

#define DECLARE_HOOK(name, ret, base, ... /* args */) namespace name { \
using ty = ret(__thiscall*)(base* ecx, __VA_ARGS__); \
inline ty original; \
ret WINAPI fn(base* ecx, __VA_ARGS__); \
}

#define DECLARE_PROXY(name, prop_name) namespace name { \
inline cs::recv_proxy_fn original{ }; \
void proxy(cs::recv_proxy_data* data, void*, void*); \
}

namespace hooks {

    inline WNDPROC original_wnd_proc{ };

    extern LRESULT CALLBACK wnd_proc(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam);
    DECLARE_HOOK(present, HRESULT, IDXGISwapChain, UINT, UINT);
    DECLARE_HOOK(resize_buffers, HRESULT, IDXGISwapChain, UINT, UINT, UINT, DXGI_FORMAT, UINT);
    DECLARE_HOOK(mouse_input_enabled, bool, se::csgo_input);
    DECLARE_HOOK(create_move, bool, se::csgo_input, uint32_t, uint8_t);
    DECLARE_HOOK(level_init, void, se::client_mode, const char*);
    DECLARE_HOOK(level_shutdown, void, se::client_mode);
    DECLARE_HOOK(on_render_start, void, se::view_render);
    DECLARE_HOOK(on_add_entity, void, se::entity_list, cs::base_entity*, cs::handle<cs::base_entity>);
    DECLARE_HOOK(on_remove_entity, void, se::entity_list, cs::base_entity*, cs::handle<cs::base_entity>);

}

#undef DECLARE_HOOK
#undef DECLARE_PROXY
