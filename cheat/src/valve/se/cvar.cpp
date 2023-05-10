#include "../../memory/memory.h"
#include "../../memory/interfaces.h"
#include "cvar.h"

inline void se::static_convar::find() noexcept {
    std::scoped_lock lock(mtx);
    if (found)
        return;

    var = interfaces::cvar->find(name);
    ASSERT_MSG(var, std::vformat(XOR("Failed to find convar {}"), std::make_format_args(name)));

    LOG_INFO(XOR("Found convar {}"), name);
    found = true;
}