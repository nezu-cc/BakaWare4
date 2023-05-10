#pragma once
#include "../../memory/memory.h"

namespace se {

enum flow : int {
    FLOW_OUTGOING = 0,
    FLOW_INCOMING = 1,
};

class net_channel_info {
public:
    VIRTUAL_FUNCTION(get_latency, float, 10, (this, flow), flow flow);
};

};