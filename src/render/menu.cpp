#include "menu.h"
#include "../core/cheat.h"
#include <imgui.h>

void menu::render() noexcept
{
    if (!is_open)
        return;

    ImGui::ShowDemoWindow();
    
    if (ImGui::Button("Unhook"))
        cheat::should_unhook = true;
}
