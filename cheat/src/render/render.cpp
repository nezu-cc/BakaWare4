#include "render.h"
#include "../base/base.h"
#include "../memory/memory.h"
#include "../core/cheat.h"
#include "menu.h"
#include "../core/features/features.h"
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

    ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.03f, 0.03f, 0.03f, 1.00f);
	colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 1.00f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.29f, 0.29f, 0.29f, 0.50f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.15f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.54f, 0.54f, 0.54f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.54f, 0.54f, 0.54f, 0.40f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.41f, 1.00f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.46f, 0.51f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 0.00f, 0.62f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.00f, 0.26f, 0.65f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.00f, 0.37f, 0.94f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.00f, 0.37f, 0.94f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 0.37f, 0.94f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 0.26f, 0.65f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 0.37f, 0.94f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.00f, 0.26f, 0.65f, 1.00f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.00f, 0.37f, 0.94f, 1.00f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 0.37f, 0.94f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.00f, 0.26f, 0.65f, 1.00f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.37f, 0.94f, 1.00f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 0.37f, 0.94f, 1.00f);
	colors[ImGuiCol_Separator] = ImVec4(0.00f, 0.00f, 1.00f, 0.50f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.00f, 0.00f, 1.00f, 0.50f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.00f, 0.00f, 1.00f, 0.50f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.44f, 0.49f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.00f, 0.00f, 1.00f, 0.49f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.00f, 0.00f, 1.00f, 0.49f);
	colors[ImGuiCol_Tab] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.00f, 0.73f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.00f, 1.00f, 0.00f, 1.00f);
    colors[ImGuiCol_TableHeaderBg] = ImVec4(0.19f, 0.19f, 0.20f, 1.00f);
    colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.35f, 1.00f);
    colors[ImGuiCol_TableBorderLight] = ImVec4(0.23f, 0.23f, 0.25f, 1.00f);
    colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.00f, 0.73f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.00f, 1.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.41f, 1.00f, 0.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

	ImGuiStyle& style = ImGui::GetStyle();
	style.ChildRounding = .0f;
	style.FrameRounding = .0f;
	style.GrabRounding = .0f;
	style.PopupRounding = .0f;
	style.ScrollbarRounding = .0f;
	style.TabRounding = .0f;
	style.WindowRounding = .0f;
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.GrabMinSize = 20.f;

    LOG_INFO(XOR("Render initialized."));
}

bool render::set_swap_chain(IDXGISwapChain* swap_chain) noexcept {
    render::swap_chain = swap_chain;

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    if (FAILED(swap_chain->GetDesc(&swapChainDesc))) {
        LOG_ERROR(XOR("Failed to get swap chain description."));
        return false;
    }
    game_window = swapChainDesc.OutputWindow;

    if (FAILED(swap_chain->GetDevice(__uuidof(ID3D11Device), reinterpret_cast<PVOID*>(&device)))) {
        LOG_ERROR(XOR("Failed to get device from swap chain."));
        return false;
    }

    device->GetImmediateContext(&context);

    ID3D11Texture2D* back_buffer;
    if (FAILED(swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<PVOID*>(&back_buffer)))) {
        LOG_ERROR(XOR("Failed to get buffer from swap chain."));
        return false;
    }

    D3D11_RENDER_TARGET_VIEW_DESC desc;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;

    if (FAILED(device->CreateRenderTargetView(back_buffer, &desc, &render_target_view))) {
        back_buffer->Release();
        LOG_ERROR(XOR("Failed to create render target view."));
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
    {
        std::scoped_lock lock(input_mutex);
        ImGui::NewFrame();
    }

    auto r = render::renderer(ImGui::GetBackgroundDrawList());

    features::render(&r);

    menu::render();

    ImGui::Render();
    context->OMSetRenderTargets(1, &render_target_view, nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

bool render::input(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept {
    {
        std::scoped_lock lock(input_mutex);
        ImGui_ImplWin32_WndProcHandler(wnd, msg, wparam, lparam);
    }
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
