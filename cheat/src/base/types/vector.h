#pragma once
#include <imgui.h>

struct vec2 {
    float x{ }, y{ };

    constexpr vec2() noexcept = default;
    constexpr vec2(float x, float y) noexcept
        : x(x), y(y) { }
    constexpr vec2(ImVec2 v) noexcept
        : x(v.x), y(v.y) { }
};

struct vec3 {
    float x{ }, y{ }, z{ };

    constexpr vec3() noexcept = default;
    constexpr vec3(float x, float y, float z) noexcept
        : x(x), y(y), z(z) { }

    bool operator==(const vec3& rhs) const noexcept
    {
        return (std::abs(x - rhs.x) <= std::numeric_limits<float>::epsilon() &&
            std::abs(y - rhs.y) <= std::numeric_limits<float>::epsilon() &&
            std::abs(z - rhs.z) <= std::numeric_limits<float>::epsilon());
    }

    bool operator!=(const vec3& rhs) const noexcept
    {
        return (std::abs(x - rhs.x) > std::numeric_limits<float>::epsilon() ||
            std::abs(y - rhs.y) > std::numeric_limits<float>::epsilon() ||
            std::abs(z - rhs.z) > std::numeric_limits<float>::epsilon());
    }

    constexpr vec3 operator+(const vec3& rhs) const noexcept
    {
        return vec3(x + rhs.x, y + rhs.y, z + rhs.z);
    }

    constexpr vec3 operator-(const vec3& rhs) const noexcept
    {
        return vec3(x - rhs.x, y - rhs.y, z - rhs.z);
    }

    constexpr vec3 operator*(const vec3& rhs) const noexcept
    {
        return vec3(x * rhs.x, y * rhs.y, z * rhs.z);
    }

    constexpr vec3 operator/(const vec3& rhs) const noexcept
    {
        return vec3(x / rhs.x, y / rhs.y, z / rhs.z);
    }

    constexpr vec3 operator+(const float rhs) const noexcept
    {
        return vec3(x + rhs, y + rhs, z + rhs);
    }

    constexpr vec3 operator-(const float rhs) const noexcept
    {
        return vec3(x - rhs, y - rhs, z - rhs);
    }

    constexpr vec3 operator*(const float rhs) const noexcept
    {
        return vec3(x * rhs, y * rhs, z * rhs);
    }

    constexpr vec3 operator/(const float rhs) const noexcept
    {
        return vec3(x / rhs, y / rhs, z / rhs);
    }

    constexpr vec3& operator=(const vec3& rhs) noexcept
    {
        x = rhs.x;
        y = rhs.y;
        z = rhs.z;
        return *this;
    }

    constexpr vec3& operator+=(const vec3& rhs) noexcept
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    constexpr vec3& operator-=(const vec3& rhs) noexcept
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    constexpr vec3& operator*=(const vec3& rhs) noexcept
    {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        return *this;
    }

    constexpr vec3& operator/=(const vec3& rhs) noexcept
    {
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        return *this;
    }

    constexpr vec3& operator+=(const float rhs) noexcept
    {
        x += rhs;
        y += rhs;
        z += rhs;
        return *this;
    }

    constexpr vec3& operator-=(const float rhs) noexcept
    {
        x -= rhs;
        y -= rhs;
        z -= rhs;
        return *this;
    }

    constexpr vec3& operator*=(const float rhs) noexcept
    {
        x *= rhs;
        y *= rhs;
        z *= rhs;
        return *this;
    }

    constexpr vec3& operator/=(const float rhs) noexcept
    {
        x /= rhs;
        y /= rhs;
        z /= rhs;
        return *this;
    }

    constexpr void inverse() noexcept
    {
        x = -x;
        y = -y;
        z = -z;
    }

    constexpr void clear() noexcept
    {
        x = { };
        y = { };
        z = { };
    }

    float length_sqr() const noexcept
    {
        return dot_product(*this);
    }

    float length() const noexcept
    {
        return sqrtf(length_sqr());
    }

    float length2d_sqr() const noexcept
    {
        return (x * x + y * y);
    }

    float length2d() const noexcept
    {
        return sqrtf(length2d_sqr());
    }

    float distance_to(const vec3& other) const noexcept
    {
        return (*this - other).length();
    }

    float dot_product(const vec3& v) const noexcept
    {
        return (x * v.x + y * v.y + z * v.z);
    }

    float dot_product_abs(const vec3& v) const noexcept
    {
        return (std::abs(x * v.x) + std::abs(y * v.y) + std::abs(z * v.z));
    }

    vec3 cross_product(const vec3& v) const noexcept
    {
        return vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }

    vec3 transform(const mat3x4& mat) noexcept
    {
        return vec3(dot_product({ mat[0][0], mat[0][1], mat[0][2] }) + mat[0][3],
          dot_product({ mat[1][0], mat[1][1], mat[1][2] }) + mat[1][3],
          dot_product({ mat[2][0], mat[2][1], mat[2][2] }) + mat[2][3]);
    }

    void normalize() noexcept
    {
        *this /= length();
    }

    auto to_vec2() const noexcept
    {
        return vec2(x, y);
    }
};

struct vec4 {
    float x{ }, y{ }, z{ }, w{ };
};

#include <format>

template<>
struct std::formatter<vec2> : std::formatter<std::string> {
    auto format(vec2 v, format_context& ctx)
    {
        return formatter<string>::format(std::format("[{}, {}]", v.x, v.y), ctx);
    }
};

template<>
struct std::formatter<vec3> : std::formatter<std::string> {
    auto format(vec3 v, format_context& ctx)
    {
        return formatter<string>::format(std::format("[{}, {}, {}]", v.x, v.y, v.z), ctx);
    }
};

template<>
struct std::formatter<vec4> : std::formatter<std::string> {
    auto format(vec4 v, format_context& ctx)
    {
        return formatter<string>::format(std::format("[{}, {}, {}, {}]", v.x, v.y, v.z, v.w), ctx);
    }
};
