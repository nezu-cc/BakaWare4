#pragma once

namespace se {

struct engine_client {

VIRTUAL_FUNCTION(get_screen_size, void, 48, (this, width, height), int* width, int* height);

};

}