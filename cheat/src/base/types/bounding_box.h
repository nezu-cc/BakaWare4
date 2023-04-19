#pragma once

struct bbox {
    float x{ }, y{ }, w{ }, h{ };

    float center_x() const noexcept { return x + ((w - x) * 0.5f); }
    float center_y() const noexcept { return y + ((h - y) * 0.5f); }

    void floor() noexcept {
        x = std::floor(x);
        y = std::floor(y);
        w = std::floor(w);
        h = std::floor(h);
    }
};
