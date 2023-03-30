#pragma once

namespace menu {
    inline bool is_open{ true };

    inline void toggle() { is_open = !is_open; };
    void render() noexcept;
}
