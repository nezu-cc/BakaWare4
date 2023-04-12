#pragma once

#include "../../memory/memory.h"

namespace se {

struct plat_window;

struct input_context {
    const char* name;
    PAD(0x9C)
    bool enabled;
};

struct input_stack_system {
    VIRTUAL_FUNCTION(set_mouse_capture, void, 16, (this, context, state), input_context* context, bool state);

    input_context* find_input_context(const char* name) {
        for (int i = 0; i < context_count; i++) {
            if (strcmp(contexts[i]->name, name) == 0)
                return contexts[i];
        }
        return nullptr;
    }
private:
    PAD(0x28)
    int context_count;
    input_context** contexts;
};

}
