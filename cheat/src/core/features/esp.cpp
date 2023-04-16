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

float render_ammo(render::renderer* r, const bbox& bb, int cur, int max) {
    if (max <= 0)
        return 0.f;

    // TODO: ammo color config
    const clr4 max_clr = clr4::cyan();
    const clr4 min_clr = clr4::blue();

    const float ratio = static_cast<float>(cur) / max;
    const auto ammo_clr = clr4::lerp(min_clr, max_clr, ratio);
    const float ammo_w = std::floor(std::lerp(bb.x, bb.w, ratio));

    r->rect_filled(
        bb.x - 1, bb.h + 2,
        bb.w + 1, bb.h + 6,
        clr4::black(128)
    );
    r->rect_filled_multi_color(
        bb.x, bb.h + 3,
        ammo_w, bb.h + 5,
        min_clr, ammo_clr,
        ammo_clr, min_clr
    );

    return 6; // size
}

void render_name(render::renderer* r, const bbox& bb, const char* name, const clr4& clr, bool bottom = false, float offset = 1.f) {
    if (!name || !strlen(name))
        return;

    const auto name_size = r->calc_text_size(name);
    r->text(
        bb.center_x() - (name_size.x / 2),
        bottom ? bb.h + offset : bb.y - (offset + name_size.y + 2),
        clr,
        name
    );
}

bool render_skeleton(render::renderer* r, cs::base_entity* controller, const clr4& clr, bool render, bbox& bb, float padding = 0.f) {
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

    skeleton->calc_world_space_bones(cs::bone_flags::FLAG_HITBOX);

    auto& model_state = skeleton->m_modelState();
    cs::model* model = model_state.m_hModel();
    const auto num_bones = model->num_bones();
    auto bones = model_state.get_bone_data();

    std::vector<vec2> bone_scrs;
    bone_scrs.reserve(num_bones * 2);

    for (uint32_t i = 0; i < num_bones; i++) {
        if (!(model->bone_flags(i) & cs::bone_flags::FLAG_HITBOX)) {
            continue;
        }

        auto parent_index = model->bone_parent(i);
        if (parent_index == -1)
            continue;

        vec2 start_scr, end_scr;
        if (!math::world_to_screen(bones[i].pos, start_scr) || !math::world_to_screen(bones[parent_index].pos, end_scr))
            continue;

        bone_scrs.push_back(start_scr);
        bone_scrs.push_back(end_scr);

        if (render) {
            r->line(
                start_scr.x, start_scr.y,
                end_scr.x, end_scr.y,
                clr, 1
            );
        }
    }

    bb.x = bb.y = std::numeric_limits<float>::max();
    bb.w = bb.h = -std::numeric_limits<float>::max();

    if (bone_scrs.empty())
        return false;

    for (const auto& pos : bone_scrs) {
        bb.x = std::min(bb.x, pos.x);
        bb.y = std::min(bb.y, pos.y);
        bb.w = std::max(bb.w, pos.x);
        bb.h = std::max(bb.h, pos.y);
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

void render_weapon_name(render::renderer* r, const bbox& bb, cs::base_player_weapon* weapon, const clr4& clr, bool bottom = false, float offset = 1.f) {
    auto static_data = weapon->m_AttributeManager().m_Item().get_static_data();
    if (!static_data)
        return;
    
    auto weapon_name = interfaces::localize->find_safe(static_data->item_base_name);
    if (!weapon_name || !strlen(weapon_name))
        return;

    render_name(r, bb, weapon_name, clr, bottom, offset);
}

float render_weapon_ammo(render::renderer* r, const bbox& bb, cs::base_player_weapon* weapon) {
    auto v_data = weapon->get_v_data();
    if (!v_data)
        return 0.f;

    const auto ammo = weapon->m_iClip1();
    const auto max_ammo = v_data->m_iMaxClip1();

    return render_ammo(r, bb, ammo, max_ammo);
}

void render_player_weapon(render::renderer* r, const bbox& bb, cs::base_player_pawn* player, const clr4& clr) {
    cs::player_weapon_services* weapon_services = player->m_pWeaponServices();
    if (!weapon_services)
        return;
    
    auto weapon = weapon_services->m_hActiveWeapon().get();
    if (!weapon)
        return;

    float offset = 1.f;

    if (cfg.esp.players.weapon_ammo)
        offset += render_weapon_ammo(r, bb, weapon);

    if (cfg.esp.players.weapon_name)
        render_weapon_name(r, bb, weapon, clr, true, offset);
}

void features::esp::render(render::renderer* r) noexcept {
    // FIXME: other types of esp
    if (cfg.esp.players.enabled == false)
        return;

    if (!interfaces::engine->is_valid())
        return;

    for (uint32_t i = 1; i < cheat::global_vars->max_clients; i++) {
        auto controller = interfaces::entity_list->get_base_entity<cs::player_controller>(i);
        if (!controller || controller->m_bIsLocalPlayerController() || !controller->m_bPawnIsAlive())
            continue;
        
        auto player = controller->m_hPawn().get();
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

        if (cfg.esp.players.weapon_name || cfg.esp.players.weapon_ammo)
            render_player_weapon(r, bb, player, clr4::white(220)); // TODO: text color config
    }

    for (uint32_t i = cheat::global_vars->max_clients; i <= interfaces::entity_list->get_highest_entity_index(); i++) {
        auto entity = interfaces::entity_list->get_base_entity(i);
        if (!entity)
            continue;

        if (cfg.esp.weapons.enabled && entity->is_base_player_weapon()) {
            auto weapon = entity->as<cs::base_player_weapon>();

            if (weapon->m_iState() != cs::weapon_state::WEAPON_NOT_CARRIED)
                continue;
            
            bbox bb;
            if (!weapon->get_bounding_box(bb, true))
                continue;
            
            if (cfg.esp.weapons.box)
                render_box(r, bb, clr4::white(220)); // TODO: box color config
            
            if (cfg.esp.weapons.name)
                render_weapon_name(r, bb, weapon, clr4::white(220)); // TODO: text color config
            
            if (cfg.esp.weapons.ammo)
                render_weapon_ammo(r, bb, weapon);
        }
    }

}