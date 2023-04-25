#include "entity.h"
#include "player.h"

#include "../../memory/interfaces.h"
#include "../../base/math.h"

#include <limits>
#include "../../core/input.h"

bool cs::base_entity::get_bounding_box(bbox &out, bool hitbox) noexcept {
    collision_property* collision = m_pCollision();
    if (!collision)
        return false;

    game_scene_node* scene_node = m_pGameSceneNode();
    if (!scene_node)
        return false;

    const se::transform node_to_world = scene_node->m_nodeToWorld();
    mat3x4 trans;
    node_to_world.to_matrix(trans);

    vec3 min, max;
    if (hitbox) {
        if (!collision->outer)
            return false;

        cs::base_animating* base_animating = collision->outer->get_base_animating();
        if (!base_animating)
            return false;
        
        se::hitbox_set* hb_set = base_animating->get_hitbox_set(0);
        if (!hb_set || hb_set->hit_boxes.size == 0)
            return false;

        ASSERT(hb_set->hit_boxes.size == 1);
        
        auto& hitbox = hb_set->hit_boxes[0];
        min = hitbox.mins;
        max = hitbox.maxs;
    } else {
        min = collision->m_vecMins();
        max = collision->m_vecMaxs();
    }

    out.x = out.y = std::numeric_limits<float>::max();
    out.w = out.h = -std::numeric_limits<float>::max();

    for (size_t i = 0; i < 8; ++i) {
        const vec3 point = vec3(
            i & 1 ? max.x : min.x, 
            i & 2 ? max.y : min.y, 
            i & 4 ? max.z : min.z
        ).transform(trans);

        vec2 screen;
        if (!math::world_to_screen(point, screen))
            return false;

        out.x = std::min(out.x, screen.x);
        out.y = std::min(out.y, screen.y);
        out.w = std::max(out.w, screen.x);
        out.h = std::max(out.h, screen.y);
    }

    const float width = out.w - out.x;
    if (width < 4.f) {
        const float half = (4.f - width) * 0.5f;
        out.x -= std::floor(half);
        out.w += std::ceil(half);
    }

    const float height = out.h - out.y;
    if (height < 4.f) {
        const float half = (4.f - height) * 0.5f;
        out.y -= std::floor(half);
        out.h += std::ceil(half);
    }

    return true;
}

bool cs::base_player_pawn::get_bounding_box(bbox &out, std::array<std::pair<vec2, vec2>, MAX_STUDIO_BONES>& bone_scrs, int& visible_bones_count) noexcept {
    visible_bones_count = 0;

    auto game_scene_node = m_pGameSceneNode();
    if (!game_scene_node)
        return false;

    auto skeleton = game_scene_node->get_skeleton_instance();
    if (!skeleton)
        return false;

    // do this first so we don't have to check every bone if the origin is offscreen
    constexpr float origin2_offset = 32.f;
    auto& origin = game_scene_node->m_vecAbsOrigin();
    auto origin2 = origin + vec3(0, 0, origin2_offset);
    vec2 origin_scr, origin2_scr;
    if (!math::world_to_screen(origin, origin_scr, false) || !math::world_to_screen(origin2, origin2_scr, false))
        return false;

    skeleton->calc_world_space_bones(cs::bone_flags::FLAG_HITBOX);

    auto& model_state = skeleton->m_modelState();
    cs::model* model = model_state.m_hModel();
    if (!model)
        return false;

    auto num_bones = model->num_bones();
    auto bone_data = model_state.get_bone_data();

    if (num_bones > MAX_STUDIO_BONES)
        num_bones = MAX_STUDIO_BONES;

    for (uint32_t i = 0; i < num_bones; i++) {
        if (!(model->bone_flags(i) & cs::bone_flags::FLAG_HITBOX)) {
            continue;
        }

        auto parent_index = model->bone_parent(i);
        if (parent_index == -1)
            continue;

        vec2 start_scr, end_scr;
        if (
            !math::world_to_screen(bone_data[i].pos, start_scr, false) ||
            !math::world_to_screen(bone_data[parent_index].pos, end_scr, false)
        ) {
            continue;
        }
        
        bone_scrs[visible_bones_count++] = std::make_pair(start_scr, end_scr);
    }

    out.x = out.y = std::numeric_limits<float>::max();
    out.w = out.h = -std::numeric_limits<float>::max();

    if (visible_bones_count == 0)
        return false;
    
    for (int i = 0; i < visible_bones_count; i++) {
        const auto& [start, end] = bone_scrs[i];

        out.x = std::min(out.x, start.x);
        out.y = std::min(out.y, start.y);
        out.w = std::max(out.w, start.x);
        out.h = std::max(out.h, start.y);

        out.x = std::min(out.x, end.x);
        out.y = std::min(out.y, end.y);
        out.w = std::max(out.w, end.x);
        out.h = std::max(out.h, end.y);
    }
    
    out.floor();

    const float scale = std::abs(origin_scr.y - origin2_scr.y) / origin2_offset;
    const float padding = std::floor(scale * 8);
    out.x -= padding;
    out.y -= padding;
    out.w += padding;
    out.h += padding;

    return true;
}

cs::entity_instance_by_class_iter::entity_instance_by_class_iter(base_entity* start_entity, const char* name) noexcept {
    entity = start_entity ? start_entity->m_pEntity() : nullptr;
    find_filter = nullptr; // haven't reversed yet, seems to always be null
    type = entity_iter_type::UNKNOWN; // haven't reversed yet, seems to always be 0
    unk1 = 0; // haven't reversed yet, hard coded to 0
    class_name = name;
    unk2 = nullptr; // haven't reversed yet, hard coded to nullptr

    // save cpu cycles in next() lmao
    if (!next_fn) {
        SIG(next_fn_ptr, dlls::client, "E8 ? ? ? ? 48 85 C0 75 2C 48 8B 0D")
        next_fn = next_fn_ptr.absolute<decltype(next_fn)>();
    }
}

cs::base_entity *cs::internal::get_entity_by_index(int index) noexcept {
    return interfaces::entity_list->get_base_entity(index);
}

cs::weapon_cs_base *cs::base_player_pawn::get_active_weapon() noexcept {
    auto weapon_services = m_pWeaponServices();
    if (!weapon_services)
        return nullptr;
    
    return weapon_services->m_hActiveWeapon().get<weapon_cs_base>();
}
