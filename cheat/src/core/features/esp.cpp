#include "features.h"

void render_box(render::renderer* r, const bbox& bb, const clr4& clr) {
    r->rect(
        bb.x, bb.y, 
        bb.w, bb.h, 
        clr4::black(128),
        3
    );
    r->rect(
        bb.x, bb.y, 
        bb.w, bb.h, 
        clr
    );
}

void render_health(render::renderer* r, const bbox& bb, const uint32_t heath) {
    // TODO: hp color config
    const clr4 top_clr = clr4::green(220);
    const clr4 bot_clr = clr4::red(220);

    const float hp = heath / 100.f;
    const clr4 hp_clr = clr4::lerp(bot_clr, top_clr, hp);
    const float hp_h = std::floor(std::lerp(bb.h, bb.y, hp));

    r->rect_filled(
        bb.x - 6, bb.y - 1, 
        bb.x - 2, bb.h + 1, 
        clr4::black(128)
    );
    r->rect_filled_multi_color(
        bb.x - 5, hp_h, 
        bb.x - 3, bb.h, 
        hp_clr, hp_clr,
        bot_clr, bot_clr
    );
}

void render_name(render::renderer* r, const bbox& bb, const char* name, const clr4& clr) {
    if (!name || !strlen(name))
        return;

    const auto name_size = r->calc_text_size(name).x;
    r->text(
        bb.center_x() - (name_size / 2),
        bb.y - 16,
        clr,
        name
    );
}

void render_skeleton(render::renderer* r, cs::base_entity* controller, const clr4& clr) {
    constexpr std::array<std::tuple<uint8_t, uint8_t>, 15> tr = {{
        { 84, 26 },{ 26, 25 },{ 25, 0  },	// right leg
        { 82, 23 },{ 23, 22 },{ 22, 0  },	// left leg
        { 5,  92 },{ 92, 11 },{ 11, 12 },	// left arm
        { 5,  96 },{ 96, 16 },{ 16, 17 },	// right arm
        { 0,  21 },{ 21, 5 }, { 5, 6   }	// spine
    }};

    constexpr std::array<std::tuple<uint8_t, uint8_t>, 15> ct = {{
        { 97, 27 },{ 27,  26 },{ 26, 0  },	// right leg
        { 95, 24 },{ 24,  23 },{ 23, 0  },	// left leg
        { 5, 55  },{ 55,  11 },{ 11, 12 },	// left arm
        { 5, 105 },{ 105, 16 },{ 16, 17 },	// right arm
        { 0, 21  },{ 21,  5  },{ 5,  6  }	// spine
    }};

    auto game_scene_node = controller->m_pGameSceneNode();
    if(!game_scene_node)
        return;

    auto skeleton = game_scene_node->get_skeleton_instance();
    if(!skeleton)
        return;

    // FIXME: bone count check

    auto bones = controller->m_iTeamNum() == 2 ? tr : ct;

    for (auto& bone : bones) {
        cs::bone_data bone1, bone2;
        skeleton->get_bone(bone1, std::get<0>(bone));
        skeleton->get_bone(bone2, std::get<1>(bone));

        vec2 bone1_scr, bone2_scr;
        if (!math::world_to_screen(bone1.pos, bone1_scr) || !math::world_to_screen(bone2.pos, bone2_scr))
            continue;

        r->line(
            bone1_scr.x, bone1_scr.y,
            bone2_scr.x, bone2_scr.y,
            clr, 2
        );
    }
}

void features::esp::render(render::renderer* r) noexcept {
    // FIXME: other types of esp
    if (cfg.esp.players.enabled == false)
        return;

    if (!interfaces::engine->is_valid())
        return;

    for (uint32_t i = 1; i < cheat::global_vars->max_clients; i++) {
        auto controller = interfaces::entity_list->get_base_entity<cs::player_controller*>(i);
        if (!controller || controller->m_bIsLocalPlayerController() || !controller->m_bPawnIsAlive())
            continue;
        
        auto player = controller->m_hPawn().get_as<cs::player_pawn>();
        if (!player)
            continue;

        bbox bb;
        if (!player->get_bounding_box(bb))
            continue;

        if (cfg.esp.players.box)
            render_box(r, bb, clr4::white(220)); // TODO: box color config

        if (cfg.esp.players.health)
            render_health(r, bb, std::min(controller->m_iPawnHealth(), 100u));

        if (cfg.esp.players.name) {
            std::string name(controller->m_sSanitizedPlayerName());
            if (controller->has_flag(cs::flags::fl_fakeclient))
                name.insert(0, "BOT ");
            render_name(r, bb, name.c_str(), clr4::white(220)); // TODO: text color config
        }

        if (cfg.esp.players.skeleton)
            render_skeleton(r, player, clr4::white(220)); // TODO: skeleton color config
    }
}