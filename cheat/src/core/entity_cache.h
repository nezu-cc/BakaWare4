#pragma once
#include <unordered_map>
#include <shared_mutex>
#include "../base/base.h"
#include "../valve/cs/entity.h"

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
    cs::handle<cs::base_entity> handle;
    entity_type type;
    bbox bb;
    bool draw;

    cached_entity(cs::handle<cs::base_entity> handle, entity_type type) noexcept
        : handle(handle), type(type), bb(), draw(false) {}
    
    cached_entity(const cached_entity&) = delete;
    cached_entity(cached_entity&&) = default;
    cached_entity& operator=(const cached_entity&) = default;

    template <typename T> requires std::derived_from<T, cs::base_entity>
    std::add_pointer_t<T> get() const noexcept {
        return handle.get<T>();
    }
};

inline std::shared_mutex mutex;
inline std::unordered_map<int, cached_entity> entities;

entity_type get_entity_type(cs::base_entity *entity) noexcept;

void initialize() noexcept;
void on_add_entity(cs::base_entity* entity, cs::handle<cs::base_entity> handle) noexcept;
void on_remove_entity(cs::base_entity* entity, cs::handle<cs::base_entity> handle) noexcept;
void update_bounding_boxes() noexcept;

}