#pragma once

#include <nlohmann/json.hpp>

namespace config {

class esp;

class player_visuals {
public:
    bool enabled{ true };
    bool health{ true };
    bool box{ true };
    bool name{ true };
    // bool weapon{ true };
    bool skeleton{ true };
    // bool ammo{ true };

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
        player_visuals, enabled, box, name, health, skeleton
    )
};

class esp_config {
public:
    player_visuals players{ };
    // visuals weapons{ };
    // visuals grenades{ };

    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
        esp_config, players
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

void load(std::wstring_view file) noexcept;
void save(std::wstring_view file) noexcept;

}

inline config::conf cfg{ };