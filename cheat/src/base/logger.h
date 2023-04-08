#pragma once

#include <chrono>
namespace ch = std::chrono;
#include <format>
#include <fstream>
#include <iostream>
#include <string_view>

#include "winapi.h"

#ifdef _DEBUG
#define LOG_ENABLED 
#endif

#ifdef LOG_ENABLED
#define LOG_SUCCESS(fmt, ...) logger::add<logger::level::success>(fmt, __VA_ARGS__)
#define LOG_INFO(fmt, ...) logger::add<logger::level::info>(fmt, __VA_ARGS__)
#define LOG_ERROR(fmt, ...) logger::add<logger::level::error>(fmt, __VA_ARGS__)
#define LOG_RAW(fmt, ...) logger::add<logger::level::raw>(fmt, __VA_ARGS__)
#else
#define LOG_SUCCESS
#define LOG_INFO
#define LOG_ERROR
#define LOG_RAW
#endif

namespace logger {

    inline HANDLE console{ };

    enum class level {
        success,
        info,
        error,
        raw
    };

    // Use the LOG macros instead of accessing these directly

    template<level lvl, class... va_args>
    void add(std::string_view fmt, va_args&&... args) noexcept
    {
    #ifdef LOG_ENABLED
        if constexpr (lvl != level::raw) {
            std::cout << "[ ";
            switch (lvl) {
            case level::success:
                SetConsoleTextAttribute(console, FOREGROUND_GREEN);
                std::cout << '+';
                break;
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
        }

        if constexpr (sizeof...(args) > 0) {
            const auto& str = std::vformat(fmt, std::make_format_args(std::forward<decltype(args)>(args)...));
            std::cout << str << '\n';
        } else {
            std::cout << fmt << '\n';
        }
    #endif
    }

    template<level lvl, class... va_args>
    void add(std::wstring_view fmt, va_args&&... args) noexcept
    {
    #ifdef LOG_ENABLED
        if constexpr (lvl != level::raw) {
            std::wcout << L"[ ";
            switch (lvl) {
            case level::success:
                SetConsoleTextAttribute(console, FOREGROUND_GREEN);
                std::wcout << L'+';
                break;
            case level::info:
                SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
                std::wcout << L'*';
                break;
            case level::error:
                SetConsoleTextAttribute(console, FOREGROUND_RED);
                std::wcout << L'!';
                break;
            }

            SetConsoleTextAttribute(console, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
            const auto time = ch::system_clock::to_time_t(ch::system_clock::now());
            std::tm tm{ };
            localtime_s(&tm, &time);
            std::wcout << L" ] " << std::put_time(&tm, L"[%T] ");
        }

        if constexpr (sizeof...(args) > 0) {
            const auto& str = std::vformat(fmt, std::make_wformat_args(std::forward<decltype(args)>(args)...));
            std::wcout << str << '\n';
        } else {
            std::wcout << fmt << '\n';
        }
    #endif
    }

    inline void initialize(std::wstring_view console_title) noexcept
    {
    #ifdef LOG_ENABLED
        AllocConsole();
        AttachConsole(ATTACH_PARENT_PROCESS);
        SetConsoleTitleW(console_title.data());

        freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
        console = GetStdHandle(STD_OUTPUT_HANDLE);

        LOG_INFO("Logger initialized.");
    #endif
    }

    inline void end() noexcept
    {
    #ifdef LOG_ENABLED
        fclose(stdout);
        FreeConsole();
    #endif
    }

}
