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
        if (!binding)
            return nullptr;
        return (T*)binding->data;
    }
    T* operator->() noexcept {
        if (!binding)
            return nullptr;
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

    T& operator[](int i) noexcept { return memory[i]; }
    const T& operator[](int i) const noexcept { return memory[i]; }
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

#define INVALID_LLIST_IDX ((I)~0)

template <class T, class I = int> 
class utl_linked_list {
public:
    inline I first() const noexcept { return head; }
    inline I previous( I i ) const noexcept { return memory[i].previous; }
    inline I next( I i ) const noexcept { return memory[i].next; }
    inline T& element( I i ) noexcept { return memory[i].element; }
    inline const T& element( I i ) const noexcept { return memory[i].element; }
    inline static I invalid_index() noexcept { return INVALID_LLIST_IDX; }

    struct alignas(8) list_elem {
		T element;
		I previous;
		I next;
	};

    utl_memory<list_elem> memory;
    I head;
	I tail;
	I first_free;
	I element_count;
	I total_elements;
	list_elem* elements;
};

struct game_tick {
    int32_t m_Value;
};

}