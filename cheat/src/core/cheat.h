#pragma once

#include "../memory/interfaces.h"
#include "../memory/hook_mgr.h"
#include "../render/render.h"
#include "config.h"

namespace cheat {

    struct local_player {
        cs::player_controller* controller;
        cs::player_pawn* pawn;

        auto operator->() noexcept { return pawn; }
        operator bool() noexcept { return pawn; }

        void update() noexcept;
        void reset() noexcept;
    };

    inline local_player local{ };
    inline bool should_unhook{ };
    inline d2 screen_size{ };
    inline se::global_vars* global_vars{ };

    void initialize(uintptr_t base) noexcept;
    DWORD end(LPVOID instance) noexcept;
    void update_global_vars() noexcept;

}
