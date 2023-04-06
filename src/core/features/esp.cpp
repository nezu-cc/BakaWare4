#include "features.h"
#include "../../base/math.h"
#include "../../core/cheat.h"

void features::esp::render(render::renderer* r) noexcept {
    // TODO: config

    if (!interfaces::engine->is_in_game() || !interfaces::engine->is_connected())
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
        r->rect(bb.x, bb.y, bb.w - bb.x, bb.h - bb.y, clr4::red());
    }
}