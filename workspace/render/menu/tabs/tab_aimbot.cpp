#include "gui.h"
#include "gui_colors.h"
#include "../../../../dependencies/oxorany/oxorany.h"
#include "../../../core/call of duty/settings/settings.h"

namespace hitbox_widget {

    enum Region { HEAD, NECK, CHEST, STOMACH, PELVIS, LARM, RARM, LLEG, RLEG, COUNT };

    inline ImU32 prio_col ( int p, float a ) {
        switch ( p ) {
        case 1:  return IM_COL32 ( 220, 50, 50, ( int )( 210 * a ) );
        case 2:  return IM_COL32 ( 220, 160, 40, ( int )( 210 * a ) );
        case 3:  return IM_COL32 ( 100, 200, 60, ( int )( 210 * a ) );
        case 4:  return IM_COL32 ( 60, 140, 220, ( int )( 210 * a ) );
        default: return IM_COL32 ( 45, 47, 55, ( int )( 200 * a ) );
        }
    }

    struct Poly {
        ImVec2 pts [12];
        int n;
    };

    inline bool point_in_poly ( const Poly& p, ImVec2 pt ) {
        bool inside = false;
        for ( int i = 0, j = p.n - 1; i < p.n; j = i++ ) {
            if ( ( p.pts[i].y > pt.y ) != ( p.pts[j].y > pt.y ) &&
                 pt.x < ( p.pts[j].x - p.pts[i].x ) * ( pt.y - p.pts[i].y ) / ( p.pts[j].y - p.pts[i].y ) + p.pts[i].x )
                inside = !inside;
        }
        return inside;
    }

    inline bool is_synced ( int a, int b ) {
        if ( ( a == LLEG || a == RLEG ) && ( b == LLEG || b == RLEG ) ) return true;
        if ( ( a == LARM || a == RARM ) && ( b == LARM || b == RARM ) ) return true;
        if ( ( a == NECK || a == CHEST ) && ( b == NECK || b == CHEST ) ) return true;
        return false;
    }

    inline int next_prio ( int current, int zone_idx ) {
        if ( current >= 1 && current <= 4 ) return 0;
        for ( int p = 1; p <= 4; ++p ) {
            bool taken = false;
            for ( int j = 0; j < COUNT; ++j ) {
                if ( j == zone_idx || is_synced ( j, zone_idx ) ) continue;
                if ( settings::aimbot::hitbox[j] == p ) { taken = true; break; }
            }
            if ( !taken ) return p;
        }
        return 0;
    }

    inline void draw ( ImDrawList* dl, float cx, float cy, float s, float alpha ) {
        ImGuiIO& io = ImGui::GetIO ( );
        int* hb = settings::aimbot::hitbox;

        ImU32 hov_out = IM_COL32 ( 255, 255, 255, ( int )( 130 * alpha ) );
        ImU32 seam = IM_COL32 ( 10, 10, 14, ( int )( 120 * alpha ) );

        auto P = [&] ( float x, float y ) { return ImVec2 ( cx + x*s, cy + y*s ); };

        struct Zone { int idx; ImVec2 mn; ImVec2 mx; float rnd; };
        Zone zones [] = {
            { HEAD,    P(-12,-78), P(12,-52), 10*s },
            { CHEST,   P(-22,-48), P(22,-10), 4*s },
            { STOMACH, P(-20,-10), P(20,8),   3*s },
            { PELVIS,  P(-18,8),   P(18,24),  3*s },
            { LARM,    P(-36,-44), P(-22,20), 5*s },
            { RARM,    P(22,-44),  P(36,20),  5*s },
            { LLEG,    P(-20,24),  P(-2,90),  5*s },
            { RLEG,    P(2,24),    P(20,90),  5*s },
        };

        const int order [] = { LLEG, RLEG, LARM, RARM, PELVIS, STOMACH, CHEST, HEAD };

        int clicked_zone = -1;

        for ( int oi = 0; oi < 8; ++oi ) {
            int r = order[oi];
            Zone* z = nullptr;
            for ( auto& zz : zones ) if ( zz.idx == r ) { z = &zz; break; }
            if ( !z ) continue;

            int prio = hb[r];
            bool hov = ImRect(z->mn, z->mx).Contains ( io.MousePos );

            ImU32 fill = prio_col ( prio, hov ? alpha : alpha * 0.85f );
            dl->AddRectFilled ( z->mn, z->mx, fill, z->rnd );

            if ( hov ) {
                dl->AddRect ( ImVec2(z->mn.x-1,z->mn.y-1), ImVec2(z->mx.x+1,z->mx.y+1), hov_out, z->rnd, 0, 2.0f );
                ImGui::SetMouseCursor ( ImGuiMouseCursor_Hand );
                if ( io.MouseClicked[0] ) clicked_zone = r;
            }
        }

        if ( clicked_zone >= 0 ) {
            int master = clicked_zone;
            if ( master == RLEG ) master = LLEG;
            if ( master == RARM ) master = LARM;
            if ( master == NECK ) master = CHEST;

            int np = next_prio ( hb[master], master );
            hb[master] = np;

            hb[NECK] = hb[CHEST];
            hb[RLEG] = hb[LLEG];
            hb[RARM] = hb[LARM];
        }

        dl->AddLine ( P(-22,-10), P(22,-10), seam, 1.0f );
        dl->AddLine ( P(-20,8), P(20,8), seam, 1.0f );
        dl->AddLine ( P(-18,24), P(18,24), seam, 1.0f );
        dl->AddLine ( P(0,24), P(0,90), seam, 1.0f );

        ImGui::PushFont ( font.spacegrotesk_medium[1] );
        float ly = cy + 98*s;
        float lx = cx - 56*s;
        float cw = 60*s;
        ImU32 txt_d = IM_COL32(90,92,100,(int)(170*alpha));
        ImU32 txt_l = IM_COL32(190,192,205,(int)(230*alpha));
        ImU32 ol = IM_COL32(8,9,12,(int)(180*alpha));

        auto leg = [&] ( float x, float y, ImU32 col, const char* lbl, ImU32 tc ) {
            dl->AddRectFilled ( ImVec2(x,y), ImVec2(x+8,y+8), col, 2 );
            dl->AddRect ( ImVec2(x,y), ImVec2(x+8,y+8), ol, 2, 0, 1.0f );
            dl->AddText ( ImVec2(x+12,y-2), tc, lbl );
        };

        leg ( lx, ly, prio_col(0,alpha), "ignore", txt_d );
        leg ( lx, ly+14, prio_col(4,alpha), "4th", txt_l );
        leg ( lx+cw, ly+14, prio_col(3,alpha), "3rd", txt_l );
        leg ( lx, ly+28, prio_col(2,alpha), "2nd", txt_l );
        leg ( lx+cw, ly+28, prio_col(1,alpha), "1st", txt_l );

        ImGui::PopFont ( );
    }
}

void ui::tabs::aimbot ( const TabCategory tab )
{
    if ( tab.name == "aimbot" )
    {
        static int section = 0;
        const char* sections [] = { "general", "fov" };
        if ( section > 1 ) section = 0;

        ui::render_section_selector ( sections, IM_ARRAYSIZE ( sections ), section );
        ui::setup_section_layout ( );

        const int full_height = static_cast< int >( ui::size.y - 95.0f );

        if ( section == 0 )
        {
            if ( ui::begin_child_left ( oxorany ( "aimbot" ), full_height ) )
            {
                ImGui::TextColored ( ui::colors::text, "aimbot" );
                ui::items::checkbox_unsafe ( "enable", &settings::aimbot::enabled, "aimbot may be detected by anti-cheat. are you sure?" );

                if ( settings::aimbot::enabled ) {
                    ImGui::Checkbox ( "controller support", &settings::aimbot::controller_support );

                    if ( !settings::aimbot::controller_support )
                        CustomWidgets::RenderKeybind ( "aim key", &settings::aimbot::aimbot_key );
                    else
                        CustomWidgets::RenderControllerKeybind ( oxorany ( "aim key" ), &settings::aimbot::aimbot_key );

                    ui::items::checkbox ( "prediction", &settings::aimbot::prediction, "leads targets based on bullet travel time and velocity" );
                    if ( settings::aimbot::prediction )
                        ImGui::SliderFloat ( "prediction strength", &settings::aimbot::prediction_strength, 0.1f, 2.0f, "%.1f" );
                    ui::items::checkbox ( "visible check", &settings::aimbot::visible_check, "only targets players you can see" );
                    ui::items::checkbox ( "sticky aim", &settings::aimbot::sticky_aim, "locks onto one target until key release or target dies" );
                }
            }
            ImGui::EndChild ( );

            if ( ui::begin_child_right ( oxorany ( "hitboxes" ), full_height ) )
            {
                ImGui::TextColored ( ui::colors::text, "hitboxes" );
                ImVec2 avail = ImGui::GetContentRegionAvail ( );
                ImVec2 cursor = ImGui::GetCursorScreenPos ( );
                ImGui::Dummy ( avail );
                float body_cx = cursor.x + avail.x * 0.5f;
                float body_cy = cursor.y + avail.y * 0.4f;
                float sc = ImMin ( avail.x / 100.0f, avail.y / 210.0f ) * 0.85f;
                hitbox_widget::draw ( ImGui::GetWindowDrawList ( ), body_cx, body_cy, sc, 1.0f );
            }
            ImGui::EndChild ( );
        }

        if ( section == 1 )
        {
            if ( ui::begin_child_left ( oxorany ( "fov" ), full_height ) )
            {
                ImGui::TextColored ( ui::colors::text, "fov & range" );
                ui::items::checkbox ( "show fov", &settings::aimbot::show_fov, "draws your aim fov circle on screen" );
                ImGui::SliderFloat ( "fov size", &settings::aimbot::fov, 5.0f, 350.0f );
                ImGui::SliderFloat ( "aim distance", &settings::aimbot::aim_distance, 10.0f, 500.0f, "%.0fm" );

                ImGui::Spacing ( );
                ImGui::TextColored ( ui::colors::text, "aim feel" );
                ImGui::SliderFloat ( "smoothness", &settings::aimbot::smoothness, 1.0f, 50.0f );
                ImGui::SliderFloat ( "deadzone", &settings::aimbot::deadzone, 0.0f, 20.0f, "%.1fpx" );
                ui::items::checkbox ( "bezier curve", &settings::aimbot::bezier_aim, "curved aim path instead of straight line, different direction each target" );
                if ( settings::aimbot::bezier_aim )
                    ImGui::SliderFloat ( "curve strength", &settings::aimbot::curve_strength, 0.0f, 0.4f, "%.2f" );
            }
            ImGui::EndChild ( );
        }
    }
}
