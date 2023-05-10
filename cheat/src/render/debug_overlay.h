#pragma once
#include "../base/base.h"
#include "../base/math.h"
#include "../render/render.h"
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <string_view>
#include <functional>

// this is our debug overlay, not to be confused with the debug overlay in the game
namespace debug_overlay {

struct debug_line {
    vec3 start = { }, end = { };
    clr4 color = { };
    vec2 start_w2s { }, end_w2s { };
    bool is_visible { };
    bool is_2d { };
};

class debug_section {
public:
    std::mutex mutex { };
    std::string log { };
    std::vector<debug_line> lines { };

public:
    debug_section() noexcept = default;
    debug_section(const debug_section &) = delete;
    debug_section &operator=(const debug_section &) = delete;

    inline void render(render::renderer* r) noexcept {
        for (const auto &line : lines) {
            if (!line.is_visible)
                continue;
            r->line(line.start_w2s.x, line.start_w2s.y, line.end_w2s.x, line.end_w2s.y, line.color);
        }
    }

    inline void w2s() noexcept {
        for (auto &line : lines)
            if (!line.is_2d)
                line.is_visible = math::world_to_screen(line.start, line.start_w2s) && math::world_to_screen(line.end, line.end_w2s);
    }

    inline void reset() noexcept {
        log.clear();
        lines.clear();
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

    void line(const vec3 &start, const vec3 &end, const clr4 &color) const noexcept {
        section.lines.emplace_back(debug_line{ start, end, color, vec2{ }, vec2{ }, false, false });
    }

    void line2d(const vec2 &start, const vec2 &end, const clr4 &color) const noexcept {
        section.lines.emplace_back(debug_line{ vec3{ }, vec3{ }, color, start, end, true, true });
    }
};

using SectionFunction = std::function<void(const section_accessor&)>;

#ifdef _DEBUG
inline void section(const std::string_view name, const SectionFunction fn) noexcept {
    const auto section = section_accessor(sections[name]);
    fn(section);
}

void render(render::renderer* r) noexcept;
void w2s() noexcept;

#define DEBUG_SECTION(name, fn) debug_overlay::section(name, fn)
#else
#define DEBUG_SECTION(name, fn)
inline void render(render::renderer* r) noexcept {};
inline void w2s() noexcept {};
#endif

}