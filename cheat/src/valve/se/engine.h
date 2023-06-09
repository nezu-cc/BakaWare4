#pragma once
#include "../../memory/memory.h"
#include "network_system.h"

namespace se {

struct engine_client {

    VIRTUAL_FUNCTION(is_in_game, bool, 30, (this));
    VIRTUAL_FUNCTION(is_connected, bool, 31, (this));
    VIRTUAL_FUNCTION(get_net_channel_info, net_channel_info*, 32, (this, split_screen_slot), int split_screen_slot);
    VIRTUAL_FUNCTION(get_screen_size, void, 48, (this, width, height), int* width, int* height);
    VIRTUAL_FUNCTION(get_level_name, const char*, 51, (this));
    VIRTUAL_FUNCTION(get_level_name_short, const char*, 52, (this));

    inline bool is_valid() noexcept {
        return is_in_game() && is_connected();
    }
};

}