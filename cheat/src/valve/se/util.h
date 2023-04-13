#pragma once

#include "../../base/base.h"

namespace se {

template <class T>
class util_vector {
public:
    size_t size;
    T* data;
};

struct resource_binding {
    void* data;
    // uint32_t last_bind_frame;
    // uint32_t flags;
    // interlocked_int ref_count;
};

template <class T>
class strong_handle {
public:
    operator T*() noexcept {
        ASSERT(binding);
        return (T*)binding->data;
    }
    T* operator->() noexcept {
        ASSERT(binding);
        return (T*)binding->data;
    }
private:
    const resource_binding *binding;
};

}