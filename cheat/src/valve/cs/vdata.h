#pragma once
#include "../../memory/memory.h"

class entity_subclass_v_data_base {
public:
    template <typename T> requires std::derived_from<T, entity_subclass_v_data_base>
    inline T* as() {
        return (T*)this;
    }
};

class base_player_weapon_v_data : public entity_subclass_v_data_base {
public:
    NETVAR(m_bIsFullAuto, "CBasePlayerWeaponVData", "m_bIsFullAuto", bool);
    NETVAR(m_iMaxClip1, "CBasePlayerWeaponVData", "m_iMaxClip1", int32_t);
};