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

    if (!set_swap_chain(swap_chain_ptr->pSwapChain))
        return;

    ImGui::CreateContext();
    ImGui_ImplWin32_Init(game_window);
    ImGui_ImplDX11_Init(device, context);

    LOG_INFO("Render initialized.");
}

bool render::set_swap_chain(IDXGISwapChain* swap_chain) noexcept {
    render::swap_chain = swap_chain;

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    if (FAILED(swap_chain->GetDesc(&swapChainDesc))) {
        LOG_ERROR("Failed to get swap chain description.");
        return false;
    }
    game_window = swapChainDesc.OutputWindow;

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

    interfaces::engine->get_screen_size(&cheat::screen_size.x, &cheat::screen_size.y);

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
    ImGui_ImplWin32_WndProcHandler(wnd, msg, wparam, lparam);
    if (!menu::is_open)
        return false;
    
    switch (msg) {
        case WM_MOUSEMOVE:
        case WM_NCMOUSEMOVE:
        case WM_MOUSELEAVE:
        case WM_NCMOUSELEAVE:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONDBLCLK:
        case WM_XBUTTONDOWN:
        case WM_XBUTTONDBLCLK:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP:
        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL:
        case WM_KEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_SETFOCUS:
        case WM_KILLFOCUS:
        case WM_CHAR:
        case WM_DEVICECHANGE:
            return true;
        default:
            return false;
    }
}

void render::end() noexcept {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    cleanup_render_target();
}
