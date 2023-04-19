#pragma once
#include "../base/debug.h"
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <string_view>
#include <functional>

// this is our debug overlay, not to be confused with the debug overlay in the game
namespace debug_overlay {

class debug_section {
public:
    std::mutex mutex { };
    std::string log { };

public:
    debug_section() noexcept = default;
    debug_section(const debug_section &) = delete;
    debug_section &operator=(const debug_section &) = delete;

    void render() noexcept;
    inline void reset() noexcept {
        log.clear();
    }
};

inline std::unordered_map<std::string_view, debug_section> sections { };
inline std::shared_mutex sections_mtx { };

class section_accessor {
    debug_section &section;
    std::lock_guard<std::mutex> lock;
    std::shared_lock<std::shared_mutex> render_lock;

public:
    section_accessor(debug_section &section) noexcept : section(section), lock(section.mutex), render_lock(sections_mtx) {
        section.reset();
    }
    section_accessor(const section_accessor &) = delete;
    section_accessor &operator=(const section_accessor &) = delete;

    template<class... va_args>
    void log(std::string_view fmt, va_args&&... args) const noexcept {
        std::string str;
        if constexpr (sizeof...(args) > 0)
            str = std::vformat(fmt, std::make_format_args(std::forward<decltype(args)>(args)...));
        else
            str = fmt;

        str += '\n';

        section.log.append(str);
    }
};

using SectionFunction = std::function<void(const section_accessor&)>;

#ifdef _DEBUG
inline void section(const std::string_view name, const SectionFunction fn) noexcept {
    const auto section = section_accessor(sections[name]);
    fn(section);
}

void render() noexcept;

#define DEBUG_SECTION(name, fn) debug_overlay::section(name, fn)
#else
#define DEBUG_SECTION(name, fn)
inline void render() noexcept {};
#endif

}