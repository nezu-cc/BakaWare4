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

class pb_base {
private:
    PAD(0x18) // 0x0
};

class cmd_qangle : public pb_base {
public:
    angle angles; // 0x18
};

static_assert(sizeof(cmd_qangle) == 0x24);

class cmd_vector : public pb_base {
public:
    vec3 vector; // 0x18
    float w; // 0x24
};

static_assert(sizeof(cmd_vector) == 0x28);

class csgo_interpolation_info : public pb_base {
public:
    float frac; // 0x18
    int src_tick; // 0x1C
    int dst_tick; // 0x20
};

static_assert(sizeof(csgo_interpolation_info) == 0x24);

class csgo_input_history_entry : public pb_base {
public:
    cmd_qangle* view_angles; // 0x18
    cmd_vector* shoot_position; // 0x20
    cmd_vector* target_head_pos_check; // 0x28
    cmd_vector* target_abs_pos_check; // 0x30
    cmd_qangle* target_abs_ang_check; // 0x38
    csgo_interpolation_info* cl_interp; // 0x40
    csgo_interpolation_info* sv_interp0; // 0x48
    csgo_interpolation_info* sv_interp1; // 0x50 
    csgo_interpolation_info* player_interp; // 0x58
    int render_tick_count; // 0x60
    float render_tick_fraction; // 0x64
    int player_tick_count; // 0x68
    float player_tick_fraction; // 0x6C
    int frame_number; // 0x70
    int target_ent_index; // 0x74
};

static_assert(sizeof(csgo_input_history_entry) == 0x78);

class sub_tick_container {
public:
    std::int32_t tick_count; // 0x0
    PAD(0x4) // 0x4
    std::uint8_t* tick_pointer; // 0x8

    csgo_input_history_entry* get_history_entry(int32_t index) {
        if (index < this->tick_count)
        {
            csgo_input_history_entry** tick_list = reinterpret_cast<csgo_input_history_entry**>(this->tick_pointer + 0x8);
            return tick_list[index];
        }

        return nullptr;
    }
};

static_assert(sizeof(sub_tick_container) == 0x10);

class user_cmd_base {
public:
    PAD(0x40) // 0x0
    cmd_qangle* view; // 0x40
    int command_number; // 0x48
    int tick_count; // 0x4C
    float forwardmove; // 0x50
    float leftmove; // 0x54
    float upmove; // 0x58

    // TODO: maybe more here
};

struct in_button_state {
    PAD(sizeof(void*)) // 0x0 (vtable)
    uint64_t buttons1; // 0x8
    uint64_t buttons2; // 0x10
    uint64_t buttons3; // 0x18
};

struct user_cmd {
    PAD(0x20) // 0x0
    sub_tick_container sub_tick_container; // 0x20
    user_cmd_base* base; // 0x30
    int attack3_start_history_index; // 0x38 (release attack1)
    int attack1_start_history_index; // 0x3C (press attack1)
    int attack2_start_history_index; // 0x40 (press attack2)
    PAD(0x4) // 0x44
    in_button_state button_state; // 0x48
    PAD(0x8) // 0x68
};

static_assert(sizeof(user_cmd) == 0x70);

struct subtick_move_step {
    uint64_t button;
    bool pressed;
    float when;
};

static_assert(sizeof(subtick_move_step) == 0x10);

// FIXME: bad name, idk how to call it
struct move_input {
    float forwardmove;
    float leftmove;
    float upmove;
};

struct per_user_input {
    user_cmd cmds[150]; // 0x0
    PAD(0x30) // 0x41A0
    int sequence_number; // 0x41D0
    PAD(0x4) // 0x41D4
    uint64_t button_state1; // 0x41D8 (is held in this tick)
    uint64_t button_state2; // 0x41E0 (has been pressed down in this tick)
    uint64_t button_state3; // 0x41E8 (has been released in this tick)
    uint64_t button_state4; // 0x41F0 (was held down in previous tick)
    PAD(0x80) // 0x41F8
    move_input move; // 0x4278
    angle viewangles; // 0x4284
    int16_t mousedx; // 0x4290
    PAD(0x2) // 0x4292
    int16_t mousedy; // 0x4294
    PAD(0x2) // 0x4296
    int subtick_count; // 0x4298
    subtick_move_step subticks[12]; // 0x429C
};

static_assert(sizeof(per_user_input) == sizeof(user_cmd) * (150 + 4)); // 0x4360

struct csgo_input {
    static csgo_input* get() noexcept {
        return *dlls::client.find(PATTERN("48 8B 0D ? ? ? ? 48 8B 01 FF 50 ? 8B DF")).absolute<se::csgo_input**>(3);
    }

    VIRTUAL_FUNCTION_SIG(set_cursor_pos, void, dlls::client, "44 89 44 24 18 89 54 24 10 48 83", (this, x, y), uint32_t x, uint32_t y)

    per_user_input* get_per_user_input(int split_screen_index) {
        if (split_screen_index >= MAX_SPLITSCREEN_PLAYERS) {
            return nullptr;
        }
        return &per_user[split_screen_index];
    }

    user_cmd* get_user_cmd(int split_screen_index) {
        auto input = get_per_user_input(split_screen_index);
        if (!input)
            return nullptr;
        return &input->cmds[input->sequence_number % std::size(input->cmds)];
    }
private:
    PAD(0x10); // 0x0
    per_user_input per_user[MAX_SPLITSCREEN_PLAYERS]; // 0x10
};

struct client_mode {
    static client_mode* get() noexcept {
        return dlls::client.find(PATTERN("48 8D 0D ? ? ? ? 48 69 C0 ? ? ? ? 48 03 C1 C3 CC CC")).absolute<se::client_mode*>(3);
    }
};

struct global_vars {
public:
    float realtime; // 0x0
    std::int32_t frame_count; // 0x4
    PAD(0x8) // 0x8
    std::uint32_t max_clients; // 0x10
    float interval_per_tick; // 0x14
    PAD(0x14) // 0x18
    float curtime; // 0x2C
    float curtime2; // 0x30
    PAD(0xC) // 0x34
    std::int32_t tick_count; // 0x40
    float interval_per_tick2; // 0x44
    void* current_netchan; // 0x48
    PAD(0x130) // 0x50
    char* current_map; // 0x180
    char* current_mapname; // 0x188
};

struct view_render {
    static view_render* get() noexcept {
        SIG(function_ptr, dlls::client, "E8 ? ? ? ? 44 8B CB 4C 8D 44 24")
        return function_ptr.absolute<view_render*(__fastcall*)()>()();
    }
};

}