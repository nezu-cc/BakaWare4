#pragma once

#include "../../memory/memory.h"
#include "../cs/entity.h"

#define MAX_SPLITSCREEN_PLAYERS 1

namespace se {

enum buttons : uint32_t {
    in_attack = 1,
    in_jump = 2,
    in_duck = 4,
    in_attack2 = 2048,
    in_moveforward = 8,
    in_moveback = 16,
    in_moveleft = 512,
    in_moveright = 1024
};

struct entity_list;

struct client_dll {
  public:
};

struct game_resource_service {
    entity_list* get_entity_list() {
        return address(this).offset(0x58).dereference<entity_list*>();
    }
};

struct entity_list {
    template <class ty = cs::base_entity>
    VIRTUAL_FUNCTION_SIG_ABSOLUTE(get_base_entity, ty*, dlls::client, "8B D3 E8 ? ? ? ? 48 8B F8 48 85 C0 74 76", 3, (this, index), int index)

    inline int32_t get_highest_entity_index() {
        int32_t max = -1;
        get_max_entities(&max);
        return max;
    }
private:
    VIRTUAL_FUNCTION_SIG_ABSOLUTE(get_max_entities, int, dlls::client, "33 DB E8 ? ? ? ? 8B 08", 3, (this, max), int32_t* max)
};

struct cmd_qangle {
    char pad1[0x18];
    vec3 angles;
};

static_assert(sizeof(cmd_qangle) == 0x24);

struct user_cmd_base {
    PAD(0x40)
    cmd_qangle* view;
    PAD(0x8)
    float forwardmove;
    float sidemove;

    // TODO: maybe more here
};

struct user_cmd {
    PAD(0x30)
    user_cmd_base* base;
    PAD(0x18)
    uint32_t buttons;
    PAD(0x1C)
};

static_assert(sizeof(user_cmd) == 0x70);

struct per_user_input {
    user_cmd cmds[150];
    PAD(0x30)
    int sequence_number;
    PAD(0x18C)
};

static_assert(sizeof(per_user_input) == sizeof(user_cmd) * (150 + 4)); // 0x4360

struct csgo_input {
    VIRTUAL_FUNCTION_SIG(set_cursor_pos, void, dlls::client, "44 89 44 24 18 89 54 24 10 48 83", (this, x, y), uint32_t x, uint32_t y)

    user_cmd* get_user_cmd(int split_screen_index) {
        if (split_screen_index >= MAX_SPLITSCREEN_PLAYERS) {
            return nullptr;
        }
        auto input = &per_user[split_screen_index];
        return &input->cmds[input->sequence_number % std::size(input->cmds)];
    }
private:
    PAD(0x10);
    per_user_input per_user[MAX_SPLITSCREEN_PLAYERS];
};

struct client_mode {

};

struct global_vars {
public:
    float realtime;
    std::int32_t frame_count;
    PAD(0x8)
    std::uint32_t max_clients;
    float interval_per_tick;
    PAD(0x12)
    float curtime;
    float curtime2;
    PAD(0xC)
    std::int32_t tick_count;
    float interval_per_tick2;
    void* current_netchan;
    PAD(0x130)
    char* current_map;
    char* current_mapname;
};

}