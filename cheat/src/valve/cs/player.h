#pragma once
#include "../../memory/memory.h"
#include "entity.h"

namespace cs {

class player_pawn_component { };

class player_weapon_services : public player_pawn_component {
public:
    NETVAR(m_hActiveWeapon, "CPlayer_WeaponServices", "m_hActiveWeapon", handle<base_player_weapon>);
};

}