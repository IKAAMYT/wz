#pragma once

#include <Windows.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <gui.h>
#include <gui_colors.h>
#include <string>
#include "../../../dependencies/oxorany/oxorany.h"

// Standalone top-center dashboard bar (ported from the reference, no leaderboard).
// Drawn as its own floating window above the menu — NOT inside the menu panel.
namespace dashboard {

    namespace theme {
        inline ImU32 accent ( ) { return ImGui::GetColorU32 ( ui::colors::main ); }
        inline ImU32 text   ( ) { return ImGui::GetColorU32 ( ui::colors::text ); }
        inline ImU32 muted  ( ) { return IM_COL32 ( 140, 140, 155, 210 ); }
        inline ImU32 bg     ( ) { return IM_COL32 ( 10, 10, 15, 235 ); }
        inline ImU32 outline( ) { return IM_COL32 ( 255, 255, 255, 16 ); }
    }

    inline ImFont* ui_font ( ) {
        return font.spacegrotesk_medium[1] ? font.spacegrotesk_medium[1] : ImGui::GetFont ( );
    }

    inline std::string masked_user ( ) {
        char buf[256] {};
        DWORD len = sizeof ( buf );
        std::string name = GetUserNameA ( buf, &len ) && len > 1 ? std::string ( buf, len - 1 ) : std::string ( "user" );
        if ( name.empty ( ) ) name = "user";
        return name;
    }

    // Returns true if "exit" was clicked. Draws a centered floating bar at the top.
    inline bool draw_topbar ( ) {
        const ImVec2 display = ImGui::GetIO ( ).DisplaySize;
        const float  bar_w = display.x < 900.0f ? display.x - 32.0f : 780.0f;
        const float  bar_h = 48.0f;
        const float  pad = 14.0f;
        const ImVec2 bar_pos ( ( display.x - bar_w ) * 0.5f, 16.0f );
        const ImVec2 bar_max ( bar_pos.x + bar_w, bar_pos.y + bar_h );

        ImFont* f = ui_font ( );
        const float fs = f->FontSize;

        const char* exit_l = oxorany ( "exit" );
        const char* brand = oxorany ( "shitware" );
        const ImVec2 exit_size = ImGui::CalcTextSize ( exit_l );
        const ImVec2 brand_size = ImGui::CalcTextSize ( brand );

        const float link_gap = 18.0f;
        const float right_w = exit_size.x + link_gap + brand_size.x;
        const float right_x = bar_max.x - pad - right_w;
        const float text_y = bar_pos.y + ( bar_h - fs ) * 0.5f;

        const ImVec2 exit_pos ( right_x, text_y );
        const ImVec2 brand_pos ( right_x + exit_size.x + link_gap, text_y );

        bool exit_pressed = false;
        bool exit_hover = false;

        // own window purely for hit-testing the exit link
        const float hpx = 4.0f, hpy = 6.0f;
        ImGui::SetNextWindowPos ( bar_pos );
        ImGui::SetNextWindowSize ( ImVec2 ( bar_w, bar_h ) );
        ImGui::PushStyleVar ( ImGuiStyleVar_WindowPadding, ImVec2 ( 0, 0 ) );
        ImGui::PushStyleColor ( ImGuiCol_WindowBg, ImVec4 ( 0, 0, 0, 0 ) );
        if ( ImGui::Begin ( "##dashboard_topbar", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoSavedSettings ) )
        {
            ImGui::SetCursorScreenPos ( ImVec2 ( exit_pos.x - hpx, exit_pos.y - hpy ) );
            if ( ImGui::InvisibleButton ( "##exit", ImVec2 ( exit_size.x + hpx * 2.0f, exit_size.y + hpy * 2.0f ) ) )
                exit_pressed = true;
            exit_hover = ImGui::IsItemHovered ( );
        }
        ImGui::End ( );
        ImGui::PopStyleColor ( );
        ImGui::PopStyleVar ( );

        // draw on the foreground so the bar floats above everything
        ImDrawList* dl = ImGui::GetForegroundDrawList ( );
        dl->AddRectFilled ( bar_pos, bar_max, theme::bg ( ), 8.0f );
        dl->AddRect ( bar_pos, bar_max, theme::outline ( ), 8.0f, 0, 1.0f );

        // left: welcome + masked user
        const std::string hello = oxorany ( "welcome back, " );
        const std::string user = masked_user ( );
        float x = bar_pos.x + pad;
        ui::items::text_shadow ( dl, ImVec2 ( x, text_y ), ImColor ( theme::muted ( ) ), hello.c_str ( ), f, fs );
        x += ImGui::CalcTextSize ( hello.c_str ( ) ).x;
        ui::items::text_shadow ( dl, ImVec2 ( x, text_y ), ImColor ( theme::text ( ) ), user.c_str ( ), f, fs );

        // center: fps
        const std::string fps = oxorany ( "fps: " ) + std::to_string ( ( int ) ImGui::GetIO ( ).Framerate );
        const float fps_w = ImGui::CalcTextSize ( fps.c_str ( ) ).x;
        ui::items::text_shadow ( dl, ImVec2 ( bar_pos.x + ( bar_w - fps_w ) * 0.5f, text_y ), ImColor ( theme::muted ( ) ), fps.c_str ( ), f, fs );

        // right: exit + brand
        ui::items::text_shadow ( dl, exit_pos, ImColor ( exit_hover ? theme::text ( ) : theme::muted ( ) ), exit_l, f, fs );
        ui::items::text_shadow ( dl, brand_pos, ImColor ( theme::accent ( ) ), brand, f, fs );

        if ( exit_hover )
            ImGui::SetMouseCursor ( ImGuiMouseCursor_Hand );

        return exit_pressed;
    }
}
