#pragma once

#include "../../base/base.h"
#include "../../base/math.h"
#include "../../memory/memory.h"
#include "../../memory/interfaces.h"
#include "../../render/render.h"
#include "../../core/cheat.h"

namespace features {

    namespace esp {
        void render(render::renderer* r) noexcept;
    }

    namespace misc {
        void render(render::renderer* r) noexcept;
        void run(se::user_cmd* cmd) noexcept;
    }

    inline void render(render::renderer* r) noexcept {
        esp::render(r);
        misc::render(r);
    }

    inline void run(se::user_cmd* cmd) noexcept {
        misc::run(cmd);
    }

}
