#pragma once

#include <chrono>
namespace ch = std::chrono;
#include <format>
#include <fstream>
#include <iostream>
#include <string_view>

#include "winapi.h"
#include "types/color.h"

// #define EXTERNAL_CONSOLE 

// #ifdef EXTERNAL_CONSOLE
#define LOG_INFO(fmt, ...) logger::add<logger::level::info>(fmt, __VA_ARGS__)
#define LOG_ERROR(fmt, ...) logger::add<logger::level::error>(fmt, __VA_ARGS__)
// #else
// #define LOG_INFO
// #define LOG_ERROR
// #endif

namespace logger {

    inline HANDLE console{ };

    enum class level {
        info,
        error,
    };

    namespace valve_logger {
        typedef int logging_channel_id;
        enum class logging_severity {
            LS_MESSAGE = 0,
            LS_WARNING = 1,
            LS_ASSERT = 2,
            LS_ERROR = 3,
            LS_HIGHEST_SEVERITY = 4,
        };

        inline logging_channel_id channel_id;

        void initialize(const char* channel_name, logging_severity severity = logging_severity::LS_HIGHEST_SEVERITY, clr4 color = clr4(0x00, 0x77, 0xFF, 0xFF)) noexcept;

        void log(logging_severity severity, const char* message) noexcept;

        bool is_channel_enabled(logging_severity severity) noexcept;
    };

    // Use the LOG macros instead of accessing these directly

    template<level lvl, class... va_args>
    void add(std::string_view fmt, va_args&&... args) noexcept
    {
        std::string str;
        if constexpr (sizeof...(args) > 0)
            str = std::vformat(fmt, std::make_format_args(std::forward<decltype(args)>(args)...));
        else
            str = fmt;

        str += '\n';
    #ifdef EXTERNAL_CONSOLE
        std::cout << "[ ";
        switch (lvl) {
        case level::info:
            SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
            std::cout << '*';
            break;
        case level::error:
            SetConsoleTextAttribute(console, FOREGROUND_RED);
            std::cout << '!';
            break;
        }

        SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        const auto time = ch::system_clock::to_time_t(ch::system_clock::now());
        std::tm tm{ };
        localtime_s(&tm, &time);
        std::cout << " ] " << std::put_time(&tm, "[%T] ");

        std::cout << fmt;
    #else
        switch (lvl) {
        case level::info:
            valve_logger::log(valve_logger::logging_severity::LS_MESSAGE, str.data());
            break;
        case level::error:
            valve_logger::log(valve_logger::logging_severity::LS_ERROR, str.data());
            break;
        }
    #endif
    }

    inline void initialize(std::string_view console_title) noexcept
    {
    #ifdef EXTERNAL_CONSOLE
        AllocConsole();
        AttachConsole(ATTACH_PARENT_PROCESS);
        SetConsoleTitleA(console_title.data());

        freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
        console = GetStdHandle(STD_OUTPUT_HANDLE);
    #else
        valve_logger::initialize(console_title.data());
    #endif
        LOG_INFO("Logger initialized.");
    }

    inline void end() noexcept
    {
    #ifdef EXTERNAL_CONSOLE
        fclose(stdout);
        FreeConsole();
    #endif
    }

}
