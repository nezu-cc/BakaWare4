#include "debug_overlay.h"
#include <imgui.h>

#ifdef _DEBUG

void debug_overlay::render() noexcept {
    std::unique_lock lock(sections_mtx);
    if (sections.empty())
        return;
    
    ImGui::SetNextWindowSize(ImVec2(500, 500), ImGuiCond_FirstUseEver);
    if (ImGui::Begin(XOR("Debug Overlay"))){
        for (auto &[name, section] : sections) {
            if (section.log.empty())
                continue;
            ImGui::SeparatorText(name.data());
            ImGui::TextUnformatted(section.log.c_str(), section.log.c_str() + section.log.size());
        }
    }
    ImGui::End();
}

#endif
