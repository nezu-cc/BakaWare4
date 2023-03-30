#include "../hooks.h"
#include "../../render/render.h"
#include "../../core/cheat.h"

HRESULT WINAPI hooks::present::fn(IDXGISwapChain *swap_chain, UINT SyncInterval, UINT Flags) {
    if (cheat::should_unhook)
        return original(swap_chain, SyncInterval, Flags);

    render::render();

    return original(swap_chain, SyncInterval, Flags);
}

HRESULT WINAPI hooks::resize_buffers::fn(IDXGISwapChain *swap_chain, UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) {
    auto ret = original(swap_chain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
    if (cheat::should_unhook)
        return ret;

    render::cleanup_render_target();

    if (SUCCEEDED(ret))
        render::set_swap_chain(swap_chain);

    return ret;
}
