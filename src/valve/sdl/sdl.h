#pragma once

#include "../../base/base.h"

namespace sdl {

void set_relative_mouse_mode(bool mode);
void set_window_polling_mode(void* window, bool mode);
void set_mouse_warp_position(void* window, int32_t x, int32_t y);
    
}
