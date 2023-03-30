#pragma once

struct d2 {
    int x{ }, y{ };

    constexpr d2() noexcept = default;
    constexpr d2(int x, int y) noexcept
        : x(x), y(y) { }

    constexpr bool operator==(d2 rhs) const noexcept
    {
        return x == rhs.x && y == rhs.y;
    }

    constexpr bool operator!=(d2 rhs) const noexcept
    {
        return x != rhs.x || y != rhs.y;
    }

    constexpr d2 operator+(d2 rhs) const noexcept
    {
        return d2(x + rhs.x, y + rhs.y);
    }

    constexpr d2 operator-(d2 rhs) const noexcept
    {
        return d2(x - rhs.x, y - rhs.y);
    }

    constexpr d2 operator*(d2 rhs) const noexcept
    {
        return d2(x * rhs.x, y * rhs.y);
    }

    constexpr d2 operator/(d2 rhs) const noexcept
    {
        return d2(x / rhs.x, y / rhs.y);
    }

    constexpr d2 operator+(const int rhs) const noexcept
    {
        return d2(x + rhs, y + rhs);
    }

    constexpr d2 operator-(const int rhs) const noexcept
    {
        return d2(x - rhs, y - rhs);
    }

    constexpr d2 operator*(const int rhs) const noexcept
    {
        return d2(x * rhs, y * rhs);
    }

    constexpr d2 operator/(const int rhs) const noexcept
    {
        return d2(x / rhs, y / rhs);
    }

    constexpr d2& operator+=(d2 rhs) noexcept
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    constexpr d2& operator-=(d2 rhs) noexcept
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    constexpr d2& operator*=(d2 rhs) noexcept
    {
        x *= rhs.x;
        y *= rhs.y;
        return *this;
    }

    constexpr d2& operator/=(d2 rhs) noexcept
    {
        x /= rhs.x;
        y /= rhs.y;
        return *this;
    }

    constexpr d2& operator+=(int rhs) noexcept
    {
        x += rhs;
        y += rhs;
        return *this;
    }

    constexpr d2& operator-=(int rhs) noexcept
    {
        x -= rhs;
        y -= rhs;
        return *this;
    }

    constexpr d2& operator*=(int rhs) noexcept
    {
        x *= rhs;
        y *= rhs;
        return *this;
    }

    constexpr d2& operator/=(int rhs) noexcept
    {
        x /= rhs;
        y /= rhs;
        return *this;
    }
};
