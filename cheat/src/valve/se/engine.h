#pragma once

namespace se {

struct engine_client {

    VIRTUAL_FUNCTION(is_in_game, bool, 30, (this));
    VIRTUAL_FUNCTION(is_connected, bool, 31, (this));
    VIRTUAL_FUNCTION(get_screen_size, void, 48, (this, width, height), int* width, int* height);
    VIRTUAL_FUNCTION(get_level_name, const char*, 51, (this));
    VIRTUAL_FUNCTION(get_level_name_short, const char*, 52, (this));

};

}