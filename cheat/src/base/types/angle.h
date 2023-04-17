#pragma once

#include <limits>

struct angle {
    float x{ }, y{ }, z{ };

    constexpr angle() noexcept = default;
    constexpr angle(float x, float y, float z) noexcept
        : x(x), y(y), z(z) { }

    bool operator==(const angle& rhs) const noexcept
    {
        return (std::abs(x - rhs.x) <= std::numeric_limits<float>::epsilon() &&
            std::abs(y - rhs.y) <= std::numeric_limits<float>::epsilon() &&
            std::abs(z - rhs.z) <= std::numeric_limits<float>::epsilon());
    }

    bool operator!=(const angle& rhs) const noexcept
    {
        return (std::abs(x - rhs.x) > std::numeric_limits<float>::epsilon() ||
            std::abs(y - rhs.y) > std::numeric_limits<float>::epsilon() ||
            std::abs(z - rhs.z) > std::numeric_limits<float>::epsilon());
    }

    constexpr angle operator+(const angle& rhs) const noexcept
    {
        return angle(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    constexpr angle operator-(const angle& rhs) const noexcept
    {
        return angle(x - rhs.x, y - rhs.y, z - rhs.z);
    }

    constexpr angle operator*(const angle& rhs) const noexcept
    {
        return angle(x * rhs.x, y * rhs.y, z * rhs.z);
    }

    constexpr angle operator/(const angle& rhs) const noexcept
    {
        return angle(x / rhs.x, y / rhs.y, z / rhs.z);
    }

    constexpr angle operator+(const float rhs) const noexcept
    {
        return angle(x + rhs, y + rhs, z + rhs);
    }

    constexpr angle operator-(const float rhs) const noexcept
    {
        return angle(x - rhs, y - rhs, z - rhs);
    }

    constexpr angle operator*(const float rhs) const noexcept
    {
        return angle(x * rhs, y * rhs, z * rhs);
    }

    constexpr angle operator/(const float rhs) const noexcept
    {
        return angle(x / rhs, y / rhs, z / rhs);
    }

    constexpr angle& operator=(const angle& rhs) noexcept
    {
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        return *this;
    }

    constexpr angle& operator+=(const angle& rhs) noexcept
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    constexpr angle& operator-=(const angle& rhs) noexcept
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    constexpr angle& operator*=(const angle& rhs) noexcept
    {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        return *this;
    }

    constexpr angle& operator/=(const angle& rhs) noexcept
    {
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        return *this;
    }

    constexpr angle& operator+=(const float rhs) noexcept
    {
        x += rhs;
        y += rhs;
        z += rhs;
        return *this;
    }

    constexpr angle& operator-=(const float rhs) noexcept
    {
        x -= rhs;
        y -= rhs;
        z -= rhs;
        return *this;
    }

    constexpr angle& operator*=(const float rhs) noexcept
    {
        x *= rhs;
        y *= rhs;
        z *= rhs;
        return *this;
    }

    constexpr angle& operator/=(const float rhs) noexcept
    {
        x /= rhs;
        y /= rhs;
        z /= rhs;
        return *this;
    }

    constexpr void clamp() noexcept
    {
        x = std::clamp(x, -89.0f, 89.0f);
        y = std::clamp(y, -180.0f, 180.0f);
        z = 0.0f;
    }

    constexpr auto length_sqr() const noexcept
    {
        return (x * x + y * y + z * z);
    }

    constexpr auto length2d_sqr() const noexcept
    {
        return (x * x + y * y);
    }

    auto length() const noexcept
    {
        return sqrtf(length_sqr());
    }

    auto length2d() const noexcept
    {
        return sqrtf(length2d_sqr());
    }

    void normalize() noexcept
    {
        x = std::isfinite(x) ? std::remainder(x, 360.0f) : 0.0f;
        y = std::isfinite(y) ? std::remainder(y, 360.0f) : 0.0f;
        z = 0.0f;
    }
};

#include <format>

template<>
struct std::formatter<angle> : std::formatter<std::string> {
    auto format(angle a, format_context& ctx)
    {
        return formatter<string>::format(std::format("[{:.02f}, {:.02f}, {:.02f}]", a.x, a.y, a.z), ctx);
    }
};
