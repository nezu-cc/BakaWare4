#include "features.h"
#include "../../base/math.h"
#include "../../core/cheat.h"

void features::esp::render(render::renderer* r) noexcept {
    // FIXME: other types of esp
    if (cfg.esp.players.enabled == false)
        return;

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

        const clr4 border_clr = clr4::black(128);

        if (cfg.esp.players.box) {
            r->rect(
                bb.x, bb.y, 
                bb.w, bb.h, 
                border_clr,
                3
            );
            r->rect(
                bb.x, bb.y, 
                bb.w, bb.h, 
                clr4::white(220) // TODO: box color config
            );
        }

        if (cfg.esp.players.health) {
            const uint32_t heath = std::min(controller->m_iPawnHealth(), 100u);

            // TODO: hp color config
            const clr4 top_clr = clr4::green(220);
            const clr4 bot_clr = clr4::red(220);

            const float hp = heath / 100.f;
            const clr4 hp_clr = clr4::lerp(bot_clr, top_clr, hp);
            const float hp_h = std::floor(std::lerp(bb.h, bb.y, hp));

            r->rect_filled(
                bb.x - 6, bb.y - 1, 
                bb.x - 2, bb.h + 1, 
                border_clr
            );
            r->rect_filled_multi_color(
                bb.x - 5, hp_h, 
                bb.x - 3, bb.h, 
                hp_clr, hp_clr,
                bot_clr, bot_clr
            );
        }

        if (cfg.esp.players.name) {
            const auto name = controller->m_sSanitizedPlayerName();
            if (name && strlen(name)) {
                const auto name_size = r->calc_text_size(name).x;

                r->text(
                    bb.center_x() - (name_size / 2),
                    bb.y - 16,
                    clr4::white(220), // TODO: text color config
                    name
                );
            }
        }
    }
}