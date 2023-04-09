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

    inline void erase_pe_headers(uintptr_t base) noexcept
    {
    #ifndef _DEBUG
        HMODULE mod;
        if (GetModuleHandleExA(
            GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            reinterpret_cast<LPCSTR>(base),
            &mod
        ) != 0) {
            LOG_ERROR(XOR("erase_pe_header: module is not manually mapped, ignoring"));
            return;
        }

        MEMORY_BASIC_INFORMATION info{ };
        if (!VirtualQuery(reinterpret_cast<LPCVOID>(base), &info, sizeof(info))) {
            LOG_ERROR(XOR("erase_pe_header: VirtualQuery failed"));
            return;
        }

        if ((info.Protect & (PAGE_READWRITE | PAGE_EXECUTE_READWRITE)) == 0) {
            LOG_ERROR(XOR("erase_pe_header: pe headers are not writable, ignoring"));
            return;
        }

        auto dos = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
        if (dos->e_magic != IMAGE_DOS_SIGNATURE) {
            LOG_ERROR(XOR("erase_pe_header: invalid DOS header"));
            return;
        }

        auto nt = reinterpret_cast<IMAGE_NT_HEADERS*>(base + dos->e_lfanew);
        if (nt->Signature != IMAGE_NT_SIGNATURE) {
            LOG_ERROR(XOR("erase_pe_header: invalid NT header"));
            return;
        }

        std::memset(reinterpret_cast<void*>(base), 0, nt->OptionalHeader.SizeOfHeaders);
    #endif
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

