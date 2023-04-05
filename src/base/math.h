#pragma once
#include "base.h"

namespace math {
    bool world_to_screen(const vec3& world, vec2& screen) noexcept;
}