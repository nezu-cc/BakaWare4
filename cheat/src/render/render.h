#pragma once

#include <d3d11.h>
#include <imgui.h>
#include "../memory/memory.h"

namespace render {

class CSwapChainDx11 {
public:
	PAD(0x178)
	IDXGISwapChain* pSwapChain;
};

inline HWND                    game_window{ };
inline IDXGISwapChain*         swap_chain{ };
inline ID3D11Device*           device{ };
inline ID3D11RenderTargetView* render_target_view{ };
inline ID3D11DeviceContext*    context{ };

void initialize() noexcept;
bool set_swap_chain(IDXGISwapChain* swap_chain) noexcept;
void cleanup_render_target() noexcept;
void render() noexcept;
bool input(HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam) noexcept;
void end() noexcept;

class renderer {
public:
    renderer(ImDrawList* draw_list) noexcept : draw_list(draw_list) {}

    void rect(float x1, float y1, float x2, float y2, clr4 color, float t = 1.0f, float r = 0.0f) {
        draw_list->AddRect(ImVec2(x1, y1), ImVec2(x2, y2), color.rgba, r, ImDrawFlags_RoundCornersAll, t);
    }

    void rect_filled(float x1, float y1, float x2, float y2, clr4 color, float r = 0.0f) {
        draw_list->AddRectFilled(ImVec2(x1, y1), ImVec2(x2, y2), color.rgba, r, ImDrawFlags_RoundCornersAll);
    }

    void rect_filled_multi_color(float x1, float y1, float x2, float y2, clr4 upr_left, clr4 upr_right, clr4 btm_right, clr4 btm_left) {
        draw_list->AddRectFilledMultiColor(ImVec2(x1, y1), ImVec2(x2, y2), upr_left.rgba, upr_right.rgba, btm_right.rgba, btm_left.rgba);
    }

    vec2 calc_text_size(const char* text) {
        return ImGui::CalcTextSize(text);
    }

    void text(float x, float y, clr4 color, const char* text, bool outline = true) {
        draw_list->AddText(ImVec2(x + 1, y + 1), clr4::black(color.a).rgba, text);
        draw_list->AddText(ImVec2(x, y), color.rgba, text);
    }

    void line(float x1, float y1, float x2, float y2, clr4 color, float t = 1.0f) {
        draw_list->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), color.rgba, t);
    }

private:
    ImDrawList* draw_list;
};

}
