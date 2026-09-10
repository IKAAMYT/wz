#pragma once
#include <blur/directx_blur.h>
#include <gui.h>
#include "../../../dependencies/oxorany/oxorany.h"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif
#include <gui_colors.h>
#include "../dashboard/dashboard.h"
#include "../../core/call of duty/settings/settings.h"

extern ID3D11ShaderResourceView* prestige_sets [4][12];
extern int prestige_set_counts [4];

namespace esp_preview {

    inline void draw ( ImVec2 menu_pos, ImVec2 menu_size, float alpha ) {
        if ( alpha < 0.35f ) return;

        const float pw = 200.0f;
        const float ph = menu_size.y;
        const float gap = 8.0f;
        const ImVec2 panel_pos ( menu_pos.x + menu_size.x + gap, menu_pos.y );
        const ImVec2 panel_max ( panel_pos.x + pw, panel_pos.y + ph );

        ImDrawList* dl = ImGui::GetForegroundDrawList ( );
        const float time = ImGui::GetTime ( );
        ImU32 black_a = IM_COL32 ( 0, 0, 0, ( int )( 255 * alpha ) );

        dl->AddRectFilled ( panel_pos, panel_max, ImGui::GetColorU32 ( ImVec4 ( ui::colors::background.x, ui::colors::background.y, ui::colors::background.z, alpha * 0.95f ) ), ui::rounding );
        dl->AddRect ( panel_pos, panel_max, ImGui::GetColorU32 ( ImVec4 ( ui::colors::outline.x, ui::colors::outline.y, ui::colors::outline.z, alpha * 0.5f ) ), ui::rounding );

        ImGui::PushFont ( font.spacegrotesk_medium [1] );
        {
            const char* lbl = "preview";
            ImVec2 lsz = ImGui::CalcTextSize ( lbl );
            dl->AddText ( ImVec2 ( panel_pos.x + ( pw - lsz.x ) * 0.5f, panel_pos.y + 8.0f ), ImGui::GetColorU32 ( ImVec4 ( ui::colors::text.x, ui::colors::text.y, ui::colors::text.z, alpha * 0.5f ) ), lbl );
        }

        static std::string pc_name;
        if ( pc_name.empty ( ) ) {
            char buf [256] {};
            DWORD len = sizeof ( buf );
            if ( GetUserNameA ( buf, &len ) && len > 1 )
                pc_name = std::string ( buf, len - 1 ) + " [SLOPPY]";
            else
                pc_name = "player [SLOPPY]";
        }

        const float cx = panel_pos.x + pw * 0.5f;
        const float cy = panel_pos.y + ph * 0.48f;
        const float sc = 1.3f;

        struct Pose { float b [14][2]; };
        static const Pose poses [] = {
            {{{ 0,-55},{0,15},{0,-5},{0,-40},{-15,-36},{-22,-18},{-18,2},{15,-36},{22,-18},{18,2},{-7,20},{-9,48},{7,20},{9,48} }},
            {{{ 2,-54},{1,16},{1,-4},{2,-39},{-13,-35},{-28,-22},{-32,-8},{17,-35},{20,-12},{16,6},{-8,22},{-14,50},{6,18},{8,46} }},
            {{{-2,-53},{-1,16},{-1,-4},{-2,-39},{-17,-35},{-20,-12},{-16,6},{13,-35},{28,-22},{32,-8},{-6,18},{-8,46},{8,22},{14,50} }},
            {{{ 0,-52},{0,18},{0,-2},{0,-38},{-14,-34},{-24,-15},{-20,5},{14,-34},{24,-15},{20,5},{-9,22},{-16,44},{9,22},{16,44} }},
            {{{ 3,-55},{2,14},{2,-6},{3,-40},{-12,-37},{-26,-26},{-30,-12},{18,-37},{16,-14},{12,4},{-6,19},{-4,48},{8,21},{12,50} }},
            {{{-3,-55},{-2,14},{-2,-6},{-3,-40},{-18,-37},{-16,-14},{-12,4},{12,-37},{26,-26},{30,-12},{-8,21},{-12,50},{6,19},{4,48} }},
            {{{ 1,-53},{0,17},{0,-3},{1,-39},{-15,-35},{-30,-20},{-35,-4},{15,-35},{24,-10},{18,8},{-7,21},{-10,49},{7,21},{10,49} }},
            {{{ 0,-56},{0,14},{0,-6},{0,-41},{-14,-38},{-18,-20},{-14,0},{14,-38},{18,-20},{14,0},{-8,18},{-12,46},{8,18},{12,46} }},
            {{{ 4,-54},{3,15},{3,-5},{4,-39},{-11,-36},{-20,-24},{-24,-10},{19,-36},{26,-16},{22,2},{-5,20},{-2,50},{9,20},{14,48} }},
            {{{-4,-54},{-3,15},{-3,-5},{-4,-39},{-19,-36},{-26,-16},{-22,2},{11,-36},{20,-24},{24,-10},{-9,20},{-14,48},{5,20},{2,50} }},
            {{{ 0,-54},{0,16},{0,-4},{0,-40},{-16,-36},{-25,-18},{-22,0},{16,-36},{25,-18},{22,0},{-6,20},{-8,47},{6,20},{8,47} }},
            {{{ 2,-56},{1,13},{1,-7},{2,-41},{-13,-38},{-22,-28},{-28,-16},{17,-38},{18,-14},{14,4},{-7,18},{-10,44},{7,18},{10,44} }},
        };
        static constexpr int num_poses = sizeof ( poses ) / sizeof ( poses [0] );

        static float pose_time = 0.0f;
        static int pose_a = 0, pose_b = 1;
        pose_time += ImGui::GetIO ( ).DeltaTime * 0.5f;
        if ( pose_time >= 1.0f ) {
            pose_time -= 1.0f;
            pose_a = pose_b;
            pose_b = ( pose_a + 1 + ( (int)( time * 7.0f ) % ( num_poses - 1 ) ) ) % num_poses;
        }

        float t = pose_time * pose_time * ( 3.0f - 2.0f * pose_time );
        float breathe = sinf ( time * 2.2f ) * 0.6f;

        ImVec2 bones [14];
        for ( int i = 0; i < 14; ++i ) {
            float px = poses[pose_a].b[i][0] + ( poses[pose_b].b[i][0] - poses[pose_a].b[i][0] ) * t;
            float py = poses[pose_a].b[i][1] + ( poses[pose_b].b[i][1] - poses[pose_a].b[i][1] ) * t;
            bones [i] = ImVec2 ( cx + px * sc, cy + py * sc + breathe * ( i < 4 ? 1.0f : 0.3f ) );
        }

        float bx0 = FLT_MAX, by0 = FLT_MAX, bx1 = -FLT_MAX, by1 = -FLT_MAX;
        for ( int i = 0; i < 14; ++i ) {
            bx0 = ImMin ( bx0, bones[i].x ); by0 = ImMin ( by0, bones[i].y );
            bx1 = ImMax ( bx1, bones[i].x ); by1 = ImMax ( by1, bones[i].y );
        }
        bx0 -= 6; by0 -= 6; bx1 += 6; by1 += 6;
        float bw = bx1 - bx0, bh = by1 - by0;

        ImU32 col_top = ImGui::ColorConvertFloat4ToU32 ( ImVec4 ( settings::visuals::gradient_top.Value.x, settings::visuals::gradient_top.Value.y, settings::visuals::gradient_top.Value.z, alpha ) );
        ImU32 col_bot = ImGui::ColorConvertFloat4ToU32 ( ImVec4 ( settings::visuals::gradient_bottom.Value.x, settings::visuals::gradient_bottom.Value.y, settings::visuals::gradient_bottom.Value.z, alpha ) );

        int fake_health = ( int ) ( 50.0f + 45.0f * sinf ( time * 0.8f ) );

        if ( settings::visuals::box && settings::visuals::box_fill ) {
            ImU32 cent = IM_COL32 ( 10, 10, 15, ( int )( 20 * alpha ) );
            float mid = ( by0 + by1 ) * 0.5f;
            dl->AddRectFilledMultiColor ( ImVec2(bx0+1,by0+1), ImVec2(bx1-1,mid), col_top, col_top, cent, cent );
            dl->AddRectFilledMultiColor ( ImVec2(bx0+1,mid), ImVec2(bx1-1,by1-1), cent, cent, col_bot, col_bot );
        }

        if ( settings::visuals::box ) {
            if ( settings::visuals::box_type == 0 ) {
                float cw = bw / 4.0f, ch = bh / 4.5f;
                auto cl = [&] ( ImVec2 a, ImVec2 b ) {
                    if ( settings::visuals::box_outline ) dl->AddLine ( a, b, black_a, 3.0f );
                    dl->AddLine ( a, b, col_top, 1.5f );
                };
                cl(ImVec2(bx0,by0),ImVec2(bx0+cw,by0)); cl(ImVec2(bx0,by0),ImVec2(bx0,by0+ch));
                cl(ImVec2(bx1-cw,by0),ImVec2(bx1,by0)); cl(ImVec2(bx1,by0),ImVec2(bx1,by0+ch));
                auto cl2 = [&] ( ImVec2 a, ImVec2 b ) {
                    if ( settings::visuals::box_outline ) dl->AddLine ( a, b, black_a, 3.0f );
                    dl->AddLine ( a, b, col_bot, 1.5f );
                };
                cl2(ImVec2(bx0,by1-ch),ImVec2(bx0,by1)); cl2(ImVec2(bx0,by1),ImVec2(bx0+cw,by1));
                cl2(ImVec2(bx1,by1-ch),ImVec2(bx1,by1)); cl2(ImVec2(bx1-cw,by1),ImVec2(bx1,by1));
            } else {
                if ( settings::visuals::box_outline ) dl->AddRect ( ImVec2(bx0,by0), ImVec2(bx1,by1), black_a, 0, 0, 3.0f );
                dl->AddLine ( ImVec2(bx0,by0), ImVec2(bx1,by0), col_top, 1.5f );
                dl->AddLine ( ImVec2(bx0,by1), ImVec2(bx1,by1), col_bot, 1.5f );
                dl->AddLine ( ImVec2(bx0,by0), ImVec2(bx0,by1), col_top, 1.5f );
                dl->AddLine ( ImVec2(bx1,by0), ImVec2(bx1,by1), col_top, 1.5f );
            }
        }

        if ( settings::visuals::skeleton ) {
            float thick = 1.5f;
            auto sline = [&] ( int a, int b ) {
                if ( settings::visuals::skeleton_outline ) dl->AddLine ( bones[a], bones[b], black_a, thick * 2.0f );
                dl->AddLine ( bones[a], bones[b], col_top, thick );
            };
            sline(3,0); sline(1,3); sline(1,10); sline(10,11); sline(1,12); sline(12,13);
            sline(3,4); sline(4,5); sline(5,6); sline(3,7); sline(7,8); sline(8,9);
        }

        if ( settings::visuals::china_hat ) {
            ImVec2 head = bones[0];
            ImVec2 tip ( head.x, head.y - 14.0f );
            ImU32 hat_col = col_top;
            for ( int i = 0; i < 12; ++i ) {
                float a0 = (float)i/12 * 6.2831853f, a1 = (float)(i+1)/12 * 6.2831853f;
                ImVec2 p0 ( head.x + cosf(a0)*9, head.y - 4 + sinf(a0)*3 );
                ImVec2 p1 ( head.x + cosf(a1)*9, head.y - 4 + sinf(a1)*3 );
                dl->AddLine ( p0, p1, hat_col, 1.0f );
                dl->AddLine ( tip, p0, hat_col, 1.0f );
            }
        }

        ImU32 white_a = IM_COL32 ( 255, 255, 255, ( int )( 220 * alpha ) );
        ImU32 shadow_a = IM_COL32 ( 0, 0, 0, ( int )( 200 * alpha ) );
        ImU32 ghost_fill = IM_COL32 ( 177, 113, 255, ( int )( 18 * alpha ) );
        ImU32 ghost_line = IM_COL32 ( 177, 113, 255, ( int )( 60 * alpha ) );
        float dt = ImGui::GetIO ( ).DeltaTime;
        ImVec2 mpos = ImGui::GetIO ( ).MousePos;

        static ImVec2 rel [6];
        static bool rel_init = false;
        static int drag_id = -1;
        static ImVec2 drag_abs;

        struct Elem { const char* txt; int* pos; bool on; int id; };
        Elem elems [] = {
            { pc_name.c_str ( ), &settings::visuals::username_pos, settings::visuals::username,       0 },
            { "level: 55",       &settings::visuals::level_pos,    settings::visuals::level,          1 },
            { "xm4 [25m]",      &settings::visuals::weapon_pos,   settings::visuals::weapon,         2 },
            { "health",          &settings::visuals::health_pos,   settings::visuals::health,         3 },
            { "12K / 3D",        &settings::visuals::kills_pos,    settings::visuals::show_kills,     4 },
            { "PC",              &settings::visuals::platform_pos, settings::visuals::show_platform,  5 },
        };

        auto esz = [&] ( int id, int pos ) -> ImVec2 {
            if ( id == 3 ) { bool v=(pos==2||pos==3); return ImVec2(v?2.5f:bw, v?bh:3.0f); }
            return ImGui::CalcTextSize ( elems[id].txt );
        };

        int nearest_pos_v = -1;
        if ( drag_id >= 0 ) {
            float bcx=(bx0+bx1)*0.5f, bcy=(by0+by1)*0.5f;
            float ddx=drag_abs.x-bcx, ddy=drag_abs.y-bcy;
            nearest_pos_v = fabsf(ddy)>fabsf(ddx) ? (ddy<0?0:1) : (ddx<0?2:3);
        }

        float ts=0,bs=0,ls=0,rs=0;
        ImVec2 targets [6];

        for ( auto& e : elems ) {
            if ( !e.on ) { targets[e.id]=ImVec2(0,0); continue; }
            int pos = *e.pos;
            if ( drag_id == e.id ) pos = nearest_pos_v >= 0 ? nearest_pos_v : pos;
            ImVec2 sz = esz(e.id, pos);
            switch ( pos ) {
            case 0: targets[e.id]=ImVec2(cx-sz.x*0.5f-cx, by0-sz.y-4-ts-cy); ts+=sz.y+2; break;
            case 1: targets[e.id]=ImVec2(cx-sz.x*0.5f-cx, by1+4+bs-cy); bs+=sz.y+2; break;
            case 2: targets[e.id]=ImVec2(bx0-sz.x-8-cx, by0+ls-cy); ls+=sz.y+2; break;
            case 3: targets[e.id]=ImVec2(bx1+8-cx, by0+rs-cy); rs+=sz.y+2; break;
            }
        }

        if ( !rel_init ) { for(int i=0;i<6;++i) rel[i]=targets[i]; rel_init=true; }

        if ( drag_id >= 0 && ImGui::GetIO().MouseDown[0] ) {
            drag_abs.x += ImGui::GetIO().MouseDelta.x;
            drag_abs.y += ImGui::GetIO().MouseDelta.y;
            rel[drag_id] = ImVec2(drag_abs.x-cx, drag_abs.y-cy);

            for ( int p = 0; p < 4; ++p ) {
                float gts=0,gbs=0,gls=0,grs=0;
                ImVec2 gsz = esz(drag_id, p);
                ImVec2 gp;
                switch(p){
                case 0: gp=ImVec2(cx-gsz.x*0.5f, by0-gsz.y-4); break;
                case 1: gp=ImVec2(cx-gsz.x*0.5f, by1+4); break;
                case 2: gp=ImVec2(bx0-gsz.x-8, by0); break;
                case 3: gp=ImVec2(bx1+8, by0); break;
                }
                dl->AddRectFilled(ImVec2(gp.x-2,gp.y-1),ImVec2(gp.x+gsz.x+2,gp.y+gsz.y+1),ghost_fill,3.0f);
                dl->AddRect(ImVec2(gp.x-2,gp.y-1),ImVec2(gp.x+gsz.x+2,gp.y+gsz.y+1),ghost_line,3.0f,0,1.0f);
            }
        }
        else if ( drag_id >= 0 ) {
            float bcx=(bx0+bx1)*0.5f, bcy=(by0+by1)*0.5f;
            float ddx=drag_abs.x-bcx, ddy=drag_abs.y-bcy;
            *elems[drag_id].pos = fabsf(ddy)>fabsf(ddx) ? (ddy<0?0:1) : (ddx<0?2:3);
            drag_id = -1;
        }

        float spd = ImClamp(dt*12.0f, 0.0f, 1.0f);
        for ( int i = 0; i < 6; ++i ) {
            if ( i == drag_id ) continue;
            rel[i].x += (targets[i].x - rel[i].x) * spd;
            rel[i].y += (targets[i].y - rel[i].y) * spd;
        }

        for ( auto& e : elems ) {
            if ( !e.on ) continue;
            ImVec2 p ( cx + rel[e.id].x, cy + rel[e.id].y );

            int cur_pos = *e.pos;
            if ( drag_id == e.id && nearest_pos_v >= 0 ) cur_pos = nearest_pos_v;

            if ( e.id != 3 ) {
                ImVec2 sz = ImGui::CalcTextSize(e.txt);
                ImRect hit(p, ImVec2(p.x+sz.x, p.y+sz.y));
                bool hov = drag_id<0 && hit.Contains(mpos);
                if ( hov || drag_id==e.id )
                    dl->AddRectFilled(ImVec2(p.x-3,p.y-2),ImVec2(p.x+sz.x+3,p.y+sz.y+2),IM_COL32(255,255,255,(int)(15*alpha)),3.0f);
                if ( hov ) {
                    ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
                    if(ImGui::GetIO().MouseClicked[0]){drag_id=e.id;drag_abs=p;}
                }
                dl->AddText(ImVec2(p.x+1,p.y+1), shadow_a, e.txt);
                ImU32 txt_col = white_a;
                if ( e.id == 5 ) txt_col = IM_COL32 ( 0, 190, 255, (int)(255*alpha) );
                dl->AddText(p, txt_col, e.txt);
            } else {
                int hp = std::clamp(fake_health, 0, 100);
                float pct = hp / 100.0f;
                bool vert = (cur_pos==2||cur_pos==3);
                float bar_w = vert?2.5f:bw, bar_h = vert?bh:3.0f;
                const auto& tc = settings::visuals::gradient_top;
                const auto& gbc = settings::visuals::gradient_bottom;
                ImU32 fc = ImGui::ColorConvertFloat4ToU32(ImVec4(tc.Value.x,tc.Value.y,tc.Value.z,alpha));
                ImU32 lc = ImGui::ColorConvertFloat4ToU32(ImVec4(gbc.Value.x,gbc.Value.y,gbc.Value.z,alpha));
                dl->AddRectFilled(ImVec2(p.x-1,p.y-1),ImVec2(p.x+bar_w+1,p.y+bar_h+1),IM_COL32(15,16,22,(int)(255*alpha)));
                dl->AddRectFilled(ImVec2(p.x,p.y),ImVec2(p.x+bar_w,p.y+bar_h),IM_COL32(15,16,22,(int)(150*alpha)));
                if(vert){float fh=bar_h*pct;dl->AddRectFilledMultiColor(ImVec2(p.x,p.y+bar_h-fh),ImVec2(p.x+bar_w,p.y+bar_h),fc,fc,lc,lc);}
                else{float fw=bar_w*pct;dl->AddRectFilledMultiColor(ImVec2(p.x,p.y),ImVec2(p.x+fw,p.y+bar_h),fc,lc,lc,fc);}
                ImRect hb(ImVec2(p.x-3,p.y-3),ImVec2(p.x+bar_w+3,p.y+bar_h+3));
                bool hov = drag_id<0 && hb.Contains(mpos);
                if(hov||drag_id==e.id) dl->AddRect(hb.Min,hb.Max,IM_COL32(255,255,255,(int)(30*alpha)),2.0f);
                if(hov){ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);if(ImGui::GetIO().MouseClicked[0]){drag_id=e.id;drag_abs=p;}}
            }
        }

        if ( settings::visuals::show_prestige ) {
            int sty = std::clamp ( settings::visuals::prestige_style, 0, 3 );
            int fake_idx = min ( 4, prestige_set_counts [sty] - 1 );
            ID3D11ShaderResourceView* picon = ( fake_idx >= 0 ) ? prestige_sets [sty][fake_idx] : nullptr;
            if ( picon ) {
                float icon_sz = 18.0f;
                float pad = 3.0f;
                int pos = settings::visuals::level ? settings::visuals::level_pos : settings::visuals::username_pos;
                ImVec2 p;
                switch ( pos ) {
                case 0: p = ImVec2 ( cx - icon_sz * 0.5f, by0 - icon_sz - 4 - ts ); ts += icon_sz + pad * 2; break;
                case 1: p = ImVec2 ( cx - icon_sz * 0.5f, by1 + 4 + bs ); bs += icon_sz + pad * 2; break;
                case 2: p = ImVec2 ( bx0 - icon_sz - 8, by0 + ls ); ls += icon_sz + pad * 2; break;
                case 3: p = ImVec2 ( bx1 + 8, by0 + rs ); rs += icon_sz + pad * 2; break;
                default: p = ImVec2 ( cx - icon_sz * 0.5f, by0 - icon_sz - 4 - ts ); ts += icon_sz + pad * 2; break;
                }
                if ( settings::visuals::prestige_bg )
                    dl->AddRectFilled ( ImVec2 ( p.x - pad, p.y - pad ), ImVec2 ( p.x + icon_sz + pad, p.y + icon_sz + pad ), IM_COL32 ( 0, 0, 0, ( int )( 140 * alpha ) ), 3.0f );
                dl->AddImage ( ( ImTextureID ) picon, p, ImVec2 ( p.x + icon_sz, p.y + icon_sz ), ImVec2(0,0), ImVec2(1,1), IM_COL32 ( 255, 255, 255, ( int )( 255 * alpha ) ) );
            }
        }

        ImGui::PopFont ( );
    }
}

namespace menu
{
    inline bool  is_open   = false;
    inline float open_anim = 0.0f;

    inline void tick ( float dt ) {
        const float target = is_open ? 1.0f : 0.0f;
        open_anim += ( target - open_anim ) * ( dt * 12.0f < 1.0f ? dt * 12.0f : 1.0f );
    }

    inline bool should_render ( ) {
        return is_open || open_anim > 0.01f;
    }

    void show ( ) {

        if ( !should_render ( ) )
            return;

        const float dt = ImGui::GetIO ( ).DeltaTime;
        tick ( dt );
        ui::update_animations ( dt );

        const ImVec2 full_size = ui::size;
        const float  anim_h = full_size.y * open_anim;

        ImGui::SetNextWindowSize ( ImVec2 ( full_size.x, anim_h ), ImGuiCond_Always );
        ImGui::PushStyleVar ( ImGuiStyleVar_Alpha, open_anim );
        ImGui::Begin ( oxorany ( "software" ), nullptr , ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus );
        {
            ui::UpdateMenuColors ( );

            ui::render_background ( );

            if ( open_anim >= 0.35f ) {
                ui::render_title_cheat ( oxorany ( "slopware" ) );

                ui::render_build_date ( );

                ui::render_tabs_content ( );

                ui::render_tabs ( ImGui::GetIO ( ).DeltaTime );

                ui::render_outline ( );

                ui::render_notification ( );
            }
        }
        ImVec2 menu_pos = ImGui::GetWindowPos ( );
        ImVec2 menu_sz  = ImGui::GetWindowSize ( );
        ImGui::End ( );
        ImGui::PopStyleVar ( );

        if ( open_anim >= 0.35f && ui::is_tab_selected ( "visuals" ) )
            esp_preview::draw ( menu_pos, menu_sz, open_anim );

        if ( open_anim >= 0.35f ) {
            if ( dashboard::draw_topbar ( ) )
                menu::is_open = false;
        }
    }

}
