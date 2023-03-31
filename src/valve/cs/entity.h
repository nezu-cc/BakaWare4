#pragma once

#include "../../memory/memory.h"

#define ENT_ENTRY_MASK 0x7fff

namespace cs {

class base_entity;

class handle {
public:
    base_entity* get() noexcept;

    inline int get_entry_index() noexcept { return m_Index & ENT_ENTRY_MASK; }
    template <typename T>
    inline T* get_as() {
        return (T*)(get());
    }

    uintptr_t m_Index;
};

class entity_instance {
    // FIXME: m_pEntity
};

class base_entity : public entity_instance {
    // TODO: shit
};

class base_player_controller : public base_entity {

};

class player_controller : public base_player_controller {
    NETVAR(m_steamID, "CBasePlayerController", "m_steamID", uint64_t);
    NETVAR(m_hPawn, "CBasePlayerController", "m_hPawn", handle);
};

}