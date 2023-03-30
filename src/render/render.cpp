#include "render.h"
#include "../base/base.h"
#include "../memory/memory.h"
#include "../core/cheat.h"
#include "menu.h"
#include <imgui.h>
#include <backends/imgui_impl_dx11.h>
#include <backends/imgui_impl_win32.h>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void render::initialize() noexcept {
    auto swap_chain_ptr = **dlls::render_system_dx11.find(PATTERN("66 0F 7F 05 ? ? ? ? 66 0F 7F 0D ? ? ? ? 48 89 35")).absolute<CSwapChainDx11***>(0x4);
    auto hwnd_ptr = *dlls::render_system_dx11.find(PATTERN("48 89 3D ? ? ? ? FF 15 ? ? ? ? 89 3D")).absolute<PBYTE*>(0x3);

    game_window = *reinterpret_cast<HWND*>(hwnd_ptr + 0x250);

    if (!set_swap_chain(swap_chain_ptr->pSwapChain))
        return;

    ImGui::CreateContext();
    ImGui_ImplWin32_Init(game_window);
    ImGui_ImplDX11_Init(device, context);

    LOG_INFO("Render initialized.");
}

bool render::set_swap_chain(IDXGISwapChain* swap_chain) noexcept {
    render::swap_chain = swap_chain;

    if (FAILED(swap_chain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<PVOID*>(&device)))) {
        LOG_ERROR("Failed to get device from swap chain.");
        return false;
    }

    device->GetImmediateContext(&context);

    ID3D11Texture2D* back_buffer;
    if (FAILED(swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<PVOID*>(&back_buffer)))) {
        LOG_ERROR("Failed to get buffer from swap chain.");
        return false;
    }

    D3D11_RENDER_TARGET_VIEW_DESC desc;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;

    if (FAILED(device->CreateRenderTargetView(back_buffer, &desc, &render_target_view))) {
        back_buffer->Release();
        LOG_ERROR("Failed to create render target view.");
        return false;
    }

    back_buffer->Release();
    return true;
}

void render::cleanup_render_target() noexcept {
    if (render_target_view) {
        render_target_view->Release();
        render_target_view = nullptr;
    }
}

void render::render() noexcept {
    if (!render_target_view)
        return;

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    menu::render();

    ImGui::Render();
    context->OMSetRenderTargets(1, &render_target_view, nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

bool render::input(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept {
    if (!menu::is_open)
        return false;
    
    bool ret = ImGui_ImplWin32_WndProcHandler(wnd, msg, wparam, lparam);
    return !ret;

    // || msg == WM_MOUSEMOVE || msg == WM_NCHITTEST || msg == WM_GETDLGCODE

    // bool want_kbd_state = ImGui::GetCurrentContext() && ImGui::GetIO().WantTextInput;
    // static bool last_want_kbd_state = want_kbd_state;

    // // FIXME: InputSystem
    // // I::InputSystem->enableInput(!want_kbd_state);
    // // if (want_kbd_state != last_want_kbd_state)
    // //     I::InputSystem->resetInputState();
    // last_want_kbd_state = want_kbd_state;

    // return ret && !((msg >= WM_KEYFIRST && msg <= WM_KEYLAST) && !ImGui::GetIO().WantCaptureKeyboard);
}

void render::end() noexcept {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    cleanup_render_target();
}
