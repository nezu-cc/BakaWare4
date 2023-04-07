#pragma once

#include "../memory/interfaces.h"
#include "../memory/hook_mgr.h"
#include "../render/render.h"
#include "config.h"

namespace cheat {

    // inline cs::local_player local{ };
    inline bool should_unhook{ };
    inline d2 screen_size{ };
    inline se::global_vars* global_vars{ };

    void initialize() noexcept;
    DWORD end(LPVOID instance) noexcept;
    void update_global_vars() noexcept;

}
