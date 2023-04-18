#pragma once
#include "base.h"

namespace math {
    void update_view_matrix() noexcept;
    bool world_to_screen(const vec3& world, vec2& screen, bool floor = true) noexcept;
    void angle_vector(const angle& angles, vec3& forward) noexcept;
}