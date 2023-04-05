#include "features.h"
#include "../../base/math.h"

void features::esp::render(render::renderer* r) noexcept {
    // TODO: config

    if (!interfaces::engine->is_in_game() || !interfaces::engine->is_connected())
        return;

    vec2 pos;
    if (math::world_to_screen(vec3(0, 0, 0), pos))
        r->rect(pos.x - 5, pos.y -5, 10, 10, clr4::red());

    r->rect(100, 100, 100, 100, clr4::red());
}