#pragma once

#include <nlohmann/json.hpp>

namespace config {

class esp;

class player_esp {
public:
    bool enabled{ true };
    bool teammates{ false };
    bool health{ true };
    bool box{ true };
    bool name{ true };
    // bool weapon{ true };
    bool skeleton{ true };
    // bool ammo{ true };

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
        player_esp, enabled, box, name, health, skeleton
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

class misc_config {
public:
    bool reoil_crosshair { true };
    bool bunny_hop { true };

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
        misc_config, reoil_crosshair, bunny_hop
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
};

void load(std::wstring_view file) noexcept;
void save(std::wstring_view file) noexcept;

}

inline config::conf cfg{ };
inline config::debug_config debug_cfg{ };