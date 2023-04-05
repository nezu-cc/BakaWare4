#pragma once

#include "../valve/se/se.h"
#include "memory.h"

namespace interfaces {

    inline std::vector<void*> hooked_tables{ };
    inline std::vector<std::pair<std::string, void*>> list{ };

}

template<class ptr> requires std::is_pointer_v<ptr>
struct interface_holder {
    ptr instance{ };
    uintptr_t* real_vmt{ };
    std::unique_ptr<uintptr_t[]> replacement_vmt{ };

    constexpr ptr operator->() const noexcept { return instance; }
    constexpr void operator=(ptr rhs) noexcept { instance = rhs; }
    constexpr operator bool() const noexcept { return instance; }
    constexpr operator ptr() noexcept { return instance; }

    // Pass false to replace_vmt if you don't hook anything from the table or if get_vmt_length() is crashing
    template<bool replace_vmt = false>
    inline void initialize(ptr vptr) noexcept
    {
        ASSERT(vptr);
        constexpr int dynamic_cast_info_len = 1;

        instance = vptr;

        if constexpr (replace_vmt) {
            real_vmt = *reinterpret_cast<uintptr_t**>(instance);

            const auto len = memory::get_vmt_length(real_vmt) + dynamic_cast_info_len;
            replacement_vmt = std::make_unique<uintptr_t[]>(len);
            std::ranges::copy(real_vmt - dynamic_cast_info_len, real_vmt + len - dynamic_cast_info_len, replacement_vmt.get());

            *reinterpret_cast<uintptr_t**>(instance) = replacement_vmt.get() + dynamic_cast_info_len;

            interfaces::hooked_tables.push_back(this);
        }
    }

    inline void restore() noexcept
    {
        *reinterpret_cast<uintptr_t**>(instance) = real_vmt;
    }
};

namespace interfaces {

inline interface_holder<se::client_dll*>            client{ };
inline interface_holder<se::engine_client*>         engine{ };
inline interface_holder<se::game_resource_service*> game_resource{ };
inline interface_holder<se::entity_list*>           entity_list{ };
inline interface_holder<se::csgo_input*>            csgo_input{ };
inline interface_holder<se::schema_system*>         schema_system{ };
inline interface_holder<se::input_system*>          input_system{ };

void initialize() noexcept;

}
