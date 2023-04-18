#include "math.h"

#include "../memory/dll.h"
#include "../core/cheat.h"
#include <numbers>

static mat4x4 view_matrix;
static bool view_matrix_initialized = false;

void math::update_view_matrix() noexcept {
    SIG(world_to_projection_ptr, dlls::client, "48 8D 05 ? ? ? ? 48 8B D3 4C 8D 05")
    static auto world_to_projection = world_to_projection_ptr.absolute<mat4x4*>(3);
    view_matrix = *world_to_projection;
    view_matrix_initialized = true;
}

bool math::world_to_screen(const vec3 &world, vec2 &screen, bool floor) noexcept {
    if (!view_matrix_initialized)
        return false;

    const float z = view_matrix[3][0] * world.x +
                    view_matrix[3][1] * world.y +
                    view_matrix[3][2] * world.z +
                    view_matrix[3][3];

    if (z < 0.001f)
        return false;
    
    screen = cheat::screen_size / 2;
    screen.x *= 1.0f + (
        view_matrix[0][0] * world.x + 
        view_matrix[0][1] * world.y +
        view_matrix[0][2] * world.z + 
        view_matrix[0][3]
    ) / z;
    screen.y *= 1.0f - (
        view_matrix[1][0] * world.x + 
        view_matrix[1][1] * world.y +
        view_matrix[1][2] * world.z + 
        view_matrix[1][3]
    ) / z;

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
