#include "entity.h"

#include "../../memory/interfaces.h"
#include "../../base/math.h"

#include <limits>
#include "../../core/input.h"

cs::base_entity *cs::handle::get() noexcept {
    return interfaces::entity_list->get_base_entity(get_entry_index());
}

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

        out.x = std::floor(std::min(out.x, screen.x));
        out.y = std::floor(std::min(out.y, screen.y));
        out.w = std::floor(std::max(out.w, screen.x));
        out.h = std::floor(std::max(out.h, screen.y));
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