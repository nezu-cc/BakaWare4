#include "../hooks.h"
#include "../../core/input.h"
#include "../../core/cheat.h"
#include "../../core/features/features.h"
#include "../../core/entity_cache.h"

bool __fastcall hooks::mouse_input_enabled::fn(se::csgo_input* rcx) {
    if (input::cursor_unlocked)
        return false;

    return original(rcx);
}

void __fastcall hooks::on_input::fn(se::csgo_input* rcx, uint32_t split_screen_index, angle* viewangles, se::move_input* move, void* unk1, float frame_time) {
    original(rcx, split_screen_index, viewangles, move, unk1, frame_time);

    features::input(viewangles, move, frame_time);
}

bool __fastcall hooks::create_move::fn(se::csgo_input* cs_input, uint32_t split_screen_index, uint8_t a3) {
    bool ret = original(cs_input, split_screen_index, a3);

    cheat::local.update();

    auto user_cmd = cs_input->get_user_cmd(split_screen_index);
    if (!user_cmd)
        return ret;

    features::create_move(user_cmd);

    return ret;
}

void __fastcall hooks::on_render_start::fn(se::view_render* rcx) {
    original(rcx);
    math::update_view_matrix();
    entity_cache::update_bounding_boxes();
}

// NOTE: yes, this takes a entity_instance, but I'm lazy
void __fastcall hooks::on_add_entity::fn(se::entity_list* rcx, cs::base_entity* entity, cs::handle<cs::base_entity> handle) {
    if (entity)
        entity_cache::on_add_entity(entity, handle);

    original(rcx, entity, handle);
}

// NOTE: yes, this takes a entity_instance, but I'm lazy
void __fastcall hooks::on_remove_entity::fn(se::entity_list* rcx, cs::base_entity* entity, cs::handle<cs::base_entity> handle) {
    if (entity)
        entity_cache::on_remove_entity(entity, handle);

    original(rcx, entity, handle);
}