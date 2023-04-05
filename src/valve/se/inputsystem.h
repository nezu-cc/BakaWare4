#pragma once

#include "../../memory/memory.h"

namespace se {

struct input_system {
public:
    void* get_sdl_window() {
        return *address(this).offset(0x2670).cast<void**>();
    }

    bool is_relative_mouse_mode() {
        return *address(this).offset(0x4F).cast<bool*>();
    }
};

}
