#pragma once

struct clr3;

struct clr4 {
    union {
        struct {
            uint8_t r, g, b, a;
        };
        uint32_t rgba{ };
    };

    constexpr clr4() noexcept = default;

    template<std::integral ix, std::integral ax>
    constexpr clr4(ix r, ix g, ix b, ax a) noexcept
        : r(r), g(g), b(b), a(a) { }

    template<std::floating_point fp>
    constexpr clr4(fp r, fp g, fp b, fp a) noexcept
        : r(static_cast<uint8_t>(r * 255)), g(static_cast<uint8_t>(g * 255)),
          b(static_cast<uint8_t>(b * 255)), a(static_cast<uint8_t>(a * 255)) { }

    float r_base() const noexcept { return r / 255.f; }
    float g_base() const noexcept { return g / 255.f; }
    float b_base() const noexcept { return b / 255.f; }
    float a_base() const noexcept { return a / 255.f; }

    static clr4 white(uint8_t a = 255) noexcept { return clr4(255, 255, 255, a); }
    static clr4 black(uint8_t a = 255) noexcept { return clr4(0, 0, 0, a); }
    static clr4 red(uint8_t a = 255) noexcept { return clr4(255, 0, 0, a); }
    static clr4 green(uint8_t a = 255) noexcept { return clr4(0, 255, 0, a); }
    static clr4 blue(uint8_t a = 255) noexcept { return clr4(0, 0, 255, a); }

    static clr4 lerp(const clr4& a, const clr4& b, float t) noexcept {
        return clr4(
            std::lerp(a.r_base(), b.r_base(), t),
            std::lerp(a.g_base(), b.g_base(), t),
            std::lerp(a.b_base(), b.b_base(), t),
            std::lerp(a.a_base(), b.a_base(), t)
        );
    }
};

static_assert(sizeof(clr4) == 4);

struct clr3 {
    uint8_t r{ }, g{ }, b{ };

    constexpr clr3() noexcept = default;
    constexpr clr3(uint8_t r, uint8_t g, uint8_t b) noexcept
        : r(r), g(g), b(b) { }

    auto to_clr4(uint8_t alpha = 255) const noexcept
    {
        return clr4(r, g, b, alpha);
    }
};
