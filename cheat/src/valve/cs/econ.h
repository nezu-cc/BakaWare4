#pragma once

#include "../../memory/memory.h"

namespace cs {

class econ_item_definition {
public:
    PAD(0x8); // vtable
    void* kv_item;
    uint16_t def_index;
    se::util_vector<uint16_t> associated_items_def_indexes;
    bool enabled;
    const char* prefab;
    uint8_t min_item_level;
    uint8_t max_item_level;
    uint8_t item_rarity;
    uint8_t item_quality;
    uint8_t forced_item_quality;
    uint8_t default_drop_item_quality;
    uint8_t default_drop_quantity;
    se::util_vector<void*> static_attributes;
    uint8_t popularity_seed;
    void* portraits_kv;
    const char* item_base_name;
    bool proper_name;
    const char* item_type_name;
    uint32_t item_type_id;
    const char* item_desc;

    auto get_simple_weapon_name() {
        return address(this).offset(0x210).dereference<const char*>();
    }
};

class econ_item_view {
public:
    VIRTUAL_FUNCTION(get_static_data, econ_item_definition*, 13, (this))
};

class attribute_container {
public:
    NETVAR(m_Item, "C_AttributeContainer", "m_Item", econ_item_view);
};

}