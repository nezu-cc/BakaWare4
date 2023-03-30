#include <windowsx.h>

#include "input.h"
#include "../memory/interfaces.h"

bool input::is_key_active(keybind key) noexcept
{
    switch (key.type) {
    case key_type::always:
        return true;
    case key_type::hold:
        return key_states[key.code] == key_state::down ||
               key_states[key.code] == key_state::toggled;
    case key_type::release:
        return key_states[key.code] == key_state::up;
    case key_type::toggle:
        if (key_states[key.code] == key_state::toggled) {
            key_states[key.code] = key_state::up;
            return true;
        }
        return false;
    case key_type::off:
    default:
        return false;
    }
}

bool input::is_hovering_item(d2 item_pos, d2 item_size) noexcept
{
    return (mouse_pos.x >= item_pos.x && mouse_pos.y >= item_pos.y &&
        mouse_pos.x <= item_pos.x + item_size.x && mouse_pos.y <= item_pos.y + item_size.y);
}

void input::process(UINT msg, WPARAM wparam, LPARAM lparam) noexcept
{
    uint64_t key{ };
    key_state state{ };
    static key_state last_state{ };

    switch (msg) {
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        key = wparam;
        state = key_state::down;
        break;
    case WM_KEYUP:
    case WM_SYSKEYUP:
        key = wparam;
        state = key_state::up;
        break;
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_LBUTTONDBLCLK:
        key = VK_LBUTTON;
        state = msg == WM_LBUTTONUP ? key_state::up : key_state::down;
        break;
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_RBUTTONDBLCLK:
        key = VK_RBUTTON;
        state = msg == WM_RBUTTONUP ? key_state::up : key_state::down;
        break;
    case WM_MOUSEMOVE:
        mouse_pos = { GET_X_LPARAM(lparam), GET_Y_LPARAM(lparam) };
        break;
    default:
        return;
    }

    if (key_states[key] == key_state::up && state == key_state::down) {
        if (last_state == key_state::down)
            key_states[key] = key_state::down;
        else
            key_states[key] = key_state::toggled;
    }
    else
    	key_states[key] = state;

    last_state = state;
}
