#pragma once

using mat3x3 = float[3][3];

struct mat3x4 {
    float data[3][4]{ };

    float* operator[](int i) noexcept
    {
        return data[i];
    }

    const float* operator[](int i) const noexcept
    {
        return data[i];
    }
};

struct mat4x4 {
    float data[4][4]{ };

    float* operator[](int i) noexcept
    {
        return data[i];
    }

    const float* operator[](int i) const noexcept
    {
        return data[i];
    }
};
