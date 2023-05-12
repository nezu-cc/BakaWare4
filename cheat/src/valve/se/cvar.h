#pragma once
#include "../../base/base.h"
#include <format>

namespace se {

// FIXME: https://cs2.poggu.me/reverse-engineering/ConVars
class con_var {
public:
    const char* name; // 0x0
    PAD(0x18); // 0x8
    const char* description; // 0x20
    uint32_t type; // 0x28
    uint32_t registered; // 0x2C
    uint32_t flags; // 0x30
    PAD(0xC); // 0x34
    uint64_t value; // 0x40
    uint64_t value_old; // 0x48

    template <typename T>
    T GetValue() {
        return *reinterpret_cast<T*>(&value);
    }
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

    con_var* find(const char* name) noexcept {
        for (int i = con_vars.first(); i != con_vars.invalid_index(); i = con_vars.next(i)) {
            auto var = con_vars.element(i);
            if (strcmp(var->name, name) == 0)
                return var;
        }
        return nullptr;
    }
};

class static_convar {
private:
    std::mutex mtx { };
    bool found { };
    con_var* var { };
    const char* name { };

    void find() noexcept;

public:
    static_convar(const char* name) noexcept : name(name) { }

    inline con_var* get() noexcept {
        if (!found)
            find();
        return var;
    }
};

}

#define CONVAR(name, type) \
    static se::static_convar CONCAT(name, _static)(#name); \
    type name = CONCAT(name, _static).get()->GetValue<type>();