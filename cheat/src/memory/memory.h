#pragma once

#define CONCAT_IMPL(x, y) x##y
#define CONCAT(x, y) CONCAT_IMPL(x, y)
#define PAD(size) private: [[maybe_unused]] std::byte CONCAT(pad, __COUNTER__)[size]{ }; public:

#include <intrin.h>

#include "address.h"
#include "dll.h"
#include "netvars.h"

namespace memory {

    template<class ty, int i, class... va_args>
    ty call_virtual(void* base, va_args... args) noexcept
    {
        return (*static_cast<ty(__thiscall***)(void*, va_args...)>(base))[i](base, args...);
    }

    template<class ty = address>
    ty get_virtual(void* base, int index) noexcept
    {
        return (*static_cast<ty**>(base))[index];
    }

    inline address get_frame_address() noexcept
    {
        return address(reinterpret_cast<uintptr_t>(_AddressOfReturnAddress()) - sizeof(uintptr_t));
    }

    inline bool is_address_valid(address addr) noexcept
    {
        if (!addr)
            return false;

        MEMORY_BASIC_INFORMATION info{ };
        if (!VirtualQuery(addr, &info, sizeof(info)))
            return false;

        return info.State & MEM_COMMIT && !(info.Protect & PAGE_NOACCESS);
    }

    inline size_t get_vmt_length(uintptr_t* vptr) noexcept
    {
        size_t length{ };
        MEMORY_BASIC_INFORMATION info{ };

        while (VirtualQuery(reinterpret_cast<LPCVOID>(vptr[length]), &info, sizeof(info)) &&
               info.State & MEM_COMMIT &&
               info.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY))
            ++length;

        return length;
    }

}

#define VIRTUAL_FUNCTION(name, ret, idx, args, ... /* params */) \
inline ret name(__VA_ARGS__) noexcept \
{ \
    return memory::call_virtual<ret, idx>args; \
}

// Parameters and arguments are reversed, only way I could get this macro to work properly.
#define VIRTUAL_FUNCTION_SIG(name, ret, dll, sig, args, ... /* params */) \
inline ret name(__VA_ARGS__) noexcept \
{ \
    static ret(__thiscall* fn)(void*, __VA_ARGS__) = dll.find(PATTERN(sig)).cast<decltype(fn)>(); \
    return fn args; \
}

#define VIRTUAL_FUNCTION_SIG_ABSOLUTE(name, ret, dll, sig, offset, args, ... /* params */) \
inline ret name(__VA_ARGS__) noexcept \
{ \
    static ret(__thiscall* fn)(void*, __VA_ARGS__) = dll.find(PATTERN(sig)).absolute<decltype(fn)>(offset); \
    return fn args; \
}

