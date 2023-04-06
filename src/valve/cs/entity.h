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

class game_scene_node {
public:
    NETVAR(m_vecAbsOrigin, "CGameSceneNode", "m_vecAbsOrigin", vec3);
};

class collision_property {
public:
    NETVAR(m_vecMins, "CCollisionProperty", "m_vecMins", vec3);
    NETVAR(m_vecMaxs, "CCollisionProperty", "m_vecMaxs", vec3);
};

class player_weapon_services {
   public:
    // NETVAR(m_hActiveWeapon, "CPlayer_WeaponServices", "m_hActiveWeapon",
    //              CHandle);
    // NETVAR(m_hMyWeapons, "CPlayer_WeaponServices", "m_hMyWeapons",
    //               CNetworkUtlVectorBase<CHandle>);
};

class entity_instance {
public:
    // FIXME: m_pEntity
};

class base_entity : public entity_instance {
public:
    bool is_player_controller();
    bool get_bounding_box(bbox& out);

    NETVAR(m_pGameSceneNode, "C_BaseEntity", "m_pGameSceneNode", game_scene_node*);
    NETVAR(m_pCollision, "C_BaseEntity", "m_pCollision", collision_property*);
    NETVAR(m_iTeamNum, "C_BaseEntity", "m_iTeamNum", uint8_t);
};

class base_player_controller : public base_entity {
public:
    NETVAR(m_steamID, "CBasePlayerController", "m_steamID", uint64_t);
    NETVAR(m_hPawn, "CBasePlayerController", "m_hPawn", handle);
    NETVAR(m_bIsLocalPlayerController, "CBasePlayerController", "m_bIsLocalPlayerController", bool);
};

class player_controller : public base_player_controller {
public:
    NETVAR(m_sSanitizedPlayerName, "CCSPlayerController", "m_sSanitizedPlayerName", const char*);
    NETVAR(m_iPawnHealth, "CCSPlayerController", "m_iPawnHealth", uint32_t);
    NETVAR(m_bPawnIsAlive, "CCSPlayerController", "m_bPawnIsAlive", bool);
};

class base_player_pawn : public base_entity {
public:
    NETVAR(m_hController, "C_BasePlayerPawn", "m_hController", handle);
    NETVAR(m_pWeaponServices, "C_BasePlayerPawn", "m_pWeaponServices", player_weapon_services*);
};

class player_pawn : public base_player_pawn {
    // bool IsEnemyToLocalPlayer(int lpTeam);
};

}