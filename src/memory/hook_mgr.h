#pragma once

#include <map>
#include "memory.h"
#include "../base/debug.h"

#define SET_SIG_HOOK(dll, sig, name) set(dll, PATTERN(sig), reinterpret_cast<void*>(name::fn), reinterpret_cast<void**>(&name::original))
#define SET_VT_HOOK(inter, name, index) set(inter, index, name::fn, name::original)

namespace hooks {

    inline std::add_pointer_t<bool(void*, void*, void*, bool)> hook_func{ };
    inline std::add_pointer_t<void(void*, bool)> unhook_func{ };

    inline std::map<void*, void*> hooked_fns{ }; /* Only contains signature hooks */

    void initialize() noexcept;
    void end() noexcept;

    template<class ty, class fn>
    void set(interface_holder<ty*>& vmt, int index, void* hook, fn& original) noexcept
    {
        ASSERT_MSG(vmt.replacement_vmt, "Trying to set hook with replace_vmt = false!");
        vmt.replacement_vmt[index + 1] = reinterpret_cast<uintptr_t>(hook);
        original = reinterpret_cast<fn>(vmt.real_vmt[index]);
    };

    template<size_t len>
    void set(dll& dll, std::array<int, len>&& sig, void* hook, void** original) noexcept
    {
        auto target = dll.find<len>(std::move(sig));
        ASSERT(target);
        ASSERT(dll.is_within_section(target, ".text"_hash));
        hooked_fns[hook] = target;
        if (!hook_func(target, hook, original, false))
            LOG_ERROR("Error while hooking function!"); /* Not fatal, but we should warn about it */
    };

}
