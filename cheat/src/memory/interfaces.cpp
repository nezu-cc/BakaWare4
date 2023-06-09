#include "interfaces.h"
#include "dll.h"

static void collect_interfaces(dll& dll) noexcept;
template<class ty>
static void get_cached_interface(interface_holder<ty*>& ptr, std::string_view version_string) noexcept;

void interfaces::initialize() noexcept
{
    collect_interfaces(dlls::tier0);
    collect_interfaces(dlls::client);
    collect_interfaces(dlls::engine2);
    collect_interfaces(dlls::schemasystem);
    collect_interfaces(dlls::input_system);
    collect_interfaces(dlls::localize);

    get_cached_interface(cvar, XOR("VEngineCvar007"));
    get_cached_interface(client, XOR("Source2Client002"));
    get_cached_interface(engine, XOR("Source2EngineToClient001"));
    get_cached_interface(game_resource, XOR("GameResourceServiceClientV001"));
    get_cached_interface(schema_system, XOR("SchemaSystem_001"));
    get_cached_interface(input_stack_system, XOR("InputStackSystemVersion001"));
    get_cached_interface(localize, XOR("Localize_001"));

    entity_list.initialize<true>(game_resource->get_entity_list());
    csgo_input.initialize<true>(se::csgo_input::get());
    client_mode.initialize<true>(se::client_mode::get());
    view_render.initialize<true>(se::view_render::get());

    LOG_INFO(XOR("Interfaces initialized."));
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
        LOG_INFO(XOR("{}: found interface: {}"), dll.name, cur->name);
        interfaces::list.push_back(std::make_pair(cur->name, cur->create_fn()));
    }
}

template<class ty>
static void get_cached_interface(interface_holder<ty*>& ptr, std::string_view version_string) noexcept
{
    for (const auto& [name, iface] : interfaces::list) {
        if (name.starts_with(version_string.data())) {
            ptr.template initialize<false>(static_cast<ty*>(iface));
            return;
        }
    }
}
