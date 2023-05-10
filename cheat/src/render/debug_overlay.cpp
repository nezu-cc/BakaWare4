#include "debug_overlay.h"
#include <imgui.h>

#ifdef _DEBUG

void debug_overlay::render(render::renderer* r) noexcept {
    std::unique_lock lock(sections_mtx);
    if (sections.empty())
        return;

    for (auto &[name, section] : sections)
        section.render(r);
    
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

void debug_overlay::w2s() noexcept {
    std::unique_lock lock(sections_mtx);
    for (auto &[name, section] : sections)
        section.w2s();
}

#endif
