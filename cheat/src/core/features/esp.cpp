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

bool render_skeleton(render::renderer* r, cs::base_entity* controller, const clr4& clr, bool render, bbox& bb, float padding = 0.f) {
    constexpr std::array<std::tuple<uint8_t, uint8_t>, 15> tr = {{
        { 84, 26 },{ 26, 25 },{ 25, 0  }, // right leg
        { 82, 23 },{ 23, 22 },{ 22, 0  }, // left leg
        { 5,  92 },{ 92, 11 },{ 11, 12 }, // left arm
        { 5,  96 },{ 96, 16 },{ 16, 17 }, // right arm
        { 0,  21 },{ 21, 5  },{ 5,  6  }  // spine
    }};

    constexpr std::array<std::tuple<uint8_t, uint8_t>, 15> ct = {{
        { 97, 27 },{ 27,  26 },{ 26, 0  }, // right leg
        { 95, 24 },{ 24,  23 },{ 23, 0  }, // left leg
        { 5, 55  },{ 55,  11 },{ 11, 12 }, // left arm
        { 5, 105 },{ 105, 16 },{ 16, 17 }, // right arm
        { 0, 21  },{ 21,  5  },{ 5,  6  }  // spine
    }};

    auto game_scene_node = controller->m_pGameSceneNode();
    if (!game_scene_node)
        return false;

    auto skeleton = game_scene_node->get_skeleton_instance();
    if (!skeleton)
        return false;

    // do this first so we don't have to check every bone if the origin is offscreen
    auto& origin = game_scene_node->m_vecAbsOrigin();
    auto origin2 = origin + vec3(0, 0, 32);
    vec2 origin_scr, origin2_scr;
    if (!math::world_to_screen(origin, origin_scr) || !math::world_to_screen(origin2, origin2_scr))
        return false;
    
    // FIXME: bone count check

    auto bones = controller->m_iTeamNum() == 2 ? tr : ct;

    size_t visible_bones = 0;
    std::array<vec2, std::tuple_size<decltype(bones)>::value * 2> bone_scrs;

    for (auto& bone : bones) {
        cs::bone_data bone1, bone2;
        skeleton->get_bone(bone1, std::get<0>(bone));
        skeleton->get_bone(bone2, std::get<1>(bone));

        vec2 bone1_scr, bone2_scr;
        if (!math::world_to_screen(bone1.pos, bone1_scr) || !math::world_to_screen(bone2.pos, bone2_scr))
            continue;

        bone_scrs[visible_bones++] = bone1_scr;
        bone_scrs[visible_bones++] = bone2_scr;

        if (render) {
            r->line(
                bone1_scr.x, bone1_scr.y,
                bone2_scr.x, bone2_scr.y,
                clr, 1
            );
        }
    }

    bb.x = bb.y = std::numeric_limits<float>::max();
    bb.w = bb.h = -std::numeric_limits<float>::max();

    if (visible_bones == 0)
        return false;

    for (size_t i = 0; i < visible_bones; i++) {
        bb.x = std::min(bb.x, bone_scrs[i].x);
        bb.y = std::min(bb.y, bone_scrs[i].y);
        bb.w = std::max(bb.w, bone_scrs[i].x);
        bb.h = std::max(bb.h, bone_scrs[i].y);
    }

    bb.x = std::floor(bb.x);
    bb.y = std::floor(bb.y);
    bb.w = std::floor(bb.w);
    bb.h = std::floor(bb.h);

    const float scale = std::abs(origin_scr.y - origin2_scr.y) / 32.f;
    bb.x -= std::floor(scale * padding);
    bb.y -= std::floor(scale * padding);
    bb.w += std::floor(scale * padding);
    bb.h += std::floor(scale * padding);

    return true;
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
        
        if (cheat::local && !cheat::local->is_enemy(player) && !cfg.esp.players.teammates)
            continue;

        bbox bb;
        if (!render_skeleton(r, player, clr4::white(220), cfg.esp.players.skeleton, bb, 8)) // TODO: skeleton color config
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
    }
}