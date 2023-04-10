#include "dll.h"

address dll::get_export(hash_t hash) const noexcept
{
    const auto nt_hdrs = get_nt_headers();
    if (!nt_hdrs)
        return address();

    const auto optional_hdr = &nt_hdrs->OptionalHeader;
    const auto dir_addr = reinterpret_cast<IMAGE_EXPORT_DIRECTORY*>(
        base + optional_hdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
    const auto functions = reinterpret_cast<uint32_t*>(base + dir_addr->AddressOfFunctions);
    const auto names = reinterpret_cast<uint32_t*>(base + dir_addr->AddressOfNames);
    const auto ordinals = reinterpret_cast<uint16_t*>(base + dir_addr->AddressOfNameOrdinals);

    for (DWORD i{ }; i < dir_addr->NumberOfFunctions; i++) {
        const auto name = reinterpret_cast<const char*>(base + names[i]);
        if (fnv1a::hash(name) == hash)
            return address(base + functions[ordinals[i]]);
    }

    return address();
}

address dll::get_import(const dll& from, hash_t hash) const noexcept
{
    const auto nt_hdrs = get_nt_headers();
    if (!nt_hdrs)
        return address();

    const auto optional_hdr = &nt_hdrs->OptionalHeader;
    auto import_desc = reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(
        base + optional_hdr->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

    while (import_desc->Name) {
        auto cur_dll = reinterpret_cast<char*>(base) + import_desc->Name;
        if (!from.name.compare(cur_dll)) {
            auto thunk_data = reinterpret_cast<IMAGE_THUNK_DATA*>(base + import_desc->OriginalFirstThunk);
            for (int i = 0; thunk_data->u1.Function; i++, thunk_data++) {
                char* cur_fn = reinterpret_cast<IMAGE_IMPORT_BY_NAME*>(base + thunk_data->u1.AddressOfData)->Name;
                if (fnv1a::hash(cur_fn) == hash) {
                    auto table = reinterpret_cast<uintptr_t*>(base + import_desc->FirstThunk);
                    return address(table[i]);
                }
            }
            return address();
        }
        import_desc++;
    }

    return address();
}

IMAGE_SECTION_HEADER* dll::get_section(hash_t hash) const noexcept
{
    const auto nt_hdrs = get_nt_headers();
    if (!nt_hdrs)
        return nullptr;

    auto section = IMAGE_FIRST_SECTION(nt_hdrs);
    if (!section)
        return nullptr;

    for (WORD i{ }; i <= nt_hdrs->FileHeader.NumberOfSections; i++, section++) {
        const auto name = reinterpret_cast<const char*>(section->Name);
        if (fnv1a::hash(name) == hash)
            return section;
    }

    return nullptr;
}

bool dll::is_within_section(address addr, hash_t section_hash) const noexcept
{
    auto section = get_section(section_hash);
    if (!section)
        return false;

    return is_within_section(addr, section);
}

bool dll::is_within_section(address addr, IMAGE_SECTION_HEADER* section) const noexcept
{
    if (!section)
        return false;

    const auto start = base + section->VirtualAddress;
    const auto end = start + section->SizeOfRawData;

    return addr.value >= start && addr.value < end;
}

void dlls::add_to_trusted_list(uintptr_t base) noexcept
{
    SIG(data_ptr, dlls::client, "48 8B 05 ? ? ? ? 49 8B F8 8B F2")
    auto data = *data_ptr.absolute<return_address_data**>(3);

    auto dos = reinterpret_cast<IMAGE_DOS_HEADER*>(base);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE) {
        LOG_ERROR(XOR("add_to_trusted_list: invalid DOS header"));
        return;
    }

    auto nt = reinterpret_cast<IMAGE_NT_HEADERS*>(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE) {
        LOG_ERROR(XOR("add_to_trusted_list: invalid NT header"));
        return;
    }

    const auto index = data->whitelist.size / 2;
    if (index >= std::size(data->whitelist.modules)) {
        LOG_ERROR(XOR("add_to_trusted_list: whitelist is full"));
        return;
    }

    auto& module_data = data->whitelist.modules[index];
    module_data.start = base + nt->OptionalHeader.BaseOfCode;
    // yes, this will go past the end of the image, this is how valve does it as well /shrug
    module_data.end = module_data.start + nt->OptionalHeader.SizeOfImage;
    data->whitelist.size += 2;

    LOG_INFO(XOR("add_to_trusted_list: added {}-{} idx: {} to whitelist"), (void*)module_data.start, (void*)module_data.end, index);
}