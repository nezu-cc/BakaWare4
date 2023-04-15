#pragma once

#include "../../base/base.h"
#include "../../base/debug.h"

namespace se {

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

class alignas(16) transform {
public:
    alignas(16) vec3 pos;
    alignas(16) quaternion rot;

    void to_matrix(mat3x4& matrix) const noexcept;
};

template <class T>
class utl_memory {
public:
    T* memory;
    int allocation_count;
    int grow_size;
};

class util_string {
public:
    const char* get() const noexcept {
        return (const char*)memory.memory;
    }

    operator const char *() const noexcept { return get(); }

    utl_memory<uint8_t> memory;
    int actual_length;
};

static_assert(sizeof(util_string) == 0x18);

template <class T>
class util_vector {
public:
    int size;
    utl_memory<T> data;
    
    T& operator[](int i) noexcept { return data.memory[i]; }
    const T& operator[](int i) const noexcept { return data.memory[i]; }
};

static_assert(sizeof(util_vector<void*>) == 0x18);

}