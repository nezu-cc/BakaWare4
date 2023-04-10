#include "features.h"

void render_recoil_crosshair(render::renderer* r) noexcept {
    // FIXME: local player
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
