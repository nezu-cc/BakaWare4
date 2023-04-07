#include "netvars.h"
#include "interfaces.h"

using NetvarKeyValueMap_t = std::unordered_map<uint32_t, int16_t>;
using NetvarTableMap_t = std::unordered_map<uint32_t, NetvarKeyValueMap_t>;

static bool init_netvars_for_class(NetvarTableMap_t& table_map, const char* class_name, uint32_t class_key) noexcept {
    auto type = interfaces::schema_system->find_type_scope_for_module("client.dll");
    if (!type) {
        LOG_ERROR("init_netvars_for_class: type scope not found for module client.dll");
        return false;
    }

    auto class_info = type->find_declared_class(class_name);
    if (!class_info) {
        table_map.emplace(class_key, NetvarKeyValueMap_t{});
        LOG_ERROR("init_netvars_for_class: class {} not found", class_name);
        return false;
    }

    auto size = class_info->size();
    auto field_data = class_info->get_field_data();

    auto& kv_Map = table_map[class_key];
    kv_Map.reserve(size);

    for (auto i = 0; i < size; i++) {
        auto& field = field_data[i];
        kv_Map.emplace(fnv1a::hash(field.name), field.offset);
    }

    return true;
}

uintptr_t netvars::get_offset(const char *class_name, uint32_t class_key, const char *member_name, uint32_t member_key) noexcept {
    static NetvarTableMap_t table_map;
    const auto& table_map_it = table_map.find(class_key);
    if (table_map_it == table_map.cend()) {
        init_netvars_for_class(table_map, class_name, class_key);
        return get_offset(class_name, class_key, member_name, member_key);
    }

    const auto& kv_map = table_map_it->second;
    const auto& kv_map_it = kv_map.find(member_key);
    if (kv_map_it == kv_map.cend()) {
        LOG_ERROR("netvars::get_offset: member {} not found in class {}", member_name, class_name);
        return 0;
    }

    LOG_INFO("resolved netvar offset {}::{} -> {}", class_name, member_name, kv_map_it->second);

    return kv_map_it->second;
}
