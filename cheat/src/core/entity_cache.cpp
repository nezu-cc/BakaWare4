#include "entity_cache.h"
#include "../memory/interfaces.h"

entity_cache::entity_type entity_cache::get_entity_type(cs::base_entity* entity) noexcept {
    if (entity->is_base_player_controller())
        return entity_cache::entity_type::PLAYER;

    if (entity->is_base_player_weapon())
        return entity_cache::entity_type::WEAPON;

    return entity_cache::entity_type::UNKNOWN;
}

void entity_cache::initialize() noexcept {
    if (!interfaces::engine->is_in_game())
        return;
    
    int highest_index = interfaces::entity_list->get_highest_entity_index();
    for (int i = 1; i <= highest_index; i++) {
        auto entity = interfaces::entity_list->get_base_entity(i);
        if (!entity)
            continue;

        // FIXME: not ideal since we lock the mutex inside the loop
        on_add_entity(entity, entity->get_ref_handle<cs::base_entity>());
    }
}

void entity_cache::on_add_entity(cs::base_entity *entity, cs::handle<cs::base_entity> handle) noexcept {
    // https://developer.valvesoftware.com/wiki/Entity_limit#Source_2_limits
    const auto index = handle.get_index();
    if (index >= 0x4000)
        return;
    
    // only cache useful entities
    entity_type type = get_entity_type(entity);
    if (type == entity_type::UNKNOWN)
        return;

    std::unique_lock lock(mutex);
    entities.insert_or_assign(index, cached_entity(handle, type));
    
    // call callbacks
    for (auto& [callback_type, callbacks] : callbacks) {
        if (!callbacks.add || callback_type != type)
            continue;
        callbacks.add(index, handle);
    }
}

void entity_cache::on_remove_entity(cs::base_entity *entity, cs::handle<cs::base_entity> handle) noexcept {
    std::unique_lock lock(mutex);
    const auto index = handle.get_index();

    // call callbacks
    for (auto& [callback_type, callbacks] : callbacks) {
        if (!callbacks.remove)
            continue;

        if (callback_type != get_entity_type(entity))
            continue;

        callbacks.remove(index, handle);
    }

    entities.erase(index);
}

void entity_cache::update_bounding_boxes() noexcept {
    std::unique_lock lock(mutex);

    for (auto& [index, cached_entity] : entities) {
        switch (cached_entity.type)
        {
        case entity_cache::entity_type::PLAYER: {
            auto controller = cached_entity.get<cs::base_player_controller>();
            if (!controller)
                continue;
            
            auto player = controller->m_hPawn().get();
            if (!player)
                continue;

            cached_entity.draw = player->get_bounding_box(cached_entity.bb, cached_entity.scr_bones, cached_entity.visible_bone_count);
            break;
        }
        case entity_cache::entity_type::WEAPON: {
            auto weapon = cached_entity.get<cs::base_player_weapon>();
            // FIXME: broken bomb bounding box
            cached_entity.draw = weapon->get_bounding_box(cached_entity.bb, true);
            break;
        }
        default: {
            cached_entity.draw = false;
            break;
        }
        }
    }
}

void entity_cache::register_callback(entity_type type, entity_callback add, entity_callback remove) noexcept {
    if (!add && !remove)
        return;

    std::unique_lock lock(mutex);
    callbacks.insert(std::make_pair(type, entity_callbacks{ add, remove }));

    if (!add)
        return;
    
    // add existing entities
    for (auto& [index, cached_entity] : entities) {
        if (cached_entity.type == type)
            add(index, cached_entity.handle);
    }
}
