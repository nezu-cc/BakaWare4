#pragma once
#include <unordered_map>
#include <shared_mutex>
#include <unordered_map>
#include "../base/base.h"
#include "../valve/cs/entity.h"
#include <span>

namespace entity_cache {

enum class entity_type : uint8_t {
    PLAYER = 0,
    WEAPON,
    // TODO: grenades
    // TODO: bomb
    // TODO: memes (chicken, fish, football, etc.)
    UNKNOWN,
};

class cached_entity {
public:
    cs::handle<> handle;
    entity_type type;
    bbox bb{ };
    std::array<std::pair<vec2, vec2>, MAX_STUDIO_BONES> scr_bones{ };
    int visible_bone_count{ };
    bool draw{ };

    cached_entity(cs::handle<> handle, entity_type type) noexcept
        : handle(handle), type(type) {}
    
    cached_entity(const cached_entity&) = delete;
    cached_entity(cached_entity&&) = default;
    cached_entity& operator=(const cached_entity&) = default;

    template <typename T> requires std::derived_from<T, cs::base_entity>
    std::add_pointer_t<T> get() const noexcept {
        return handle.get<T>();
    }

    inline auto get_scr_bones() const noexcept {
        return std::span(scr_bones).first(visible_bone_count);
    }
};

// typedef for entity callback
using entity_callback = void(*)(int index, const cs::handle<> handle) noexcept;
struct entity_callbacks {
    entity_callback add;
    entity_callback remove;
};

inline std::shared_mutex mutex;
inline std::unordered_map<int, cached_entity> entities;
inline std::unordered_multimap<entity_type, entity_callbacks> callbacks;

entity_type get_entity_type(cs::base_entity *entity) noexcept;

void initialize() noexcept;
void on_add_entity(cs::base_entity* entity, cs::handle<cs::base_entity> handle) noexcept;
void on_remove_entity(cs::base_entity* entity, cs::handle<cs::base_entity> handle) noexcept;
void update_bounding_boxes() noexcept;
void register_callback(entity_type type, entity_callback add, entity_callback remove) noexcept;

}