#pragma once

#include "../base/base.h"
#include "../valve/se/inputsystem.h"
#include <array>
#include <Windows.h>
#ifdef small
#undef small
#endif

#include "../base/types/dimension.h"

namespace input {

    enum class key_state {
        up,
        down,
        toggled
    };

    enum class key_type {
        off,
        always,
        hold,
        toggle,
        release
    };

    struct keybind {
        uint64_t code{ };
        key_type type{ };

        constexpr keybind() noexcept = default;
        constexpr keybind(uint64_t code, key_type type) noexcept
            : code(code), type(type) { }
    };

    inline se::input_context* input_context{ };
    inline std::array<key_state, 255u> key_states{ };
    inline d2 mouse_pos{ };
    inline bool last_mouse_enabled{ };
    inline bool cursor_unlocked{ };

    void initialize(bool unlock = false) noexcept;
    void unlock_cursor(bool enable) noexcept;
    bool is_key_active(keybind key) noexcept;
    bool is_hovering_item(d2 item_pos, d2 item_size) noexcept;
    void process(UINT msg, WPARAM wparam, LPARAM lparam) noexcept;

}

using input::keybind;
