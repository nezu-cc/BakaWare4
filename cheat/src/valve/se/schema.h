#pragma once

#include "../../memory/memory.h"

namespace se {

struct type_scope;
struct schema_class_info_data;
struct schema_class_field_data;

struct schema_system {
    VIRTUAL_FUNCTION(find_type_scope_for_module, type_scope*, 13, (this, module), const char* module)
};

struct type_scope {
    VIRTUAL_FUNCTION(find_declared_class, schema_class_info_data*, 2, (this, name), const char* name)
};

struct schema_class_info_data {
    short size() {
        return *address(this).offset(0x1C).cast<short*>();
    }

    schema_class_field_data* get_field_data() {
        return *address(this).offset(0x28).cast<schema_class_field_data**>();
    }
};

struct schema_class_field_data {
    const char* name;
    PAD(0x8);
    short offset;
    PAD(0xE);
};

}