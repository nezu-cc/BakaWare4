#pragma once
#include <string_view>

namespace crash_handler {

    void initialize(const uintptr_t base) noexcept;
    void end() noexcept;
    void handle_crash(std::string_view msg) noexcept;

}