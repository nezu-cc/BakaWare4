#pragma once
#include "../base/base.h"
#include <imgui.h>

namespace ImGui
{
    bool ColorEdit4(const char* label, uint32_t* col, ImGuiColorEditFlags flags = 0) {
        float col_f[4] = {
            ((*col >> 0 ) & 0xFF) / 255.0f,
            ((*col >> 8 ) & 0xFF) / 255.0f,
            ((*col >> 16) & 0xFF) / 255.0f,
            ((*col >> 24) & 0xFF) / 255.0f
        };
        if (ColorEdit4(label, col_f, flags)) {
            *col = (((uint8_t)(col_f[3] * 255.0f)) << 24) |
                (((uint8_t)(col_f[2] * 255.0f)) << 16) |
                (((uint8_t)(col_f[1] * 255.0f)) << 8) |
                (((uint8_t)(col_f[0] * 255.0f)) << 0);
            return true;
        }
        return false;
    }

    bool ColorEdit4(const char* label, clr4* col, ImGuiColorEditFlags flags = 0) {
        return ColorEdit4(label, &col->rgba, flags);
    }
}