#pragma once

#include "../base/base.h"
#include "../crypt/fnv1a.h"

namespace netvars {

uintptr_t get_offset(const char* className, uint32_t classKey, const char* memberName, uint32_t memberKey) noexcept;

}

#define NETVAR_OFFSET(var_name, datatable, prop_name, extra_offset, type) \
    std::add_lvalue_reference_t<type> var_name() {                        \
                                                                          \
        static const auto offset = netvars::get_offset(                   \
            datatable, datatable##_hash, prop_name, prop_name##_hash);    \
                                                                          \
        return *reinterpret_cast<std::add_pointer_t<type>>(               \
            (uintptr_t)(this) + offset + extra_offset);                   \
    }

#define NETVAR(var_name, datatable, prop_name, type) \
    NETVAR_OFFSET(var_name, datatable, prop_name, 0, type)