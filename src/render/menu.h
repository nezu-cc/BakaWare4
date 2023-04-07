#pragma once
#include "../base/base.h"

namespace menu {
    inline bool is_open{ true };

    inline void toggle() { is_open = !is_open; };
    void render() noexcept;

    class menu_tab {
    private:
        std::string label;
        float btn_width{ 0.f };
        float btn_speed{ 0.f };
        unsigned int btn_mode{ 0 };
    public:
        menu_tab(std::string label) noexcept : label(label) { };
        bool render_button(const ImVec2 size, bool selected = false, bool vertical = false) noexcept;
        virtual void render() noexcept = 0;
    };

    class menu_tab_visuals : public menu_tab {
    public:
        menu_tab_visuals() noexcept : menu_tab("Visuals") { };
        void render() noexcept override;
    };

    class menu_tab_misc : public menu_tab {
    public:
        menu_tab_misc() noexcept : menu_tab("Misc") { };
        void render() noexcept override;
    };

    inline std::vector<menu_tab*> tabs {
        new menu_tab_visuals(),
        new menu_tab_misc()
    };
}
