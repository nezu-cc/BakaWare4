#include "entity.h"

#include "../../memory/interfaces.h"
#include "../../base/math.h"

#include <limits>
#include "../../core/input.h"

cs::base_entity *cs::handle::get() noexcept {
    return interfaces::entity_list->get_base_entity(get_entry_index());
}

bool cs::base_entity::is_player_controller() {
    return false;
}

bool cs::base_entity::get_bounding_box(bbox &out) {
    collision_property* pCollision = m_pCollision();
    if (!pCollision)
        return false;

    game_scene_node* pGameSceneNode = m_pGameSceneNode();
    if (!pGameSceneNode)
        return false;

    const vec3& origin = pGameSceneNode->m_vecAbsOrigin();
    const vec3 min = pCollision->m_vecMins() + origin;
    const vec3 max = pCollision->m_vecMaxs() + origin;

    out.x = out.y = std::numeric_limits<float>::max();
    out.w = out.h = -std::numeric_limits<float>::max();

    for (size_t i = 0; i < 8; ++i) {
        const vec3 point {
            i & 1 ? max.x : min.x, 
            i & 2 ? max.y : min.y, 
            i & 4 ? max.z : min.z
        };
        vec2 screen;
        if (!math::world_to_screen(point, screen))
            return false;

        out.x = std::floor(std::min(out.x, screen.x));
        out.y = std::floor(std::min(out.y, screen.y));
        out.w = std::floor(std::max(out.w, screen.x));
        out.h = std::floor(std::max(out.h, screen.y));
    }

    return true;
}
