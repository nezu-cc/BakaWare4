#include "interfaces.h"
#include "dll.h"

static void collect_interfaces(dll& dll) noexcept;
template<bool replace_vmt = false, class ty>
static void get_cached_interface(interface_holder<ty*>& ptr, std::string_view version_string) noexcept;
template<bool replace_vmt = false, class ty>
static void find_interface(interface_holder<ty*>& ptr, dll& dll, std::string_view version_string) noexcept;

void interfaces::initialize() noexcept
{
    collect_interfaces(dlls::client);
    collect_interfaces(dlls::engine2);
    collect_interfaces(dlls::schemasystem);
    collect_interfaces(dlls::input_system);

    get_cached_interface(client, "Source2Client002");
    get_cached_interface(engine, "Source2EngineToClient001");
    get_cached_interface(game_resource, "GameResourceServiceClientV001");
    get_cached_interface(schema_system, "SchemaSystem_001");
    get_cached_interface<true>(input_system, "InputSystemVersion001");

    entity_list.initialize(game_resource->get_entity_list());
    csgo_input.initialize<true>(*dlls::client.find(PATTERN("48 8B 0D ? ? ? ? 48 8B 01 FF 50 ? 8B DF")).absolute<se::csgo_input**>(3));
    client_mode.initialize<true>(dlls::client.find(PATTERN("48 8D 0D ? ? ? ? 48 69 C0 ? ? ? ? 48 03 C1 C3 CC CC")).absolute<se::client_mode*>(3));

    LOG_INFO("Interfaces initialized.");
}

namespace se {

struct interface_reg {
    std::add_pointer_t<void*()> create_fn{ };
    const char* name{ };
    interface_reg* next{ };
};

}

static auto get_interface_regs(dll& dll) noexcept
{
    if (!dll.create_interface) {
        dll.create_interface = dll.get_export("CreateInterface"_hash);
        ASSERT(dll.create_interface);
    }

    // Follow jmp instruction inside function to get to CreateInterfaceInternal(), where the global interface list is moved into ESI.
    const auto reg_list = *dll.create_interface.absolute<se::interface_reg**>(0x3);
    ASSERT(reg_list);
    return reg_list;
}

static void collect_interfaces(dll& dll) noexcept
{
    for (auto cur = get_interface_regs(dll); cur; cur = cur->next) {
        LOG_INFO("{}: found interface {}", dll.name, cur->name);
        interfaces::list.push_back(std::make_pair(cur->name, cur->create_fn()));
    }
}

// Version strings may be partial.

template<bool replace_vmt, class ty>
static void find_interface(interface_holder<ty*>& ptr, dll& dll, std::string_view version_string) noexcept
{
    for (auto cur = get_interface_regs(dll); cur; cur = cur->next) {
        if (std::string(cur->name).starts_with(version_string)) {
            LOG_SUCCESS("Found interface {}.", cur->name);
            ptr.template initialize<replace_vmt>(static_cast<ty*>(cur->create_fn()));
            return;
        }
    }
    ASSERT(false);
}

template<bool replace_vmt, class ty>
static void get_cached_interface(interface_holder<ty*>& ptr, std::string_view version_string) noexcept
{
    for (const auto& [name, iface] : interfaces::list) {
        if (name.starts_with(version_string.data())) {
            ptr.template initialize<replace_vmt>(static_cast<ty*>(iface));
            return;
        }
    }
}
