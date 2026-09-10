#include "gui.h"
#include "gui_colors.h"
#include "../../../../dependencies/oxorany/oxorany.h"
#include "../../../core/call of duty/settings/settings.h"

void ui::tabs::visuals ( const TabCategory tab )
{
    if ( tab.name == "visuals" )
    {
        static int section = 0;
        const char* sections[] = { "esp", "info", "text", "colors", "loot" };
        if ( section > 4 ) section = 0;

        ui::render_section_selector ( sections, IM_ARRAYSIZE ( sections ), section );
        ui::setup_section_layout ( );

        const int full_height = static_cast< int >( ui::size.y - 95.0f );

        // ── esp ──────────────────────────────────────────────
        if ( section == 0 )
        {
            if ( ui::begin_child_left ( "esp", full_height ) )
            {
                ImGui::TextColored ( ui::colors::text, "esp" );
                ImGui::Checkbox ( "skeleton", &settings::visuals::skeleton );
                if ( settings::visuals::skeleton ) {
                    ImGui::Checkbox ( "skeleton outline", &settings::visuals::skeleton_outline );
                }
                ImGui::Checkbox ( "box", &settings::visuals::box );
                if ( settings::visuals::box ) {
                    const char* box_types[] = { "corner box", "bounding box" };
                    ImGui::Combo ( "box type", &settings::visuals::box_type, box_types, IM_ARRAYSIZE ( box_types ) );
                    ImGui::Checkbox ( "box outline", &settings::visuals::box_outline );
                    ui::items::checkbox ( "box fill", &settings::visuals::box_fill, "gradient fill inside the box" );
                }
                ImGui::Checkbox ( "health bar", &settings::visuals::health );
                ImGui::SliderFloat ( "esp distance", &settings::visuals::esp_distance, 10.0f, 500.0f, "%.0fm" );
            }
            ImGui::EndChild ( );

            if ( ui::begin_child_right ( "extras", full_height ) )
            {
                ImGui::TextColored ( ui::colors::text, "extras" );
                ui::items::checkbox ( "offscreen arrows", &settings::visuals::offscreen_arrows, "directional arrows for players outside your view" );
                ImGui::Checkbox ( "chinese hat", &settings::visuals::china_hat );
                ui::items::checkbox ( "snaplines", &settings::visuals::snaplines, "gradient lines connecting you to enemies" );
                if ( settings::visuals::snaplines ) {
                    const char* snap_types[] = { "bottom", "crosshair" };
                    ImGui::Combo ( "snap origin", &settings::visuals::snapline_type, snap_types, IM_ARRAYSIZE(snap_types) );
                }
                ui::items::checkbox ( "aim direction", &settings::visuals::aim_direction, "gradient line showing enemy facing direction" );

                ImGui::Spacing ( );
                ImGui::TextColored ( ui::colors::text, "misc" );
                {
                    ImGui::Checkbox ( "streamproof", &settings::overlay::streamproof );
                    ImGui::SameLine ( 0.0f, 0.0f );
                    ImGui::SetCursorPosX ( ImGui::GetContentRegionMax ( ).x - 17.0f );
                    ui::items::safe_icon ( "streamproof##safe", "recommended — hides overlay from screen capture & streaming. keep this on to stay undetected." );
                }
                ui::items::checkbox ( "enable radar", &settings::visuals::radar::enabled, "2d top-down minimap showing player positions" );
            }
            ImGui::EndChild ( );
        }

        // ── info ─────────────────────────────────────────────
        if ( section == 1 )
        {
            if ( ui::begin_child_left ( "player info", full_height ) )
            {
                ImGui::TextColored ( ui::colors::text, "player info" );
                ImGui::Checkbox ( "username", &settings::visuals::username );
                ImGui::Checkbox ( "weapon", &settings::visuals::weapon );
                ImGui::Checkbox ( "level", &settings::visuals::level );
                ui::items::checkbox ( "kills / deaths", &settings::visuals::show_kills, "show K/D from scoreboard" );
                ui::items::checkbox ( "platform icon", &settings::visuals::show_platform, "show pc/xbox/ps icon next to name" );
            }
            ImGui::EndChild ( );

            if ( ui::begin_child_right ( "prestige", full_height ) )
            {
                ImGui::TextColored ( ui::colors::text, "prestige" );
                ui::items::checkbox ( "prestige icon", &settings::visuals::show_prestige, "show prestige emblem above level" );
                if ( settings::visuals::show_prestige ) {
                    const char* pstyles[] = { "BO7", "BO6", "BO3", "CoD4" };
                    ImGui::Combo ( "icon style", &settings::visuals::prestige_style, pstyles, IM_ARRAYSIZE ( pstyles ) );
                    ui::items::checkbox ( "icon background", &settings::visuals::prestige_bg, "dark rounded bg behind the icon" );
                }
            }
            ImGui::EndChild ( );
        }

        // ── text ─────────────────────────────────────────────
        if ( section == 2 )
        {
            if ( ui::begin_child_left ( "text settings", full_height ) )
            {
                ImGui::TextColored ( ui::colors::text, "text style" );

                const char* outline_types[] = { "no outline", "normal outline", "drop shadow" };
                ImGui::Combo ( "outline type", &settings::visuals::outline_type, outline_types, IM_ARRAYSIZE ( outline_types ) );

                ImGui::SliderFloat ( "text size", &settings::visuals::text_size, 8.0f, 20.0f, "%.0f" );
                ui::items::checkbox ( "text scaling", &settings::visuals::text_scaling, "shrinks text with distance for cleaner visuals" );

                const char* fonts[] = { "franklin gothic", "pixel" };
                ImGui::Combo ( "font", &settings::visuals::font_index, fonts, IM_ARRAYSIZE ( fonts ) );
            }
            ImGui::EndChild ( );
        }

        // ── colors ───────────────────────────────────────────
        if ( section == 3 )
        {
            const ImGuiColorEditFlags cf = ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaPreview;

            if ( ui::begin_child_left ( "visible gradient", full_height ) )
            {
                ImGui::TextColored ( ui::colors::text, "visible" );
                ImGui::ColorEdit4 ( "top", ( float* ) &settings::visuals::gradient_top, cf );
                ImGui::ColorEdit4 ( "bottom", ( float* ) &settings::visuals::gradient_bottom, cf );
            }
            ImGui::EndChild ( );

            if ( ui::begin_child_right ( "hidden gradient", full_height ) )
            {
                ImGui::TextColored ( ui::colors::text, "hidden" );
                ImGui::ColorEdit4 ( "top ", ( float* ) &settings::visuals::gradient_top_hidden, cf );
                ImGui::ColorEdit4 ( "bottom ", ( float* ) &settings::visuals::gradient_bottom_hidden, cf );
            }
            ImGui::EndChild ( );
        }

        // ── loot ─────────────────────────────────────────────
        if ( section == 4 )
        {
            if ( ui::begin_child_left ( "loot esp", full_height ) )
            {
                ImGui::TextColored ( ui::colors::text, "loot esp" );
                ui::items::checkbox ( "enable loot", &settings::loot::draw_loot, "shows dropped items on the ground" );
                ui::items::checkbox ( "placed equipment", &settings::loot::draw_equipment, "shows enemy placed equipment" );
                ui::items::checkbox ( "loot icons", &settings::loot::show_icons, "shows category icons above loot text" );
                if ( settings::loot::show_icons )
                    ui::items::checkbox ( "icons only", &settings::loot::icons_only, "hides text, shows only the icon" );
                ImGui::SliderInt ( "max distance", &settings::loot::max_distance, 10, 150, "%dm" );
            }
            ImGui::EndChild ( );

            if ( ui::begin_child_right ( "loot filters", full_height ) )
            {
                ImGui::TextColored ( ui::colors::text, "filters" );
                ImGui::Checkbox ( "weapons", &settings::loot::weapons );
                ImGui::Checkbox ( "ammo", &settings::loot::ammo );
                ImGui::Checkbox ( "armor", &settings::loot::armor );
                ImGui::Checkbox ( "killstreaks", &settings::loot::streaks );
                ImGui::Checkbox ( "stims", &settings::loot::stims );
                ImGui::Checkbox ( "crates", &settings::loot::crates );
                ImGui::Checkbox ( "cash", &settings::loot::money );
            }
            ImGui::EndChild ( );
        }
    }
}
