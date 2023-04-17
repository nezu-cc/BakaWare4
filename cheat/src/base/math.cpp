#include "math.h"

#include "../memory/dll.h"
#include "../core/cheat.h"
#include <numbers>

bool math::world_to_screen(const vec3 &world, vec2 &screen, bool floor) noexcept {
    SIG(function_ptr, dlls::client, "E8 ? ? ? ? F3 0F 10 45 ? 8B D0")
    auto function = function_ptr.absolute<bool(__thiscall*)(const vec3&, vec3&)>();
    vec3 screen_ret;
    if (function(world, screen_ret))
        return false;
    
    screen.x = (1.0f + screen_ret.x) * (cheat::screen_size.x * 0.5f);
    screen.y = (1.0f - screen_ret.y) * (cheat::screen_size.y * 0.5f);

    if (floor) {
        screen.x = std::floor(screen.x);
        screen.y = std::floor(screen.y);
    }

    return true;
}

void math::angle_vector(const angle &angles, vec3 &forward) noexcept {
    const float x = angles.x * std::numbers::pi_v<float> / 180.f;
    const float y = angles.y * std::numbers::pi_v<float> / 180.f;
    const float sp = std::sin(x);
    const float cp = std::cos(x);
    const float sy = std::sin(y);
    const float cy = std::cos(y);
    forward.x = cp * cy;
    forward.y = cp * sy;
    forward.z = -sp;
}
