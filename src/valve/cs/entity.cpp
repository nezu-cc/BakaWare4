#include "entity.h"

#include "../../memory/interfaces.h"

cs::base_entity *cs::handle::get() noexcept {
    return interfaces::entity_list->get_base_entity(get_entry_index());
}
