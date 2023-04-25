#pragma once

#include "../../base/base.h"
#include "../../base/math.h"
#include "../../memory/memory.h"
#include "../../memory/interfaces.h"
#include "../../render/render.h"
#include "../../core/cheat.h"
#include "../../valve/se/se.h"
#include "../../valve/cs/cs.h"

namespace features {

    namespace esp {
        void render(render::renderer* r) noexcept;
    }

    namespace misc {
        void render(render::renderer* r) noexcept;
        void create_move(se::user_cmd* cmd) noexcept;
        void input(angle* va, se::move_input* input, float frame_time) noexcept;
    }

    inline void render(render::renderer* r) noexcept {
        esp::render(r);
        misc::render(r);
    }

    inline void create_move(se::user_cmd* cmd) noexcept {
        misc::create_move(cmd);
    }

    inline void input(angle* va, se::move_input* input, float frame_time) noexcept {
        misc::input(va, input, frame_time);
    }

}
