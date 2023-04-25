#include "features.h"
#include "../entity_cache.h"

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

void render_skeleton(render::renderer* r, const entity_cache::cached_entity& cached_entity, const clr4& clr) {
    for (const auto& [start_scr, end_scr] : cached_entity.get_scr_bones()) {
        r->line(
            start_scr.x, start_scr.y,
            end_scr.x, end_scr.y,
            clr, 1
        );
    }
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
    auto weapon = player->get_active_weapon();
    if (!weapon)
        return;

    float offset = 1.f;

    if (cfg.esp.players.weapon_ammo)
        offset += render_weapon_ammo(r, bb, weapon);

    if (cfg.esp.players.weapon_name)
        render_weapon_name(r, bb, weapon, clr, true, offset);
}

void render_player_esp(render::renderer* r, const entity_cache::cached_entity& cached_entity) {
    auto controller = cached_entity.get<cs::player_controller>();
    if (!controller || controller->m_bIsLocalPlayerController() || !controller->m_bPawnIsAlive())
        return;
    
    auto player = controller->m_hPawn().get();
    if (!player)
        return;
    
    if (cheat::local && !cheat::local->is_enemy(player) && !cfg.esp.players.teammates)
        return;
    
    if (cfg.esp.players.skeleton)
        render_skeleton(r, cached_entity, clr4::white(220)); // TODO: skeleton color config

    if (cfg.esp.players.box)
        render_box(r, cached_entity.bb, clr4::white(220)); // TODO: box color config

    if (cfg.esp.players.health)
        render_health(r, cached_entity.bb, std::min(controller->m_iPawnHealth(), 100u));

    if (cfg.esp.players.name) {
        std::string name(controller->m_sSanitizedPlayerName());
        if (controller->has_flag(cs::flags::fl_fakeclient))
            name.insert(0, "BOT ");
        render_name(r, cached_entity.bb, name.c_str(), clr4::white(220)); // TODO: text color config
    }

    if (cfg.esp.players.weapon_name || cfg.esp.players.weapon_ammo)
        render_player_weapon(r, cached_entity.bb, player, clr4::white(220)); // TODO: text color config
}

void render_weapon_esp(render::renderer* r, const entity_cache::cached_entity& cached_entity) {
    auto weapon = cached_entity.get<cs::base_player_weapon>();
    if (!weapon)
        return;
    
    if (weapon->m_iState() != cs::weapon_state::WEAPON_NOT_CARRIED)
        return;

    if (cfg.esp.weapons.box)
        render_box(r, cached_entity.bb, clr4::white(220)); // TODO: box color config

    if (cfg.esp.weapons.ammo)
        render_weapon_ammo(r, cached_entity.bb, weapon);

    if (cfg.esp.weapons.name)
        render_weapon_name(r, cached_entity.bb, weapon, clr4::white(220)); // TODO: text color config
}

void features::esp::render(render::renderer* r) noexcept {
    if (!interfaces::engine->is_valid())
        return;

    std::shared_lock lock(entity_cache::mutex);
    for (const auto& [index, cached_entity] : entity_cache::entities) {
        if (!cached_entity.draw)
            continue;

        switch (cached_entity.type)
        {
        case entity_cache::entity_type::PLAYER:
            if (cfg.esp.players.enabled)
                render_player_esp(r, cached_entity);
            break;
        case entity_cache::entity_type::WEAPON:
            if (cfg.esp.weapons.enabled)
                render_weapon_esp(r, cached_entity);
            break;
        }
    }

}