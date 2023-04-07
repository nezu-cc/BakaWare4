#pragma once

#include <cstddef>
#include <cstdint>

#include "../base/debug.h"

struct address {
    uintptr_t value{ };

    address() = default;
    address(uintptr_t value) noexcept
        : value(value) { }
    address(uintptr_t* value) noexcept
        : value(reinterpret_cast<uintptr_t>(value)) { }
    address(uint8_t* value) noexcept
        : value(reinterpret_cast<uintptr_t>(value)) { }
    address(void* value) noexcept
        : value(reinterpret_cast<uintptr_t>(value)) { }
    address(std::nullptr_t value) noexcept
        : value(0) { }

    operator bool() const noexcept { return value != 0; }
    operator void*() noexcept { return reinterpret_cast<void*>(value); }

    template<class ty>
    constexpr ty cast() noexcept
    {
        ASSERT(value != 0);
        return reinterpret_cast<ty>(value);
    }

    constexpr address& offset(ptrdiff_t offset) noexcept
    {
        value += offset;
        return *this;
    }

    template<class ty>
    ty& dereference() noexcept
    {
        ASSERT(value != 0);
        return *reinterpret_cast<ty*>(value);
    }

    template<class ty>
    ty absolute(ptrdiff_t rel_offset = 0x1, ptrdiff_t abs_offset = 0x0) noexcept
    {
        ASSERT(value != 0);
        const auto jmp = value + rel_offset;
        const auto target = *reinterpret_cast<int32_t*>(jmp);
        if (target)
            // Base address + offset + size of next instruction + target address.
            return reinterpret_cast<ty>(jmp + abs_offset + sizeof(int32_t) + target);
        return ty();
    }
};
