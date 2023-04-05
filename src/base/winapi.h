#pragma once

#include "base.h"
#include <filesystem>
namespace fs = std::filesystem;
#include <string_view>
#include <Windows.h>
#include <TlHelp32.h>

struct dll;

namespace win {

    // Only usable for handles closed via CloseHandle()!
    struct scoped_handle {
        HANDLE handle{ };

        scoped_handle(HANDLE handle) noexcept
            : handle(handle) { }
        ~scoped_handle()
        {
            if (handle)
                CloseHandle(handle);
        }

        operator bool() noexcept
        {
            return handle != nullptr && handle != INVALID_HANDLE_VALUE;
        }

        operator HANDLE() noexcept
        {
            return handle;
        }
    };

}
