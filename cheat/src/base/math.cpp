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

vec3 math::angle_vector(const angle &angles) noexcept {
    vec3 forward;
    angle_vector(angles, forward);
    return forward;
}

// distance between to line segments
float math::segment_dist(vec3 start1, vec3 end1, vec3 start2, vec3 end2) noexcept {
    vec3 u = end1 - start1;
    vec3 v = end2 - start2;
    vec3 w = start1 - start2;
    float a = u.dot_product(u);
    float b = u.dot_product(v);
    float c = v.dot_product(v);
    float d = u.dot_product(w);
    float e = v.dot_product(w);
    float D = a * c - b * b;
    float sc, sN, sD = D;
    float tc, tN, tD = D;

    if (D < 0.001f) {
        sN = 0.0f;
        sD = 1.0f;
        tN = e;
        tD = c;
    } else {
        sN = (b * e - c * d);
        tN = (a * e - b * d);
        if (sN < 0.0f) {
            sN = 0.0f;
            tN = e;
            tD = c;
        } else if (sN > sD) {
            sN = sD;
            tN = e + b;
            tD = c;
        }
    }

    if (tN < 0.0f) {
        tN = 0.0f;

        if (-d < 0.0f) {
            sN = 0.0f;
        } else if (-d > a) {
            sN = sD;
        } else {
            sN = -d;
            sD = a;
        }
    } else if (tN > tD) {
        tN = tD;

        if ((-d + b) < 0.0f) {
            sN = 0;
        } else if ((-d + b) > a) {
            sN = sD;
        } else {
            sN = (-d + b);
            sD = a;
        }
    }

    sc = (std::abs(sN) < 0.001f ? 0.0f : sN / sD);
    tc = (std::abs(tN) < 0.001f ? 0.0f : tN / tD);

    vec3 dP = w + (u * sc) - (v * tc);
    return dP.length();
}

vec3 math::lerp(const vec3 &a, const vec3 &b, float t) noexcept {
    return vec3(
        std::lerp(a.x, b.x, t),
        std::lerp(a.y, b.y, t),
        std::lerp(a.z, b.z, t)
    );
}