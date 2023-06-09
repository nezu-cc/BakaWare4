#include "features.h"

void render_recoil_crosshair(render::renderer* r) noexcept {
    if (!cheat::local.valid())
        return;
    
    auto weapon = cheat::local->get_active_weapon();
    if (!weapon)
        return;
    
    if (weapon->m_flRecoilIndex() <= 1.1f)
        return;
    
    vec3 pos, forward_va;
    cheat::local->get_pos(&pos, &forward_va, nullptr, nullptr);

    vec2 va_screen;
    math::world_to_screen(pos + forward_va * 2000.f, va_screen);
    
    angle va, aim_punch;
    cheat::local->get_view_angles(&va);
    cheat::local->get_aim_punch(&aim_punch, true);

    vec3 forward = math::angle_vector(va + aim_punch);

    vec2 screen;
    math::world_to_screen(pos + forward * 2000.f, screen);

    if (screen.distance_to(va_screen) <= 25)
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
    
    // if (!cheat::local->has_flag(cs::flags::fl_onground))
    //     cmd->buttons &= ~se::buttons::in_jump;
}

void run_rcs(angle* va) noexcept {
    if (!cheat::local.valid() || !cheat::local.void_move_type() || !cheat::global_vars)
        return;

    auto weapon = cheat::local->get_active_weapon();
    if (!weapon)
        return;
    
    angle aim_punch;
    cheat::local->get_aim_punch(&aim_punch, true);

    static bool active = false;
    static angle old_aim_punch = aim_punch;

    if (active && aim_punch.length2d_sqr() < .01f)
        active = false;

    if (!active && cheat::local->m_iShotsFired() > 1) {
        old_aim_punch = angle{ 0.f, 0.f, 0.f };
        active = true;
    }

    if (!active) {
        old_aim_punch = angle{ 0.f, 0.f, 0.f };
        return;
    }

    angle delta = aim_punch - old_aim_punch;
    old_aim_punch = aim_punch;

    delta.x *= cfg.legit.rcs.vertical / 100.f;
    delta.y *= cfg.legit.rcs.horizontal / 100.f;

    *va -= delta;
    va->normalize();
}

void features::misc::render(render::renderer* r) noexcept {
    if (!interfaces::engine->is_valid())
        return;

    if (cfg.misc.reoil_crosshair)
        render_recoil_crosshair(r);
}

void features::misc::create_move(se::user_cmd *cmd) noexcept {
    if (cfg.misc.bunny_hop)
        run_bunny_hop(cmd);
}

void features::misc::input(angle *va, se::move_input *input, float frame_time) noexcept {
    if (cfg.legit.rcs.enabled)
        run_rcs(va);
}
