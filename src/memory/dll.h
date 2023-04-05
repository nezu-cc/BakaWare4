#pragma once

#include <unordered_map>
#include <Windows.h>
#include <winternl.h>

#include "../base/base.h"
#include "../crypt/fnv1a.h"
#include "memory.h"

struct dll;
namespace dlls { inline std::vector<dll*> list; }

struct dll {
    std::string name{ };
    uintptr_t base{ };
    size_t size{ };
    address create_interface{ }; // Only relevant to game DLLs

    explicit dll(std::string_view name) noexcept
        : name(name)
    {
        dlls::list.push_back(this);
    }

    template<size_t len> requires(len > 0)
    address find(std::array<int, len>&& pattern) const noexcept
    {
        auto bytes = reinterpret_cast<uint8_t*>(base);
        for (size_t i{ }; i < size - len; i++) {
            for (size_t j{ }; j < len; j++) {
                if (bytes[i + j] != pattern[j] && pattern[j] != -1)
                    break;
                if (j + 1 == len)
                    return address(&bytes[i]);
            }
        }

        LOG_ERROR("{}: did not find pattern {}", name, [&]()
        {
            /* Slow, but this only runs when something goes wrong. */
            std::stringstream ss{ };
            for (auto byte : pattern) {
                if (byte == -1)
                    ss << '?';
                else
                    ss << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << byte;
                ss << ' ';
            }
            return ss.str();
        }());
        return address();
    }

    inline IMAGE_NT_HEADERS* get_nt_headers() const noexcept
    {
        auto dos = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
        if (dos->e_magic != IMAGE_DOS_SIGNATURE)
            return nullptr;

        auto nt = reinterpret_cast<IMAGE_NT_HEADERS*>(base + dos->e_lfanew);
        if (nt->Signature != IMAGE_NT_SIGNATURE)
            return nullptr;

        return nt;
    }

    address get_import(const dll& from, hash_t hash) const noexcept;
    address get_export(hash_t hash) const noexcept;
    IMAGE_SECTION_HEADER* get_section(hash_t hash) const noexcept;
    bool is_within_section(address addr, hash_t section_hash) const noexcept;
    bool is_within_section(address addr, IMAGE_SECTION_HEADER* section) const noexcept;

};

namespace dlls {

    inline dll cs2{ "cs2.exe" };
    inline dll sdl2{ "SDL2.dll" };
    inline dll game_overlay_renderer64{ "gameoverlayrenderer64.dll" };
    inline dll render_system_dx11{ "rendersystemdx11.dll" };
    inline dll client{ "client.dll" };
    inline dll engine2{ "engine2.dll" };
    inline dll schemasystem{ "schemasystem.dll" };
    inline dll input_system{ "inputsystem.dll" };

    inline void initialize() noexcept
    {
        struct LDR_DATA_TABLE_ENTRY {
            LIST_ENTRY InLoadOrderLinks;
            LIST_ENTRY InMemoryOrderLinks;
            LIST_ENTRY InInitializationOrderLinks;
            PVOID DllBase;
            PVOID EntryPoint;
            ULONG SizeOfImage;
            UNICODE_STRING FullDllName;
            UNICODE_STRING BaseDllName;
        };

        std::unordered_map<hash_t, LDR_DATA_TABLE_ENTRY*> loaded{ };

        const auto peb = reinterpret_cast<const PEB*>(__readgsqword(0x60));
        auto cur = CONTAINING_RECORD(peb->Ldr->InMemoryOrderModuleList.Flink,
                                     LDR_DATA_TABLE_ENTRY,
                                     InMemoryOrderLinks);

        while (cur->BaseDllName.Length) {
            loaded.insert_or_assign(fnv1a::hash(cur->BaseDllName.Buffer), cur);
            LOG_INFO(L"Found module {} at {}", cur->BaseDllName.Buffer, cur->DllBase);
            cur = reinterpret_cast<LDR_DATA_TABLE_ENTRY*>(cur->InLoadOrderLinks.Flink);
        }

        for (auto entry : list) {
            const auto res = loaded.find(fnv1a::hash(entry->name));
            ASSERT_MSG(res != loaded.cend(), "DLL not loaded yet?");
            const auto dll = res->second;
            entry->base = reinterpret_cast<uintptr_t>(dll->DllBase);
            entry->size = dll->SizeOfImage;
        }
    }

};
