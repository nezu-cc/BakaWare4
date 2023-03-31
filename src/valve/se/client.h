#pragma once

#include "../../memory/memory.h"
#include "../cs/entity.h"

namespace se {

struct entity_list;

struct client_dll {
  public:
};

struct game_resource_service {
    entity_list* get_entity_list() {
        return *address(this).offset(0x58).cast<entity_list**>();
    }
};

struct entity_list {
    template <class ty = cs::base_entity *>
    VIRTUAL_FUNCTION_SIG_ABSOLUTE(get_base_entity, ty, dlls::client, "8B D3 E8 ? ? ? ? 48 8B F8 48 85 C0 74 76", 3, (this, index), int index)
    VIRTUAL_FUNCTION_SIG_ABSOLUTE(get_max_entities, int, dlls::client, "33 DB E8 ? ? ? ? 8B 08", 3, (this, max), int* max)
};

} // namespace se