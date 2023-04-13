#pragma once

#include "../../memory/memory.h"
#include "../se/util.h"

#define ENT_ENTRY_MASK 0x7fff

namespace cs {

enum class life_state : uint8_t {
	LIFE_ALIVE = 0x0,
	LIFE_DYING = 0x1,
	LIFE_DEAD = 0x2,
	LIFE_RESPAWNABLE = 0x3,
	LIFE_RESPAWNING = 0x4,
};

enum class move_type : uint8_t {
	MOVETYPE_NONE = 0x0,
	MOVETYPE_OBSOLETE = 0x1,
	MOVETYPE_WALK = 0x2,
	MOVETYPE_STEP = 0x3,
	MOVETYPE_FLY = 0x4,
	MOVETYPE_FLYGRAVITY = 0x5,
	MOVETYPE_PHYSICIST = 0x6,
	MOVETYPE_PUSH = 0x7,
	MOVETYPE_NOCLIP = 0x8,
	MOVETYPE_OBSERVER = 0x9,
	MOVETYPE_LADDER = 0xa,
	MOVETYPE_CUSTOM = 0xb,
	MOVETYPE_LAST = 0xc,
	MOVETYPE_MAX_BITS = 0x5,
};

enum flags : uint32_t {
    fl_onground = (1 << 0),
    fl_ducking = (1 << 1),
    fl_waterjump = (1 << 2),
    fl_ontrain = (1 << 3),
    fl_inrain = (1 << 4),
    fl_frozen = (1 << 5),
    fl_atcontrols = (1 << 6),
    fl_client = (1 << 7),
    fl_fakeclient = (1 << 8),
    fl_inwater = (1 << 9),
    fl_fly = (1 << 10),
    fl_swim = (1 << 11),
    fl_conveyor = (1 << 12),
    fl_npc = (1 << 13),
    fl_godmode = (1 << 14),
    fl_notarget = (1 << 15),
    fl_aimtarget = (1 << 16),
    fl_partialground = (1 << 17),
    fl_staticprop = (1 << 18),
    fl_graphed = (1 << 19),
    fl_grenade = (1 << 20),
    fl_stepmovement = (1 << 21),
    fl_donttouch = (1 << 22),
    fl_basevelocity = (1 << 23),
    fl_worldbrush = (1 << 24),
    fl_object = (1 << 25),
    fl_killme = (1 << 26),
    fl_onfire = (1 << 27),
    fl_dissolving = (1 << 28),
    fl_transragdoll = (1 << 29),
    fl_unblockable_by_player = (1 << 30)
};

enum bone_flags : uint32_t {
	FLAG_NO_BONE_FLAGS = 0x0,
	FLAG_BONEFLEXDRIVER = 0x4,
	FLAG_CLOTH = 0x8,
	FLAG_PHYSICS = 0x10,
	FLAG_ATTACHMENT = 0x20,
	FLAG_ANIMATION = 0x40,
	FLAG_MESH = 0x80,
	FLAG_HITBOX = 0x100,
	FLAG_BONE_USED_BY_VERTEX_LOD0 = 0x400,
	FLAG_BONE_USED_BY_VERTEX_LOD1 = 0x800,
	FLAG_BONE_USED_BY_VERTEX_LOD2 = 0x1000,
	FLAG_BONE_USED_BY_VERTEX_LOD3 = 0x2000,
	FLAG_BONE_USED_BY_VERTEX_LOD4 = 0x4000,
	FLAG_BONE_USED_BY_VERTEX_LOD5 = 0x8000,
	FLAG_BONE_USED_BY_VERTEX_LOD6 = 0x10000,
	FLAG_BONE_USED_BY_VERTEX_LOD7 = 0x20000,
	FLAG_BONE_MERGE_READ = 0x40000,
	FLAG_BONE_MERGE_WRITE = 0x80000,
	FLAG_ALL_BONE_FLAGS = 0xfffff,
	BLEND_PREALIGNED = 0x100000,
	FLAG_RIGIDLENGTH = 0x200000,
	FLAG_PROCEDURAL = 0x400000,
};

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

struct alignas(16) bone_data {
    vec3 pos;
    float scale;
    vec4 rot;
};

static_assert(sizeof(bone_data) == 0x20);

class model {
public:
    VIRTUAL_FUNCTION_SIG_ABSOLUTE(num_bones, uint32_t, dlls::client, "E8 ? ? ? ? 85 C0 7E 21", 1, (this))
    VIRTUAL_FUNCTION_SIG(bone_flags, uint32_t, dlls::client, "85 D2 78 16 3B 91", (this, index), uint32_t index)
    VIRTUAL_FUNCTION_SIG(bone_name, const char*, dlls::client, "85 D2 78 25 3B 91", (this, index), uint32_t index)
    VIRTUAL_FUNCTION_SIG(bone_parent, int32_t, dlls::client, "85 D2 78 17 3B 91 70", (this, index), uint32_t index)
};

class model_state {
public:
    NETVAR(m_hModel, "CModelState", "m_hModel", se::strong_handle<model>);

    bone_data* get_bone_data() noexcept {
        return address(this).offset(0x80).dereference<bone_data*>();
    }
};

class skeleton_instance {
public:
    NETVAR(m_modelState, "CSkeletonInstance", "m_modelState", model_state);

    VIRTUAL_FUNCTION_SIG_ABSOLUTE(get_bone, void, dlls::client, "E8 ? ? ? ? EB 19 48 8B CF", 1, (this, data, index), bone_data& data, int index)
    VIRTUAL_FUNCTION_SIG(calc_world_space_bones, void, dlls::client, "40 55 56 57 41 54 41 55 41 56 41 57 48 81 EC F0", (this, bone_mask), bone_flags bone_mask)
};

class game_scene_node {
public:
    NETVAR(m_vecAbsOrigin, "CGameSceneNode", "m_vecAbsOrigin", vec3);

    VIRTUAL_FUNCTION(get_skeleton_instance, skeleton_instance*, 8, (this))
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
    
    inline bool has_flag(flags flag) noexcept { return m_fFlags() & flag; }

    NETVAR(m_pGameSceneNode, "C_BaseEntity", "m_pGameSceneNode", game_scene_node*);
    NETVAR(m_pCollision, "C_BaseEntity", "m_pCollision", collision_property*);
    NETVAR(m_iTeamNum, "C_BaseEntity", "m_iTeamNum", uint8_t);
    NETVAR(m_lifeState, "C_BaseEntity", "m_lifeState", life_state);
    NETVAR(m_MoveType, "C_BaseEntity", "m_MoveType", move_type);
    NETVAR(m_fFlags, "C_BaseEntity", "m_fFlags", flags);
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

class player_pawn_base : public base_player_pawn {
public:
    NETVAR(m_iHealth, "C_CSPlayerPawnBase", "m_aimPunchCache", se::util_vector<angle>);
};

class player_pawn : public player_pawn_base {
public:
    VIRTUAL_FUNCTION_SIG(is_enemy, bool, dlls::client, "40 57 48 83 EC 40 48 8B F9 48 85", (this, other_player), player_pawn* other_player)
};

inline cs::player_controller* get_local_player_controller(int32_t split_screen_slot = 0) noexcept {
    SIG(sig, dlls::client, "E8 ? ? ? ? 49 89 47 08")
    auto fn = sig.absolute<player_controller*(__stdcall*)(int32_t)>();
    return fn(split_screen_slot);
}

}