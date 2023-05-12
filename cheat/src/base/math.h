#pragma once
#include "base.h"

namespace math {
    void update_view_matrix() noexcept;
    bool world_to_screen(const vec3& world, vec2& screen, bool floor = true) noexcept;
    void angle_vector(const angle& angles, vec3& forward) noexcept;
    vec3 angle_vector(const angle &angles) noexcept;
    float segment_dist(vec3 start1, vec3 end1, vec3 start2, vec3 end2) noexcept;
    vec3 lerp(const vec3 &a, const vec3 &b, float t) noexcept;
}