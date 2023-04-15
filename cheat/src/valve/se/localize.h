#pragma once
#include "../../memory/memory.h"

namespace se {

class localize {
public:
    VIRTUAL_FUNCTION(find_safe, const char*, 17, (this, token), const char* token);
};

}