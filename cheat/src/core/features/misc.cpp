#include "features.h"

void render_recoil_crosshair(render::renderer* r) noexcept {
    if (!cheat::local.valid())
        return;
    
    vec3 pos, forward_va;
    cheat::local->get_pos(&pos, &forward_va, nullptr, nullptr);

    vec2 va_screen;
    math::world_to_screen(pos + forward_va * 2000.f, va_screen);
    
    angle va, aim_punch;
    cheat::local->get_view_angles(&va);
    cheat::local->get_aim_punch(&aim_punch, true);

    vec3 forward;
    math::angle_vector(va + aim_punch, forward);

    vec2 screen;
    math::world_to_screen(pos + forward * 2000.f, screen);

    if (std::abs(screen.y - va_screen.y) <= 1)
        return;

    const auto [x, y] = screen;
    const auto size = 3.f; // TODO: size config
    const auto color = clr4::cyan(); // TODO: crosshair color config
    const auto outline = clr4::black(220);

    // outline
    r->rect(x - size - 1, y - 1, x + size + 2, y + 2, outline);
    r->rect(x - 1, y - size - 1, x + 2, y + size + 2, outline);

    // crosshair
    r->rect_filled(x - size, y, x + size + 1, y + 1, color);
    r->rect_filled(x, y - size, x + 1, y + size + 1, color);

}

void run_bunny_hop(se::user_cmd* cmd) noexcept {
    if (!cheat::local.valid() || !cheat::local.void_move_type())
        return;
    
    if (!cheat::local->has_flag(cs::flags::fl_onground))
        cmd->buttons &= ~se::buttons::in_jump;
}

void features::misc::render(render::renderer* r) noexcept {
    if (!interfaces::engine->is_valid())
        return;

    if (cfg.misc.reoil_crosshair)
        render_recoil_crosshair(r);
}

void features::misc::run(se::user_cmd *cmd) noexcept {
    if (cfg.misc.bunny_hop)
        run_bunny_hop(cmd);
}
