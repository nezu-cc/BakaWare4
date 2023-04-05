#include "menu.h"
#include "../core/cheat.h"
#include "../valve/sdl/sdl.h"
#include <imgui.h>

void menu::render() noexcept {
    if (!is_open)
        return;

    ImGui::ShowDemoWindow();

    ImGui::Text("w: %d, h: %d", cheat::screen_size.x, cheat::screen_size.y);
    
    if (ImGui::Button("Unhook"))
        cheat::should_unhook = true;

    if (ImGui::Button("Crash"))
        ASSERT_MSG(false, "Crash button pressed!");
}
