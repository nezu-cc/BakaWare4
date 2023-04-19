#include "menu.h"
#include <imgui.h>
#include <imgui_internal.h>
#include "imgui_extra.h"
#include "../core/cheat.h"
#include "../core/config.h"
#include "../core/entity_cache.h"

#define ADD_VERSION(str) (str " " BAKAWARE_VERSION)

void menu::render() noexcept {
    if (!is_open)
        return;

#ifdef _DEBUG
    ImGui::ShowDemoWindow();

    if (ImGui::Button(XOR("Unhook")))
        cheat::should_unhook = true;

    ImGui::SeparatorText(XOR("Debug config"));

    ImGui::Text(XOR("debug_flags: 0x%X %d"), debug_cfg.debug_flags, debug_cfg.debug_flags);
    for (size_t i = 0; i < sizeof(debug_cfg.debug_flags) * CHAR_BIT; i++) {
        auto mask = (1 << i);
        bool val = (debug_cfg.debug_flags & mask) != 0;
        if (i % 8 != 0)
            ImGui::SameLine();
        if (ImGui::Checkbox(std::format(XOR("{:02}"), i + 1).c_str(), &val))
            debug_cfg.debug_flags ^= mask;
    }
    ImGui::SliderInt(XOR("debug_int"), &debug_cfg.debug_int, 0, 100);
    ImGui::SliderFloat(XOR("debug_float"), &debug_cfg.debug_float, 0, 100);
    ImGui::ColorEdit4(XOR("debug_color"), &debug_cfg.debug_color);

    ImGui::SeparatorText(XOR("Debug info"));
    
    ImGui::Text(XOR("Global vars: %p"), cheat::global_vars);
    ImGui::Text(XOR("Local controller: %p"), cheat::local.controller);
    ImGui::Text(XOR("Local player pawn: %p"), cheat::local.pawn);
    ImGui::Text(XOR("highest_entity_index: %d"), interfaces::entity_list->get_highest_entity_index());
    ImGui::Text(XOR("last_mouse_enabled: %d"), input::last_mouse_enabled ? 1 : 0);
    {
        std::shared_lock lock(entity_cache::mutex);
        ImGui::Text(XOR("entity_cache size: %d"), entity_cache::entities.size());
    }
#endif

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(800, 600));
    if (ImGui::Begin(XOR(ADD_VERSION("BakaWare")), 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize)) {
        {
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
            ImGui::Columns(2, 0, true);
            ImGui::SetColumnWidth(-1, 150);
            ImGui::GetCurrentWindow()->DC.CurrentColumns->Flags |= ImGuiColumnsFlags_NoResize;
            const ImVec2 size(ImGui::GetContentRegionAvail().x, 50);
            for (int i = 0; i < tabs.size(); i++) {
                auto& tab = tabs[i];
                if (tab->render_button(size, i == selected_tab))
                    selected_tab = i;
            }
            ImGui::PopStyleVar();

            // TODO: configs

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			ImGui::NextColumn();
			ImGui::PopStyleVar();
        }
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
        if (ImGui::BeginChild(XOR("##settings"), ImVec2(), true)) {
			tabs[selected_tab]->render();
		}
		ImGui::EndChild();
		ImGui::PopStyleVar();
		ImGui::Columns();
    }
    ImGui::End();
	ImGui::PopStyleVar();

}

bool menu::menu_tab::render_button(const ImVec2 size, bool selected, bool vertical) noexcept {
    ImGuiID id = ImGui::GetID((label + "_back").c_str());
	ImGuiStyle style = ImGui::GetStyle();
	const ImVec2 label_size = ImGui::CalcTextSize(label.c_str(), NULL, true);
    const auto pos = ImGui::GetCurrentWindow()->DC.CursorPos;
	const ImRect rect(pos, pos + size);
	ImGui::ItemSize(size, style.FramePadding.y);
	if (!ImGui::ItemAdd(rect, id))
        return false;
    bool hovered;
	bool pressed = ImGui::ButtonBehavior(rect, id, &hovered, 0);
    if (pressed) ImGui::MarkItemEdited(id);
    float delta = ImGui::GetIO().DeltaTime;
	float size_var = vertical ? size.y : size.x;
	if (hovered || selected) {
		if (btn_mode == 100) {
			btn_mode = 0;
			btn_speed = 15;
		}
		if (btn_mode != 2) {
			btn_speed += delta * 50;
			btn_width += btn_speed * delta * 100;
		}
		if (btn_width > size_var) {
			btn_width = size_var;
			btn_speed *= btn_mode == 0 ? -.3f : 0;
			btn_mode++;
		}
	} else {
		if (btn_mode != 100) btn_speed = 0;
		btn_mode = 100;
		btn_speed -= delta * 50;
		btn_width += btn_speed * delta * 100;
		if (btn_width < 0) {
			btn_speed = 0;
			btn_width = 0;
		}
	}
    const ImRect rect2(pos, pos + (vertical ? ImVec2(size.x, btn_width) : ImVec2(btn_width, size.y)));
	ImGui::RenderNavHighlight(rect, id);
	ImGui::RenderFrame(rect2.Min, rect2.Max, ImGui::GetColorU32(selected ? ImGuiCol_ButtonActive : ImGuiCol_Button), false, style.FrameRounding);
	ImGui::RenderTextClipped(rect.Min + style.FramePadding, rect.Max - style.FramePadding, label.c_str(), NULL, &label_size, style.ButtonTextAlign, &rect);

	return pressed;
}

void menu::menu_tab_visuals::render() noexcept {
    ImGui::SeparatorText(XOR("Players"));
    ImGui::Checkbox(XOR("Enabled##Players"), &cfg.esp.players.enabled);
    ImGui::Checkbox(XOR("Teammates##Players"), &cfg.esp.players.teammates);
    ImGui::Checkbox(XOR("Box##Players"), &cfg.esp.players.box);
    ImGui::Checkbox(XOR("Skeleton##Players"), &cfg.esp.players.skeleton);
    ImGui::Checkbox(XOR("Name##Players"), &cfg.esp.players.name);
    ImGui::Checkbox(XOR("Health##Players"), &cfg.esp.players.health);
    ImGui::Checkbox(XOR("Weapon name##Players"), &cfg.esp.players.weapon_name);
    ImGui::Checkbox(XOR("Weapon ammo##Players"), &cfg.esp.players.weapon_ammo);
    

    ImGui::SeparatorText(XOR("Weapons"));
    ImGui::Checkbox(XOR("Enabled##Weapons"), &cfg.esp.weapons.enabled);
    ImGui::Checkbox(XOR("Box##Weapons"), &cfg.esp.weapons.box);
    ImGui::Checkbox(XOR("Name##Weapons"), &cfg.esp.weapons.name);
    ImGui::Checkbox(XOR("Ammo##Weapons"), &cfg.esp.weapons.ammo);
}

void menu::menu_tab_misc::render() noexcept {
    ImGui::Checkbox(XOR("Bunny hop"), &cfg.misc.bunny_hop);
    ImGui::Checkbox(XOR("Recoil crosshair"), &cfg.misc.reoil_crosshair);

    if (ImGui::Button(XOR("CRASH!")))
        *(uint32_t*)nullptr = 0xDEADBEEF;
    ImGui::SameLine();
    ASSERT(!ImGui::Button(XOR("ASSERT!")));
    ImGui::SameLine();
    ASSERT_MSG(!ImGui::Button(XOR("ASSERT MSG!")), "This is a manual assert message!");
}
