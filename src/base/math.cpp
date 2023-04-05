#include "math.h"

#include "../memory/dll.h"
#include "../core/cheat.h"

bool math::world_to_screen(const vec3 &world, vec2 &screen) noexcept {
    static bool(__thiscall* function)(const vec3&, vec3&) = dlls::client.find(PATTERN("E8 ? ? ? ? F3 0F 10 45 ? 8B D0")).absolute<decltype(function)>();
    vec3 screen_ret;
    if (function(world, screen_ret))
        return false;
    
    screen.x = (1.0f + screen_ret.x) * (cheat::screen_size.x * 0.5f);
    screen.y = (1.0f - screen_ret.y) * (cheat::screen_size.y * 0.5f);
    return true;
}
