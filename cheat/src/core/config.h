#pragma once

#include <nlohmann/json.hpp>
#include "../base/types/color.h"

namespace config {

class esp;

class player_esp {
public:
    bool enabled{ true };
    bool teammates{ false };
    bool box{ true };
    bool health{ true };
    bool skeleton{ true };
    bool name{ true };
    bool weapon_name{ true };
    bool weapon_ammo{ true };

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
        player_esp, enabled, teammates, box, health, skeleton, name, weapon_name, weapon_ammo
    )
};

class weapon_esp {
public:
    bool enabled{ true };
    bool box{ true };
    bool name{ true };
    bool ammo{ true };

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
        weapon_esp, enabled, box, name, ammo
    )
};

class esp_config {
public:
    player_esp players{ };
    weapon_esp weapons{ };

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
        esp_config, players, weapons
    )
};

class rcs_config {
public:
    bool enabled{ true };
    // bool standalone{ false };
    union {
        struct {
            float horizontal;
            float vertical;
        };
        float axis[2] = { 100.0f, 100.0f };
    };

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
        rcs_config, enabled, horizontal, vertical
    )
};

class misc_config {
public:
    bool reoil_crosshair { true };
    rcs_config rcs { }; // TODO: move to legit bot
    bool bunny_hop { false };

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
        misc_config, reoil_crosshair, rcs, bunny_hop
    )
};

class conf {
public:
    esp_config esp{ };
    misc_config misc{ };

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
        conf, esp, misc
    )
};

class debug_config {
public:
    uint32_t debug_flags{ 0 };
    int32_t debug_int{ 0 };
    float debug_float{ 0.0f };
    clr4 debug_color = clr4::white(255);
};

void load(std::wstring_view file) noexcept;
void save(std::wstring_view file) noexcept;

}

inline config::conf cfg{ };
inline config::debug_config debug_cfg{ };