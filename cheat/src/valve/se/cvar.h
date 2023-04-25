#pragma once
#include "../../base/base.h"

namespace se {

// FIXME: https://cs2.poggu.me/reverse-engineering/ConVars
class con_var {
public:
    const char* name;
    PAD(0x18)
    const char* description;
    uint32_t type;
    uint32_t registered;
    uint32_t flags;
    PAD(0xC)
    uint64_t value;
    uint64_t value_old;
};

static_assert(sizeof(con_var) == 0x50);

class cvar {
public:
    PAD(0x40)
    utl_linked_list<con_var*, short> con_vars;

    void unlock_all() noexcept {
        int count = 0;
        for (int i = con_vars.first(); i != con_vars.invalid_index(); i = con_vars.next(i)) {
            auto var = con_vars.element(i);
            var->flags &= ~0x402;
            count++;
        }
        LOG_INFO(XOR("Unlocked {} cvars"), count);
    }
};

}