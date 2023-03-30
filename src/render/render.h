#pragma once

#include <d3d11.h>
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

}
