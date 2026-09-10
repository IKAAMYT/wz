#pragma once

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_internal.h"
#include <d3d11.h>
#include <D3DX11tex.h>
#pragma comment(lib, "D3DX11.lib")
#include "gui_colors.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <cctype> 
#include <ranges>
#include <algorithm>


namespace images
{
    inline ID3D11ShaderResourceView* background;
};

struct Fonts {
    ImFont* spacegrotesk_medium[3];
    ImFont* montserrat_semibold[3];
    ImFont* tab_icon;
    ImFont* widget_icon[3];
    ImFont* notification_icon;
}; inline Fonts font;

struct TabSub
{
    std::string name;
};

struct TabCategory
{
    std::string name;
    std::string icon;

    TabCategory(const std::string& n, const std::string& i)
        : name(n), icon(i)
    {
    }
};

extern std::vector<TabCategory> categories;
inline int selected_tab_index = 0;
inline std::vector<float> tab_lerp_values;
inline float tab_enter_anim = 1.0f;
inline bool has_scrollbar = false;
inline int child_width = 320;

struct MinimizeData
{
    bool target_minimized = false;
    float animated_height = 0.0f;
};

static std::unordered_map<ImGuiID, MinimizeData> minimize_data;

namespace ui
{
    inline ImVec2 size = ImVec2(674, 560);
    const float rounding = 6.f;

    // intialize font
    void initialize_fonts();

    // initialize images
    void initialize_images();

    // render background
    void render_background();

    // render title
    void render_title();

    // render title cheat
    void render_title_cheat(std::string game_name);

    // render build date
    void render_build_date();

    // initialize tabs
    void initialize_tabs();

    // tab selected
    bool is_tab_selected(const std::string& tab_name);

    // render tabs
    void render_tabs(float dt);

    // add notification
    void add_notification(const std::string& icon, const std::string& msg, const ImVec4& icon_color);

    // render notification
    void render_notification();

    // get tab alpha
    float get_tab_alpha(int index);

    void reset_positions(ImVec2 base_left, ImVec2 base_right, float space_x, float space_y);
    bool begin_child_left(const char* id, int height);
    bool begin_child_right(const char* id, int height);

    // menu open/close + tab switch animation driver
    void update_animations(float dt);

    // subtab (section) selector + layout
    inline constexpr float k_section_row = 34.0f;
    void render_section_selector(const char* const* names, int count, int& selected);
    void setup_section_layout();

    // render tabs content
    void render_tabs_content();

    // render watermark
    void show_fps();

    // render outline
    void render_outline();

    namespace tabs
    {
        void aimbot(const TabCategory tab);
        void visuals(const TabCategory tab);
        void exploits(const TabCategory tab);
        void loot ( const TabCategory tab );
        void cloud ( const TabCategory tab );

        void settings(const TabCategory tab);
    };

    void flush_tooltips ( );

    namespace items
    {
        void text_shadow(ImDrawList* draw_list, ImVec2 pos, ImColor text_color, const char* text, ImFont* font, float font_size);
        bool info_icon ( const char* id, const char* tooltip );
        bool safe_icon ( const char* id, const char* tooltip );
        bool unsafe_icon ( const char* id, const char* tooltip );
        bool checkbox ( const char* label, bool* v, const char* info = nullptr );
        bool checkbox_unsafe ( const char* label, bool* v, const char* warning = nullptr );
    };
};
