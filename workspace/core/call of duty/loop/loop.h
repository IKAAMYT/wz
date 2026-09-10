#pragma once
#include "../cache/cache.h"
#include <imgui.h>
#include <cmath>
#include <algorithm>
#include <numbers>
#include "../../dependencies/vigem/setup/controller.h"

extern ImFont* PlatformIcons;
extern ID3D11ShaderResourceView* loot_stim;
extern ID3D11ShaderResourceView* loot_cash;
extern ID3D11ShaderResourceView* loot_crate;
extern ID3D11ShaderResourceView* loot_armor;
extern ID3D11ShaderResourceView* loot_heartbeat;
extern ID3D11ShaderResourceView* loot_grapple;
extern ID3D11ShaderResourceView* crate_common;
extern ID3D11ShaderResourceView* crate_rare;
extern ID3D11ShaderResourceView* crate_epic;
extern ID3D11ShaderResourceView* prestige_sets [4][12];
extern int prestige_set_counts [4];

namespace actor {

    struct actor_bounds {

        float min_x = 0.f, max_x = 0.f, min_y = 0.f, max_y = 0.f;
        Vector3 origin {};  
        Vector3 mins {};    
        Vector3 maxs {};    
        float   radius = 0.f; 
        float   height = 0.f;

        constexpr float screen_width ( ) const { return max_x - min_x; }
        constexpr float screen_height ( ) const { return max_y - min_y; }
        constexpr bool  has_screen_rect ( ) const {
            return min_x != 0.f || max_x != 0.f || min_y != 0.f || max_y != 0.f;
        }

        Vector3 center ( ) const {
            return { ( mins.x + maxs.x ) * 0.5f, ( mins.y + maxs.y ) * 0.5f, ( mins.z + maxs.z ) * 0.5f };
        }

        static actor_bounds from_world ( const Vector3& origin, float radius, float height ) {
            actor_bounds b {};
            b.origin = origin;
            b.radius = radius;
            b.height = height;
            b.mins = { origin.x - radius, origin.y - radius, origin.z };
            b.maxs = { origin.x + radius, origin.y + radius, origin.z + height };
            return b;
        }
    };

    inline float get_text_scale ( float distance, float base_scale = 1.0f ) {
        if ( !settings::visuals::text_scaling )
            return base_scale;

        const float min_scale = 0.7f;
        const float max_scale = 1.0f;
        const float max_distance = 150.0f;
        const float min_distance = 10.0f;

        float clamped_dist = std::clamp ( distance, min_distance, max_distance );
        float normalized = 1.0f - ( clamped_dist - min_distance ) / ( max_distance - min_distance );
        float scale = min_scale + normalized * ( max_scale - min_scale );

        return scale * base_scale;
    }

    inline ImVec2 calc_esp_text_size ( const char* text, float scale = 1.0f ) {
        float font_size = settings::visuals::text_size * scale;
        float ratio = font_size / ImGui::GetFont ( )->FontSize;
        ImVec2 s = ImGui::CalcTextSize ( text );
        return ImVec2 ( s.x * ratio, s.y * ratio );
    }

    inline void draw_text_outline ( ImDrawList* draw_list, ImVec2 pos, ImU32 color, const char* text, float scale = 1.0f ) {
        if ( !text || text [0] == '\0' ) return;

        const float font_size = settings::visuals::text_size * scale;
        const ImU32 shadow = IM_COL32 ( 0, 0, 0, 255 );

        switch ( settings::visuals::outline_type ) {
        case 1: // normal outline (8-direction)
            for ( float dx = -1.0f; dx <= 1.0f; dx++ )
                for ( float dy = -1.0f; dy <= 1.0f; dy++ )
                    if ( dx != 0.0f || dy != 0.0f )
                        draw_list->AddText ( nullptr, font_size, ImVec2 ( pos.x + dx, pos.y + dy ), shadow, text );
            break;
        case 2: // drop shadow (single offset)
            draw_list->AddText ( nullptr, font_size, ImVec2 ( pos.x + 1.0f, pos.y + 1.0f ), shadow, text );
            break;
        default: // 0 = no outline
            break;
        }

        draw_list->AddText ( nullptr, font_size, pos, color, text );
    }

    void draw_health_bar ( int health, const Vector4& bbox, float scale = 1.0f, bool visible = true ) {
        if ( health <= 0 )
            return;

        health = std::clamp ( health, 0, 100 );

        auto* draw = ImGui::GetBackgroundDrawList ( );

        const float bar_w = 2.5f;
        const float padding = 4.0f * scale;
        const float x = bbox.x - bar_w - padding;
        const float y = bbox.y;
        const float h = bbox.w;

        const float pct = health / 100.0f;
        const float fill_h = h * pct;
        const float fill_y = y + h - fill_h;

        const ImU32 outline_col = IM_COL32 ( 15, 16, 22, 255 );
        const ImU32 bg_col      = IM_COL32 ( 15, 16, 22, 150 );
        const auto& top_c = visible ? settings::visuals::gradient_top : settings::visuals::gradient_top_hidden;
        const auto& bot_c = visible ? settings::visuals::gradient_bottom : settings::visuals::gradient_bottom_hidden;
        const ImU32 full_col = ImGui::ColorConvertFloat4ToU32 ( top_c.Value );
        const ImU32 low_col  = ImGui::ColorConvertFloat4ToU32 ( bot_c.Value );

        draw->AddRectFilled ( ImVec2 ( x - 1, y - 1 ), ImVec2 ( x + bar_w + 1, y + h + 1 ), outline_col );
        draw->AddRectFilled ( ImVec2 ( x, y ), ImVec2 ( x + bar_w, y + h ), bg_col );
        draw->AddRectFilledMultiColor (
            ImVec2 ( x, fill_y ),
            ImVec2 ( x + bar_w, y + h ),
            full_col, full_col,
            low_col, low_col
        );

        if ( health < 100 ) {
            const float hp_font_sz = 9.0f;
            char hp_text [8];
            snprintf ( hp_text, sizeof ( hp_text ), "%d", health );
            ImFont* f = ImGui::GetFont ( );
            float ratio = hp_font_sz / f->FontSize;
            ImVec2 txt_sz ( f->CalcTextSizeA ( f->FontSize, FLT_MAX, 0.0f, hp_text ).x * ratio, hp_font_sz );
            float txt_x = x + ( bar_w * 0.5f ) - ( txt_sz.x * 0.5f );
            float txt_y = fill_y - txt_sz.y - 1.0f;
            draw->AddText ( f, hp_font_sz, ImVec2 ( txt_x + 1, txt_y + 1 ), IM_COL32 ( 0, 0, 0, 200 ), hp_text );
            draw->AddText ( f, hp_font_sz, ImVec2 ( txt_x, txt_y ), IM_COL32 ( 195, 200, 215, 255 ), hp_text );
        }
    }

    inline bool validate_bone_position ( const Vector3& bone, const Vector3& origin ) {
        if ( bone.x == 0.f && bone.y == 0.f && bone.z == 0.f )
            return false;

        float dist = origin.distance_to ( bone );
        if ( dist > 150.f )
            return false;

        return true;
    }

    inline actor_bounds get_bone_bounds ( const std::vector<Vector3>& bone_positions, const Vector3& origin, const RefDef_T& refdef, const Vector3& cam_pos ) {
        if ( bone_positions.empty ( ) ) return {};

        float min_x = FLT_MAX, max_x = -FLT_MAX;
        float min_y = FLT_MAX, max_y = -FLT_MAX;
        bool has_valid = false;

        for ( const auto& bone : bone_positions ) {
            if ( !validate_bone_position ( bone, origin ) )
                continue;

            Vector2 screen;
            if ( sdk::engine::player::w2s_cached ( bone, screen, refdef, cam_pos ) ) {
                if ( screen.x > 0 && screen.y > 0 ) {
                    min_x = min ( min_x, screen.x );
                    max_x = max ( max_x, screen.x );
                    min_y = min ( min_y, screen.y );
                    max_y = max ( max_y, screen.y );
                    has_valid = true;
                }
            }
        }

        if ( !has_valid ) return {};

        float box_height = max_y - min_y;
        float box_width = max_x - min_x;

        if ( box_height <= 0 || box_width <= 0 )
            return {};

        float width_offset = box_width * 0.175f;
        float height_offset_top = box_height * 0.125f;
        float height_offset_bottom = box_height * 0.05f;

        float min_x_out = min_x - width_offset;
        float max_x_out = max_x + width_offset;
        float width_pad = ( max_x_out - min_x_out ) * 0.125f;

        return { min_x_out - width_pad, max_x_out + width_pad, min_y - height_offset_top, max_y + height_offset_bottom };
    }

    inline void draw_box_fill_vignette ( ImDrawList* dl, float left, float top, float right, float bottom, ImColor color, bool visible = true ) {
        const auto& tc = visible ? settings::visuals::gradient_top : settings::visuals::gradient_top_hidden;
        const auto& bc = visible ? settings::visuals::gradient_bottom : settings::visuals::gradient_bottom_hidden;
        float r = tc.Value.x, g = tc.Value.y, b = tc.Value.z;
        float r2 = bc.Value.x, g2 = bc.Value.y, b2 = bc.Value.z;

        auto desat = [] ( float c, float avg ) { return c * 0.7f + avg * 0.3f; };

        float avg_t = ( r + g + b ) / 3.0f;
        ImU32 edge_top = IM_COL32 ( (int)(desat(r,avg_t)*255), (int)(desat(g,avg_t)*255), (int)(desat(b,avg_t)*255), 127 );

        float avg_b = ( r2 + g2 + b2 ) / 3.0f;
        ImU32 edge_bot = IM_COL32 ( (int)(desat(r2,avg_b)*255), (int)(desat(g2,avg_b)*255), (int)(desat(b2,avg_b)*255), 127 );

        ImU32 center_col = IM_COL32 ( 10, 10, 15, 20 );

        float mid_y = ( top + bottom ) * 0.5f;
        dl->AddRectFilledMultiColor ( ImVec2 ( left + 1, top + 1 ), ImVec2 ( right - 1, mid_y ), edge_top, edge_top, center_col, center_col );
        dl->AddRectFilledMultiColor ( ImVec2 ( left + 1, mid_y ), ImVec2 ( right - 1, bottom - 1 ), center_col, center_col, edge_bot, edge_bot );
    }

    inline void draw_box ( ImDrawList* draw_list, float left, float top, float right, float bottom, float width, float height, ImColor color, int box_type, bool outline, float scale = 1.0f ) {
        switch ( box_type ) {
        case 0: {
            float corner_w = width / 4.0f;
            float corner_h = height / 4.5f;

            auto corner_line = [&] ( ImVec2 a, ImVec2 b ) {
                if ( outline ) {
                    draw_list->AddLine ( a, b, IM_COL32 ( 0, 0, 0, 255 ), 3.0f * scale );
                }
                draw_list->AddLine ( a, b, color, 1.5f * scale );
                };

            corner_line ( ImVec2 ( left, top ), ImVec2 ( left + corner_w, top ) );
            corner_line ( ImVec2 ( left, top ), ImVec2 ( left, top + corner_h ) );
            corner_line ( ImVec2 ( right - corner_w, top ), ImVec2 ( right, top ) );
            corner_line ( ImVec2 ( right, top ), ImVec2 ( right, top + corner_h ) );
            corner_line ( ImVec2 ( left, bottom - corner_h ), ImVec2 ( left, bottom ) );
            corner_line ( ImVec2 ( left, bottom ), ImVec2 ( left + corner_w, bottom ) );
            corner_line ( ImVec2 ( right, bottom - corner_h ), ImVec2 ( right, bottom ) );
            corner_line ( ImVec2 ( right - corner_w, bottom ), ImVec2 ( right, bottom ) );
            break;
        }
        case 1: {
            if ( outline ) {
                draw_list->AddRect ( ImVec2 ( left, top ), ImVec2 ( right, bottom ), IM_COL32 ( 0, 0, 0, 255 ), 0.0f, 0, 3.0f * scale );
            }
            draw_list->AddRect ( ImVec2 ( left, top ), ImVec2 ( right, bottom ), color, 0.0f, 0, 1.5f * scale );
            break;
        }
        }
    }

    inline void draw_china_hat ( ImDrawList* draw_list, const Vector3& head_pos, const RefDef_T& refdef, const Vector3& cam_pos, ImColor color, float scale = 1.0f ) {
        Vector3 tip_3d = { head_pos.x, head_pos.y, head_pos.z + 22.f };
        Vector2 screen_tip;

        if ( !sdk::engine::player::w2s_cached ( tip_3d, screen_tip, refdef, cam_pos ) )
            return;

        if ( screen_tip.x <= 0 || screen_tip.y <= 0 )
            return;

        std::vector<ImVec2> base_points_2d;
        base_points_2d.reserve ( 23 );

        const float angle_step = 2.0f * std::numbers::pi / 23;
        const float radius = 17.f * scale;
        const float height_offset = 9.f * scale;

        for ( int i = 0; i < 23; ++i ) {
            const float angle = angle_step * i;
            Vector3 base_point_3d = {
                head_pos.x + std::cos ( angle ) * radius,
                head_pos.y + std::sin ( angle ) * radius,
                head_pos.z + height_offset
            };

            Vector2 screen_base;
            if ( sdk::engine::player::w2s_cached ( base_point_3d, screen_base, refdef, cam_pos ) ) {
                if ( screen_base.x > 0 && screen_base.y > 0 ) {
                    base_points_2d.push_back ( ImVec2 ( screen_base.x, screen_base.y ) );
                }
                else {
                    base_points_2d.push_back ( ImVec2 ( -FLT_MAX, -FLT_MAX ) );
                }
            }
            else {
                base_points_2d.push_back ( ImVec2 ( -FLT_MAX, -FLT_MAX ) );
            }
        }

        float line_thickness = 1.5f * scale;

        for ( size_t i = 0; i < 23; ++i ) {
            const auto& p1 = base_points_2d [i];
            const auto& p2 = base_points_2d [( i + 1 ) % 23];
            if ( p1.x != -FLT_MAX && p2.x != -FLT_MAX ) {
                draw_list->AddLine ( p1, p2, color, line_thickness );
            }
        }

        ImVec2 tip_2d ( screen_tip.x, screen_tip.y );
        for ( const auto& base_point : base_points_2d ) {
            if ( base_point.x != -FLT_MAX ) {
                draw_list->AddLine ( tip_2d, base_point, color, line_thickness );
            }
        }
    }

    inline ImU32 gradient_color_at ( float screen_y, float bbox_top, float bbox_bottom, bool visible = true ) {
        float t = std::clamp ( ( screen_y - bbox_top ) / ( bbox_bottom - bbox_top + 0.001f ), 0.0f, 1.0f );
        t = t * t * ( 3.0f - 2.0f * t );
        const ImVec4& top_c = visible ? settings::visuals::gradient_top.Value : settings::visuals::gradient_top_hidden.Value;
        const ImVec4& bot_c = visible ? settings::visuals::gradient_bottom.Value : settings::visuals::gradient_bottom_hidden.Value;
        ImVec4 c = ImLerp ( top_c, bot_c, t );
        return ImGui::ColorConvertFloat4ToU32 ( c );
    }

    inline void draw_gradient_line_seg ( ImDrawList* dl, ImVec2 a, ImVec2 b,
        ImU32 col_a, ImU32 col_b, float thickness, int segments = 8 ) {
        ImVec4 ca = ImGui::ColorConvertU32ToFloat4 ( col_a );
        ImVec4 cb = ImGui::ColorConvertU32ToFloat4 ( col_b );
        for ( int i = 0; i < segments; ++i ) {
            float t0 = ( float ) i / segments;
            float t1 = ( float ) ( i + 1 ) / segments;
            ImVec2 p0 = ImLerp ( a, b, t0 );
            ImVec2 p1 = ImLerp ( a, b, t1 );
            ImVec4 cm = ImLerp ( ca, cb, ( t0 + t1 ) * 0.5f );
            dl->AddLine ( p0, p1, ImGui::ColorConvertFloat4ToU32 ( cm ), thickness );
        }
    }


    inline void draw_box_gradient ( ImDrawList* draw_list, float left, float top, float right, float bottom,
        float width, float height, int box_type, bool outline, float scale, float bbox_top, float bbox_bottom, bool visible = true ) {

        ImU32 col_top    = gradient_color_at ( top, bbox_top, bbox_bottom, visible );
        ImU32 col_bottom = gradient_color_at ( bottom, bbox_top, bbox_bottom, visible );
        float thick      = 1.5f * scale;
        float thick_ol   = 3.0f * scale;

        auto corner_grad = [&] ( ImVec2 a, ImVec2 b ) {
            ImU32 ca = gradient_color_at ( a.y, bbox_top, bbox_bottom, visible );
            ImU32 cb = gradient_color_at ( b.y, bbox_top, bbox_bottom, visible );
            if ( outline )
                draw_gradient_line_seg ( draw_list, a, b, IM_COL32 ( 0, 0, 0, 255 ), IM_COL32 ( 0, 0, 0, 255 ), thick_ol );
            draw_gradient_line_seg ( draw_list, a, b, ca, cb, thick );
        };

        switch ( box_type ) {
        case 0: {
            float cw = width / 4.0f;
            float ch = height / 4.5f;
            corner_grad ( ImVec2 ( left, top ), ImVec2 ( left + cw, top ) );
            corner_grad ( ImVec2 ( left, top ), ImVec2 ( left, top + ch ) );
            corner_grad ( ImVec2 ( right - cw, top ), ImVec2 ( right, top ) );
            corner_grad ( ImVec2 ( right, top ), ImVec2 ( right, top + ch ) );
            corner_grad ( ImVec2 ( left, bottom - ch ), ImVec2 ( left, bottom ) );
            corner_grad ( ImVec2 ( left, bottom ), ImVec2 ( left + cw, bottom ) );
            corner_grad ( ImVec2 ( right, bottom - ch ), ImVec2 ( right, bottom ) );
            corner_grad ( ImVec2 ( right - cw, bottom ), ImVec2 ( right, bottom ) );
            break;
        }
        case 1: {
            if ( outline ) {
                draw_list->AddLine ( ImVec2 ( left, top ), ImVec2 ( right, top ), IM_COL32 ( 0, 0, 0, 255 ), thick_ol );
                draw_list->AddLine ( ImVec2 ( left, bottom ), ImVec2 ( right, bottom ), IM_COL32 ( 0, 0, 0, 255 ), thick_ol );
                draw_gradient_line_seg ( draw_list, ImVec2 ( left, top ), ImVec2 ( left, bottom ), IM_COL32 ( 0, 0, 0, 255 ), IM_COL32 ( 0, 0, 0, 255 ), thick_ol );
                draw_gradient_line_seg ( draw_list, ImVec2 ( right, top ), ImVec2 ( right, bottom ), IM_COL32 ( 0, 0, 0, 255 ), IM_COL32 ( 0, 0, 0, 255 ), thick_ol );
            }
            draw_list->AddLine ( ImVec2 ( left, top ), ImVec2 ( right, top ), col_top, thick );
            draw_list->AddLine ( ImVec2 ( left, bottom ), ImVec2 ( right, bottom ), col_bottom, thick );
            draw_gradient_line_seg ( draw_list, ImVec2 ( left, top ), ImVec2 ( left, bottom ), col_top, col_bottom, thick, 12 );
            draw_gradient_line_seg ( draw_list, ImVec2 ( right, top ), ImVec2 ( right, bottom ), col_top, col_bottom, thick, 12 );
            break;
        }
        }
    }

    inline void draw_offscreen_arrow ( ImDrawList* dl, const Vector3& entity_pos, const Vector3& cam_pos, const RefDef_T& refdef, bool visible ) {
        Vector3 delta = entity_pos - cam_pos;

        float rx = delta.Dot ( refdef.axis [1] );
        float ry = -delta.Dot ( refdef.axis [2] );
        float rz = delta.Dot ( refdef.axis [0] );

        if ( rz < 0.0f ) { rx = -rx; ry = -ry; }

        float len = sqrtf ( rx * rx + ry * ry );
        if ( len < 0.001f ) return;
        rx /= len; ry /= len;

        ImVec2 display = ImGui::GetIO ( ).DisplaySize;
        float scx = display.x * 0.5f, scy = display.y * 0.5f;

        float orbit_r = settings::aimbot::fov + 20.0f;
        ImVec2 tip ( scx + rx * orbit_r, scy + ry * orbit_r );

        float arrow_size = 10.0f;
        float angle = atan2f ( ry, rx );
        float a1 = angle + 2.5f, a2 = angle - 2.5f;
        ImVec2 p1 ( tip.x + cosf(a1)*arrow_size, tip.y + sinf(a1)*arrow_size );
        ImVec2 p2 ( tip.x + cosf(a2)*arrow_size, tip.y + sinf(a2)*arrow_size );

        const auto& tc = visible ? settings::visuals::gradient_top : settings::visuals::gradient_top_hidden;
        ImU32 col = ImGui::ColorConvertFloat4ToU32 ( ImVec4 ( tc.Value.x, tc.Value.y, tc.Value.z, 0.85f ) );

        dl->AddTriangle ( tip, p1, p2, IM_COL32(0,0,0,180), 2.0f );
        dl->AddTriangleFilled ( tip, p1, p2, col );
    }

    inline constexpr ImU32 k_outline = IM_COL32 ( 0, 0, 0, 255 );

    inline void drawtext ( ImDrawList* draw_list, ImVec2 pos, ImU32 color, const char* text, float scale = 1.0f ) {
        draw_text_outline ( draw_list, pos, color, text, scale );
    }

    inline void draw_outlined_shit ( ImDrawList* draw_list, ImVec2 pos, ImColor color, const char* text, float scale = 1.0f ) {
        drawtext ( draw_list, pos, static_cast< ImU32 >( color ), text, scale );
    }

    void loop ( ) {
        const auto& entities = cache::get_entities ( );

        if ( entities.empty ( ) ) {
            return;
        }

        const sdk::gInfo* game = cache::get_game_info ( );
        if ( !game || !game->inGame ) {
            return;
        }

        const RefDef_T& refdef = game->refdef;
        Vector3 cam_pos = cache::get_camera_position ( );

        ImDrawList* draw_list = ImGui::GetBackgroundDrawList ( );

        bool game_focused = false;
        {
            HWND fg = GetForegroundWindow ( );
            if ( fg ) {
                DWORD fg_pid = 0;
                GetWindowThreadProcessId ( fg, &fg_pid );
                game_focused = ( fg_pid == g_vm->m_pid );
            }
        }
        const bool primary_down = game_focused && settings::aimbot::aimbot_key.is_down.load ( std::memory_order_relaxed );

        static int sticky_target = -1;
        static bool was_primary_down = false;

        if ( primary_down && !was_primary_down ) {
            sticky_target = -1;
        }

        if ( !primary_down ) {
            sticky_target = -1;
        }

        float best_dist = FLT_MAX;
        int best_index = -1;
        Vector3 best_pos;
        Vector2 best_screen;

        for ( const auto& ent : entities ) {

            if ( ent.distance > settings::visuals::esp_distance )
                continue;

            if ( ent.i == game->index )
                continue;

            if ( ent.isDead )
                continue;

            if ( ent.team == game->localTeam )
                continue;

            Vector2 screenHead, screenFeet;


            if ( !ent.bonePositions.empty ( ) && ent.bonePositions.size ( ) > 7 ) {
                if ( !sdk::engine::player::w2s_cached ( ent.bonePositions [7], screenHead, refdef, cam_pos ) ) {
                    Vector3 headPos = ent.position;
                    headPos.z += 70.0f;
                    if ( !sdk::engine::player::w2s_cached ( headPos, screenHead, refdef, cam_pos ) ) {
                        if ( settings::visuals::offscreen_arrows )
                            draw_offscreen_arrow ( draw_list, ent.position, cam_pos, refdef, ent.isVisible );
                        continue;
                    }
                }
            }
            else {
                Vector3 headPos = ent.position;
                headPos.z += 70.0f;
                if ( !sdk::engine::player::w2s_cached ( headPos, screenHead, refdef, cam_pos ) ) {
                    if ( settings::visuals::offscreen_arrows )
                        draw_offscreen_arrow ( draw_list, ent.position, cam_pos, refdef, ent.isVisible );
                    continue;
                }
            }

            Vector3 feetPos = ent.position;
            if ( !sdk::engine::player::w2s_cached ( feetPos, screenFeet, refdef, cam_pos ) ) {
                if ( settings::visuals::offscreen_arrows )
                    draw_offscreen_arrow ( draw_list, ent.position, cam_pos, refdef, ent.isVisible );
                continue;
            }

            if ( ent.bonePositions.empty ( ) )
                continue;

            actor_bounds bounds = get_bone_bounds ( ent.bonePositions, ent.position, refdef, cam_pos );
            if ( !bounds.has_screen_rect ( ) )
                continue;

            {
                actor_bounds world = actor_bounds::from_world ( ent.position, ent.boundsRadius, ent.boundsHeight );
                bounds.origin = world.origin;
                bounds.mins = world.mins;
                bounds.maxs = world.maxs;
                bounds.radius = world.radius;
                bounds.height = world.height;
            }

            float left   = bounds.min_x;
            float right  = bounds.max_x;
            float top    = bounds.min_y;
            float bottom = bounds.max_y;
            float width  = right - left;
            float height = bottom - top;

            if ( width <= 0 || height <= 0 )
                continue;

            float text_scale = get_text_scale ( ent.distance );
            ImColor render_color = ent.isVisible ? ImColor ( 255, 255, 255, 255 ) : ImColor ( 255, 0, 0, 255 );
            Vector3 local_vel = sdk::engine::prediction::get_speed ( game->index );

            std::string displayName;
            if ( ent.nameEntry.clanAbbrev [0] != '\0' ) {
                displayName = std::string ( ent.nameEntry.name ) + " [" + std::string ( ent.nameEntry.clanAbbrev ) + "]";
            }
            else {
                displayName = std::string ( ent.nameEntry.name );
            }

            float top_off = 0, bot_off = 0, left_off = 0, right_off = 0;

            auto place_text_col = [&] ( const char* txt, int pos, float ts, ImU32 col ) {
                ImVec2 sz = calc_esp_text_size ( txt, ts );
                ImVec2 p;
                switch ( pos ) {
                case 0: p = ImVec2 ( left + width*0.5f - sz.x*0.5f, top - sz.y - 2 - top_off ); top_off += sz.y + 1; break;
                case 1: p = ImVec2 ( left + width*0.5f - sz.x*0.5f, bottom + 2 + bot_off ); bot_off += sz.y + 1; break;
                case 2: p = ImVec2 ( left - sz.x - 6, top + left_off ); left_off += sz.y + 1; break;
                case 3: p = ImVec2 ( right + 6, top + right_off ); right_off += sz.y + 1; break;
                default: p = ImVec2 ( left + width*0.5f - sz.x*0.5f, top - sz.y - 2 - top_off ); top_off += sz.y + 1; break;
                }
                draw_text_outline ( draw_list, p, col, txt, ts );
            };

            auto place_text = [&] ( const char* txt, int pos, float ts ) {
                place_text_col ( txt, pos, ts, IM_COL32 ( 255, 255, 255, 220 ) );
            };

            if ( settings::visuals::username && !displayName.empty ( ) && displayName != "unknown" )
                place_text ( displayName.c_str ( ), settings::visuals::username_pos, text_scale );

            if ( settings::visuals::level ) {
                std::string levelText = "level: " + std::to_string ( ent.nameEntry.rank_mp );
                place_text ( levelText.c_str ( ), settings::visuals::level_pos, text_scale );
            }

            if ( settings::visuals::show_prestige && ent.nameEntry.prestige_mp >= 1 ) {
                int sty = std::clamp ( settings::visuals::prestige_style, 0, 3 );
                int prest_idx = std::clamp ( ent.nameEntry.prestige_mp, 1, prestige_set_counts [sty] ) - 1;
                ID3D11ShaderResourceView* picon = prestige_sets [sty][prest_idx];
                if ( picon ) {
                    float dist_norm = std::clamp ( ( ent.distance - 10.0f ) / 140.0f, 0.0f, 1.0f );
                    float icon_sz = 22.0f - dist_norm * 12.0f;
                    float pad = 3.0f;
                    int pos = settings::visuals::level ? settings::visuals::level_pos : settings::visuals::username_pos;
                    ImVec2 p;
                    switch ( pos ) {
                    case 0: p = ImVec2 ( left + width * 0.5f - icon_sz * 0.5f, top - icon_sz - 2 - top_off ); top_off += icon_sz + pad * 2; break;
                    case 1: p = ImVec2 ( left + width * 0.5f - icon_sz * 0.5f, bottom + 2 + bot_off ); bot_off += icon_sz + pad * 2; break;
                    case 2: p = ImVec2 ( left - icon_sz - 6, top + left_off ); left_off += icon_sz + pad * 2; break;
                    case 3: p = ImVec2 ( right + 6, top + right_off ); right_off += icon_sz + pad * 2; break;
                    default: p = ImVec2 ( left + width * 0.5f - icon_sz * 0.5f, top - icon_sz - 2 - top_off ); top_off += icon_sz + pad * 2; break;
                    }
                    if ( settings::visuals::prestige_bg )
                        draw_list->AddRectFilled ( ImVec2 ( p.x - pad, p.y - pad ), ImVec2 ( p.x + icon_sz + pad, p.y + icon_sz + pad ), IM_COL32 ( 0, 0, 0, 140 ), 3.0f );
                    draw_list->AddImage ( ( ImTextureID ) picon, p, ImVec2 ( p.x + icon_sz, p.y + icon_sz ) );
                }
            }

            if ( settings::visuals::box ) {
                if ( settings::visuals::box_fill )
                    draw_box_fill_vignette ( draw_list, left, top, right, bottom, ImColor ( 1.0f, 1.0f, 1.0f ), ent.isVisible );

                draw_box_gradient ( draw_list, left, top, right, bottom, width, height, settings::visuals::box_type, settings::visuals::box_outline, text_scale, top, bottom, ent.isVisible );
            }

            if ( settings::visuals::china_hat ) {
                if ( !ent.bonePositions.empty ( ) && ent.bonePositions.size ( ) > 7 ) {
                    Vector3 head_pos = ent.bonePositions [7];
                    if ( validate_bone_position ( head_pos, ent.position ) ) {
                        ImU32 hat_col = gradient_color_at ( top, top, bottom, ent.isVisible );
                        draw_china_hat ( draw_list, head_pos, refdef, cam_pos, ImColor ( hat_col ), text_scale );
                    }
                }
            }

            if ( settings::visuals::health && ent.health > 0 ) {
                int hp = settings::visuals::health_pos;
                switch ( hp ) {
                case 0: draw_health_bar ( ent.health, { left, top - 8, width, 3 }, text_scale, ent.isVisible ); break;
                case 1: draw_health_bar ( ent.health, { left, bottom + 4, width, 3 }, text_scale, ent.isVisible ); break;
                case 3: draw_health_bar ( ent.health, { right + 6, top, 3, height }, text_scale, ent.isVisible ); break;
                default: draw_health_bar ( ent.health, { left, top, width, height }, text_scale, ent.isVisible ); break;
                }
            }

            if ( settings::visuals::weapon ) {
                std::string distanceText = ent.weaponName + " [" + std::to_string ( ( int ) ent.distance ) + "m]";
                place_text ( distanceText.c_str ( ), settings::visuals::weapon_pos, text_scale );
            }

            if ( settings::visuals::show_kills ) {
                char kd_buf [32];
                snprintf ( kd_buf, sizeof(kd_buf), "%uK / %uD", ent.kills, ent.deaths );
                place_text ( kd_buf, settings::visuals::kills_pos, text_scale );
            }

            if ( settings::visuals::show_platform ) {
                const char* icon = "A";

                if ( PlatformIcons ) {
                    float dist_norm = std::clamp ( ( ent.distance - 10.0f ) / 140.0f, 0.0f, 1.0f );
                    float icon_sz = 12.0f - dist_norm * 5.0f;
                    ImVec2 sz ( icon_sz, icon_sz );
                    ImVec2 p;
                    int pos = settings::visuals::platform_pos;
                    switch ( pos ) {
                    case 0: p = ImVec2 ( left + width * 0.5f - sz.x * 0.5f, top - sz.y - 2 - top_off ); top_off += sz.y + 1; break;
                    case 1: p = ImVec2 ( left + width * 0.5f - sz.x * 0.5f, bottom + 2 + bot_off ); bot_off += sz.y + 1; break;
                    case 2: p = ImVec2 ( left - sz.x - 6, top + left_off ); left_off += sz.y + 1; break;
                    case 3: p = ImVec2 ( right + 6, top + right_off ); right_off += sz.y + 1; break;
                    default: p = ImVec2 ( left + width * 0.5f - sz.x * 0.5f, top - sz.y - 2 - top_off ); top_off += sz.y + 1; break;
                    }

                    const auto& tc = ent.isVisible ? settings::visuals::gradient_top : settings::visuals::gradient_top_hidden;
                    ImU32 black = IM_COL32 ( 0, 0, 0, 255 );

                    switch ( settings::visuals::outline_type ) {
                    case 1:
                        for ( float dx = -1.0f; dx <= 1.0f; dx++ )
                            for ( float dy = -1.0f; dy <= 1.0f; dy++ )
                                if ( dx != 0.0f || dy != 0.0f )
                                    draw_list->AddText ( PlatformIcons, icon_sz, ImVec2 ( p.x + dx, p.y + dy ), black, icon );
                        break;
                    case 2:
                        draw_list->AddText ( PlatformIcons, icon_sz, ImVec2 ( p.x + 1, p.y + 1 ), black, icon );
                        break;
                    }
                    draw_list->AddText ( PlatformIcons, icon_sz, p, tc, icon );
                }
            }

            if ( settings::visuals::skeleton ) {
                const auto& bone_positions = ent.bonePositions;
                if ( bone_positions.size ( ) >= 14 ) {
                    float thickness = 1.5f * text_scale;
                    const bool do_outline = settings::visuals::skeleton_outline;
                    const float bbox_h = ( bottom - top ) + 0.001f;

                    auto get_bone_screen = [&] ( int idx, Vector2& out ) -> bool {
                        if ( idx >= ( int ) bone_positions.size ( ) ) return false;
                        if ( !validate_bone_position ( bone_positions [idx], ent.position ) ) return false;
                        return sdk::engine::player::w2s_cached ( bone_positions [idx], out, refdef, cam_pos )
                            && out.x > 0 && out.y > 0;
                    };

                    auto grad_col = [&] ( float screen_y ) -> ImU32 {
                        return gradient_color_at ( screen_y, top, bottom, ent.isVisible );
                    };

                    auto skel_line = [&] ( int i0, int i1 ) {
                        Vector2 s0, s1;
                        if ( !get_bone_screen ( i0, s0 ) || !get_bone_screen ( i1, s1 ) ) return;
                        ImVec2 a ( s0.x, s0.y ), b ( s1.x, s1.y );
                        ImU32 ca = grad_col ( s0.y ), cb = grad_col ( s1.y );
                        if ( do_outline )
                            draw_gradient_line_seg ( draw_list, a, b, IM_COL32 ( 0, 0, 0, 255 ), IM_COL32 ( 0, 0, 0, 255 ), thickness * 2.0f );
                        draw_gradient_line_seg ( draw_list, a, b, ca, cb, thickness );
                    };

                    skel_line ( 3, 0 );
                    skel_line ( 1, 3 );
                    skel_line ( 1, 10 );  skel_line ( 10, 11 );
                    skel_line ( 1, 12 );  skel_line ( 12, 13 );
                    skel_line ( 3, 4 );   skel_line ( 4, 5 );   skel_line ( 5, 6 );
                    skel_line ( 3, 7 );   skel_line ( 7, 8 );   skel_line ( 8, 9 );
                }
            }

            if ( settings::visuals::snaplines ) {
                float sx = ImGui::GetIO().DisplaySize.x * 0.5f;
                float sy = settings::visuals::snapline_type == 0 ? ImGui::GetIO().DisplaySize.y : ImGui::GetIO().DisplaySize.y * 0.5f;
                float tx = ( left + right ) * 0.5f;
                float ty = bottom;
                ImU32 c_start = gradient_color_at ( sy, top, bottom, ent.isVisible );
                ImU32 c_end   = gradient_color_at ( ty, top, bottom, ent.isVisible );
                draw_list->AddLine ( ImVec2(sx,sy), ImVec2(tx,ty), c_end, 1.0f );
            }

            if ( settings::visuals::aim_direction && ent.bonePositions.size() >= 14 ) {
                Vector3 chest = ent.bonePositions[2];
                Vector3 head  = ent.bonePositions[0];
                if ( validate_bone_position(chest, ent.position) && validate_bone_position(head, ent.position) ) {
                    Vector3 dir = { head.x - chest.x, head.y - chest.y, 0 };
                    float len = sqrtf(dir.x*dir.x + dir.y*dir.y);
                    if ( len > 0.1f ) {
                        dir.x /= len; dir.y /= len;
                        Vector3 aim_end = { head.x + dir.x * 40.0f, head.y + dir.y * 40.0f, head.z };
                        Vector2 s_head, s_end;
                        if ( sdk::engine::player::w2s_cached(head, s_head, refdef, cam_pos) &&
                             sdk::engine::player::w2s_cached(aim_end, s_end, refdef, cam_pos) ) {
                            ImU32 c0 = gradient_color_at ( s_head.y, top, bottom, ent.isVisible );
                            ImU32 c1 = gradient_color_at ( s_end.y, top, bottom, ent.isVisible );
                            draw_list->AddLine(ImVec2(s_head.x,s_head.y), ImVec2(s_end.x,s_end.y), c0, 1.5f);
                        }
                    }
                }
            }

            if ( settings::aimbot::enabled && primary_down ) {
                if ( ent.i == game->index ) continue;
                if ( ent.distance > settings::aimbot::aim_distance ) continue;

                if ( settings::aimbot::visible_check && !ent.isVisible ) continue;

                bool is_sticky_candidate = settings::aimbot::sticky_aim && sticky_target != -1 && ent.i == sticky_target;

                if ( is_sticky_candidate && ent.isDead ) {
                    sticky_target = -1;
                    is_sticky_candidate = false;
                }

                Vector3 target_bone = ent.position;

                if ( !ent.bonePositions.empty ( ) ) {
                    const int region_bone [] = { 7, 6, 5, 3, 2, 4, 8, 10, 12 };
                    float scx = ImGui::GetIO ( ).DisplaySize.x * 0.5f;
                    float scy = ImGui::GetIO ( ).DisplaySize.y * 0.5f;

                    int pick = -1;
                    float pick_dist = FLT_MAX;
                    int pick_prio = 99;

                    for ( int ri = 0; ri < 9; ++ri ) {
                        int p = settings::aimbot::hitbox [ri];
                        if ( p <= 0 ) continue;
                        int idx = region_bone [ri];
                        if ( idx >= ( int ) ent.bonePositions.size ( ) ) continue;
                        if ( !validate_bone_position ( ent.bonePositions [idx], ent.position ) ) continue;

                        Vector2 scr;
                        if ( !sdk::engine::player::w2s_cached ( ent.bonePositions [idx], scr, refdef, cam_pos ) ) continue;

                        float d = std::sqrt ( ( scr.x - scx ) * ( scr.x - scx ) + ( scr.y - scy ) * ( scr.y - scy ) );

                        if ( d < pick_dist - 5.0f || ( d < pick_dist + 5.0f && p < pick_prio ) ) {
                            pick_dist = d;
                            pick_prio = p;
                            pick = idx;
                        }
                    }

                    if ( pick >= 0 )
                        target_bone = ent.bonePositions [pick];
                    else {
                        target_bone = ent.position;
                        target_bone.z += 70.0f;
                    }
                }
                else {
                    target_bone = ent.position;
                    target_bone.z += 70.0f;
                }

                if ( settings::aimbot::prediction ) {
                    Vector3 pred = sdk::engine::prediction::get_prediction_players ( ent.i, target_bone, game->localPos, local_vel );
                    if ( std::isfinite ( pred.x ) && std::isfinite ( pred.y ) && std::isfinite ( pred.z ) )
                        target_bone = pred;
                }

                Vector2 screen_pos;
                if ( !sdk::engine::player::w2s_cached ( target_bone, screen_pos, refdef, cam_pos ) )
                    continue;

                float screen_center_x = ImGui::GetIO ( ).DisplaySize.x / 2.0f;
                float screen_center_y = ImGui::GetIO ( ).DisplaySize.y / 2.0f;
                float dx = screen_pos.x - screen_center_x;
                float dy = screen_pos.y - screen_center_y;
                float dist = std::sqrt ( dx * dx + dy * dy );

                if ( dist > settings::aimbot::fov ) {
                    if ( is_sticky_candidate ) {
                        sticky_target = -1;
                    }
                    continue;
                }

                if ( is_sticky_candidate ) {
                    best_dist = dist;
                    best_index = ent.i;
                    best_pos = target_bone;
                    best_screen = screen_pos;
                }
                else if ( sticky_target == -1 || !settings::aimbot::sticky_aim ) {
                    if ( dist < best_dist ) {
                        best_dist = dist;
                        best_index = ent.i;
                        best_pos = target_bone;
                        best_screen = screen_pos;
                    }
                }
            }
        }

        if ( settings::aimbot::sticky_aim && primary_down && best_index != -1 ) {
            sticky_target = best_index;
        }

        was_primary_down = primary_down;

        bool sent_input = false;

        if ( settings::aimbot::enabled && primary_down && best_index != -1 ) {
            float screen_center_x = ImGui::GetIO ( ).DisplaySize.x / 2.0f;
            float screen_center_y = ImGui::GetIO ( ).DisplaySize.y / 2.0f;

            float dx = best_screen.x - screen_center_x;
            float dy = best_screen.y - screen_center_y;
            float dist = std::sqrt ( dx * dx + dy * dy );

            if ( dist > settings::aimbot::deadzone ) {
                float smoothness = settings::aimbot::smoothness;
                float distance_factor = min ( 1.0f, dist / 200.0f );
                float dynamic_smooth = smoothness * ( 0.8f + 0.4f * distance_factor );
                float smooth_v = max ( 1.0f, 1.5f + dynamic_smooth );

                float move_x = dx / smooth_v;
                float move_y = dy / smooth_v;

                if ( settings::aimbot::bezier_aim ) {
                    static float curve_dir = 1.0f;
                    static int last_target = -1;
                    if ( best_index != last_target ) {
                        curve_dir = ( ( best_index * 7 + ( int )( ImGui::GetTime ( ) * 100 ) ) % 3 - 1 ) >= 0 ? 1.0f : -1.0f;
                        last_target = best_index;
                    }

                    float norm = ( dist > 0.001f ) ? dist : 1.0f;
                    float perp_x = -dy / norm;
                    float perp_y = dx / norm;
                    float curve = settings::aimbot::curve_strength;
                    float falloff = dist / ( dist + 30.0f );
                    float offset = std::sin ( dist * 0.08f ) * dist * curve * falloff * curve_dir;
                    move_x += perp_x * offset;
                    move_y += perp_y * offset;
                }

                move_x = std::clamp ( move_x, -20.0f, 20.0f );
                move_y = std::clamp ( move_y, -20.0f, 20.0f );

                if ( std::abs ( move_x ) > 0.5f || std::abs ( move_y ) > 0.5f ) {
                    if ( !settings::aimbot::controller_support ) {
                        g_vm->move_mouse ( static_cast< int32_t >( move_x ), static_cast< int32_t >( move_y ), 0 );
                    }
                    else {
                        float max_stick = 32767.0f;
                        vigem::move ( move_x / 20.0f * max_stick, -move_y / 20.0f * max_stick );
                    }
                    sent_input = true;
                }
            }
        }

        if ( !sent_input && settings::aimbot::controller_support ) {
            vigem::clear_override ( );
        }

    }

    inline void render_loot ( ImDrawList* dl, const RefDef_T& refdef, const Vector3& cam_pos ) {
        if ( !settings::loot::draw_loot ) return;

        const auto& loot = cache::get_loot ( );

        for ( const auto& item : loot ) {
            if ( !item.valid || item.name.empty ( ) ) continue;
            if ( item.distance > ( float ) settings::loot::max_distance ) continue;

            Vector2 screen;
            if ( !sdk::engine::player::w2s_cached ( item.position, screen, refdef, cam_pos ) ) continue;
            if ( screen.x <= 0 || screen.y <= 0 ) continue;

            const std::string& n = item.name;
            const char* label = nullptr;
            const auto& tc = settings::visuals::gradient_top;
            ImU32 col = ImGui::ColorConvertFloat4ToU32 ( tc.Value );

            auto has = [&] ( const char* s ) { return n.find ( s ) != std::string::npos; };

            if ( has ( "tag_origin" ) || n == "Unknown" || n.empty ( ) ) continue;

            if ( settings::loot::weapons && ( has ( "wpn_" ) || has ( "wm_" ) || has ( "WEAPON" ) || has ( "sat_" ) ) ) {
                std::string clean = sdk::engine::loot::clean_loot_name ( n );
                static thread_local char buf [128];
                snprintf ( buf, sizeof ( buf ), "%s [%dm]", clean.c_str ( ), ( int ) item.distance );
                label = buf;

                std::string cl = clean;
                for ( auto& c : cl ) c = ( char ) tolower ( ( unsigned char ) c );
                auto whas = [&] ( const char* s ) { return cl.find ( s ) != std::string::npos; };

                if ( whas ( "sniper" ) || whas ( "svd" ) || whas ( "lw3" ) || whas ( "lr 7" ) )
                    col = IM_COL32 ( 255, 80, 80, 255 );
                else if ( whas ( "marksman" ) || whas ( "sirin" ) )
                    col = IM_COL32 ( 255, 140, 60, 255 );
                else if ( whas ( "lmg" ) || whas ( "xmg" ) || whas ( "pu-21" ) )
                    col = IM_COL32 ( 255, 200, 50, 255 );
                else if ( whas ( "smg" ) || whas ( "c9" ) || whas ( "pp-" ) || whas ( "jackal" ) || whas ( "kompakt" ) || whas ( "saug" ) || whas ( "ksx" ) )
                    col = IM_COL32 ( 50, 200, 255, 255 );
                else if ( whas ( "pistol" ) || whas ( "gs45" ) || whas ( "stryder" ) || whas ( "grekhova" ) )
                    col = IM_COL32 ( 200, 160, 255, 255 );
                else if ( whas ( "shotgun" ) || whas ( "marine" ) || whas ( "maelstrom" ) || whas ( "asz" ) )
                    col = IM_COL32 ( 255, 160, 30, 255 );
                else if ( whas ( "launcher" ) )
                    col = IM_COL32 ( 255, 60, 60, 255 );
                else if ( whas ( "knife" ) || whas ( "melee" ) )
                    col = IM_COL32 ( 180, 180, 180, 255 );
                else
                    col = IM_COL32 ( 100, 255, 100, 255 );
            }
            else if ( settings::loot::ammo && has ( "ammo" ) ) {
                static thread_local char buf [64];
                snprintf ( buf, sizeof ( buf ), "Ammo [%dm]", ( int ) item.distance );
                label = buf;
                col = IM_COL32 ( 255, 200, 80, 255 );
            }
            else if ( settings::loot::armor && has ( "armor" ) ) {
                static thread_local char buf [64];
                snprintf ( buf, sizeof ( buf ), "Armor [%dm]", ( int ) item.distance );
                label = buf;
                col = IM_COL32 ( 60, 160, 255, 255 );
            }
            else if ( settings::loot::streaks && ( has ( "tablet" ) || has ( "streak" ) || has ( "airstrike" ) || has ( "precision" ) || has ( "decon" ) || has ( "bunker" ) || has ( "heartbeat" ) || has ( "offhand" ) || has ( "uav" ) || has ( "chopper" ) || has ( "sentry" ) || has ( "gunship" ) ) ) {
                std::string clean = sdk::engine::loot::clean_loot_name ( n );
                static thread_local char buf [128];
                snprintf ( buf, sizeof ( buf ), "%s [%dm]", clean.c_str ( ), ( int ) item.distance );
                label = buf;
                col = IM_COL32 ( 255, 60, 60, 255 );
            }
            else if ( settings::loot::stims && has ( "stim" ) ) {
                static thread_local char buf [64];
                snprintf ( buf, sizeof ( buf ), "Stim [%dm]", ( int ) item.distance );
                label = buf;
                col = IM_COL32 ( 80, 255, 120, 255 );
            }
            else if ( settings::loot::crates && ( has ( "crate" ) || has ( "bag" ) || has ( "backpack" ) || has ( "cache" ) ) ) {
                // parse rarity from raw name
                std::string low = n;
                for ( auto& c : low ) c = ( char ) tolower ( ( unsigned char ) c );

                const char* rarity_tag = "";
                if ( low.find ( "legendary" ) != std::string::npos )      { rarity_tag = "Legendary"; col = IM_COL32 ( 255, 165, 0, 255 ); }
                else if ( low.find ( "epic" ) != std::string::npos )      { rarity_tag = "Epic";      col = IM_COL32 ( 180, 70, 255, 255 ); }
                else if ( low.find ( "rare" ) != std::string::npos )      { rarity_tag = "Rare";      col = IM_COL32 ( 60, 140, 255, 255 ); }
                else if ( low.find ( "uncommon" ) != std::string::npos )  { rarity_tag = "Uncommon";  col = IM_COL32 ( 80, 220, 80, 255 ); }
                else                                                      { rarity_tag = "";          col = IM_COL32 ( 220, 180, 80, 255 ); }

                // clean crate type
                const char* crate_type = "Crate";
                if ( low.find ( "military" ) != std::string::npos )       crate_type = "Military Crate";
                else if ( low.find ( "ammo" ) != std::string::npos )      crate_type = "Ammo Crate";
                else if ( low.find ( "weapon" ) != std::string::npos )    crate_type = "Weapon Crate";
                else if ( low.find ( "medical" ) != std::string::npos || low.find ( "med" ) != std::string::npos ) crate_type = "Med Crate";
                else if ( low.find ( "supply" ) != std::string::npos )    crate_type = "Supply Crate";
                else if ( low.find ( "loot" ) != std::string::npos )      crate_type = "Loot Crate";
                else if ( low.find ( "cache" ) != std::string::npos )     crate_type = "Cache";
                else if ( low.find ( "bag" ) != std::string::npos )       crate_type = "Bag";
                else if ( low.find ( "backpack" ) != std::string::npos )  crate_type = "Backpack";

                static thread_local char buf [128];
                if ( rarity_tag [0] )
                    snprintf ( buf, sizeof ( buf ), "%s %s [%dm]", rarity_tag, crate_type, ( int ) item.distance );
                else
                    snprintf ( buf, sizeof ( buf ), "%s [%dm]", crate_type, ( int ) item.distance );
                label = buf;
            }
            else if ( settings::loot::money && has ( "money" ) ) {
                static thread_local char buf [64];
                snprintf ( buf, sizeof ( buf ), "Cash [%dm]", ( int ) item.distance );
                label = buf;
                col = IM_COL32 ( 50, 255, 50, 255 );
            }
            else if ( has ( "claymore" ) || has ( "mine" ) || has ( "c4" ) || has ( "razor" ) || has ( "trip" ) ) {
                static thread_local char buf [64];
                snprintf ( buf, sizeof ( buf ), "! Explosive [%dm]", ( int ) item.distance );
                label = buf;
                col = IM_COL32 ( 255, 50, 30, 255 );
            }
            else if ( has ( "trophy" ) || has ( "field_mic" ) || has ( "sensor" ) || has ( "jammer" ) ) {
                static thread_local char buf [64];
                snprintf ( buf, sizeof ( buf ), "Equipment [%dm]", ( int ) item.distance );
                label = buf;
                col = IM_COL32 ( 180, 130, 255, 255 );
            }
            else if ( has ( "Equipment" ) ) {
                static thread_local char buf [64];
                snprintf ( buf, sizeof ( buf ), "Placed Equipment [%dm]", ( int ) item.distance );
                label = buf;
                col = IM_COL32 ( 255, 120, 0, 255 );
            }

            float loot_scale = actor::get_text_scale ( item.distance );
            ImVec2 sz = actor::calc_esp_text_size ( label, loot_scale );
            float tx = screen.x - sz.x * 0.5f;
            float ty = screen.y;


            if ( label && settings::loot::show_icons ) {
                ID3D11ShaderResourceView* icon = nullptr;
                auto lhas = [&] ( const char* s ) { return n.find ( s ) != std::string::npos; };

                if ( settings::loot::stims && lhas ( "stim" ) )                                             icon = loot_stim;
                else if ( settings::loot::money && ( lhas ( "money" ) || lhas ( "cash" ) ) )                icon = loot_cash;
                else if ( settings::loot::crates && ( lhas ( "crate" ) || lhas ( "bag" ) || lhas ( "cache" ) ) ) {
                    std::string il = n; for ( auto& c : il ) c = ( char ) tolower ( ( unsigned char ) c );
                    if ( il.find ( "ammo" ) != std::string::npos )
                        icon = loot_crate;
                    else if ( il.find ( "legendary" ) != std::string::npos || il.find ( "epic" ) != std::string::npos )
                        icon = crate_epic;
                    else if ( il.find ( "rare" ) != std::string::npos )
                        icon = crate_rare;
                    else
                        icon = crate_common;
                }
                else if ( settings::loot::armor && lhas ( "armor" ) )                                       icon = loot_armor;
                else if ( lhas ( "heartbeat" ) )                                                            icon = loot_heartbeat;
                else if ( lhas ( "grappl" ) || lhas ( "hook" ) )                                            icon = loot_grapple;

                if ( icon ) {
                    float dist_norm = std::clamp ( ( item.distance - 5.0f ) / 80.0f, 0.0f, 1.0f );
                    float icon_sz = 22.0f - dist_norm * 12.0f;
                    float pad = 3.0f;
                    float ix = screen.x - icon_sz * 0.5f;
                    float iy = settings::loot::icons_only ? screen.y - icon_sz * 0.5f : ty - icon_sz - 4.0f;
                    dl->AddRectFilled ( ImVec2 ( ix - pad, iy - pad ), ImVec2 ( ix + icon_sz + pad, iy + icon_sz + pad ), IM_COL32 ( 0, 0, 0, 140 ), 3.0f );
                    dl->AddImage ( ( ImTextureID ) icon, ImVec2 ( ix, iy ), ImVec2 ( ix + icon_sz, iy + icon_sz ) );
                }

                if ( settings::loot::icons_only ) continue;
            }

            actor::draw_text_outline ( dl, ImVec2 ( tx, ty ), col, label, loot_scale );
        }
    }

    namespace radar_ns {

        inline ImVec2 radar_pos = ImVec2 ( 20.0f, 20.0f );
        inline bool   is_dragging = false;
        inline ImVec2 drag_offset = ImVec2 ( 0.0f, 0.0f );

        inline void draw ( ) {
            const auto& entities = cache::get_entities ( );
            const sdk::gInfo* game = cache::get_game_info ( );

            if ( !game || !game->inGame || !settings::visuals::radar::enabled )
                return;

            const RefDef_T& refdef = game->refdef;
            const float radar_size = settings::visuals::radar::size;
            const float radar_half = radar_size * 0.5f;
            const float max_dist  = settings::visuals::radar::max_distance;
            const float drag_bar_h = 18.0f;

            const Vector3& forward = refdef.axis [FORWARD_VEC];
            const float yaw = atan2f ( forward.y, forward.x );

            ImDrawList* dl = ImGui::GetBackgroundDrawList ( );
            ImVec2 center ( radar_pos.x + radar_half, radar_pos.y + radar_half );

            if ( menu::is_open ) {
                ImGui::SetNextWindowPos ( ImVec2 ( radar_pos.x, radar_pos.y - drag_bar_h ) );
                ImGui::SetNextWindowSize ( ImVec2 ( radar_size, radar_size + drag_bar_h ) );
                ImGui::PushStyleColor ( ImGuiCol_WindowBg, ImVec4 ( 0, 0, 0, 0 ) );
                ImGui::PushStyleVar ( ImGuiStyleVar_WindowBorderSize, 0.0f );
                ImGui::PushStyleVar ( ImGuiStyleVar_WindowPadding, ImVec2 ( 0, 0 ) );
                ImGui::Begin ( "##radar_drag", nullptr,
                    ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
                    ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoSavedSettings );
                ImGui::End ( );
                ImGui::PopStyleVar ( 2 );
                ImGui::PopStyleColor ( );

                ImGuiIO& io = ImGui::GetIO ( );
                ImVec2 mouse = io.MousePos;
                ImVec2 bar_min ( radar_pos.x, radar_pos.y - drag_bar_h );
                ImVec2 bar_max ( radar_pos.x + radar_size, radar_pos.y );

                bool hovering_bar = mouse.x >= bar_min.x && mouse.x <= bar_max.x &&
                                    mouse.y >= bar_min.y && mouse.y <= bar_max.y;

                if ( hovering_bar && io.MouseDown [0] && !is_dragging ) {
                    is_dragging = true;
                    drag_offset = ImVec2 ( radar_pos.x - mouse.x, radar_pos.y - mouse.y );
                }

                if ( is_dragging ) {
                    if ( io.MouseDown [0] ) {
                        float new_x = mouse.x + drag_offset.x;
                        float new_y = mouse.y + drag_offset.y;
                        new_x = std::clamp ( new_x, 0.0f, io.DisplaySize.x - radar_size );
                        new_y = std::clamp ( new_y, drag_bar_h, io.DisplaySize.y - radar_size );
                        radar_pos = ImVec2 ( new_x, new_y );
                        center = ImVec2 ( radar_pos.x + radar_half, radar_pos.y + radar_half );
                    }
                    else {
                        is_dragging = false;
                    }
                }

                float bar_rounding = 6.0f;
                dl->AddRectFilled (
                    ImVec2 ( radar_pos.x, radar_pos.y - drag_bar_h ),
                    ImVec2 ( radar_pos.x + radar_size, radar_pos.y ),
                    IM_COL32 ( 0, 0, 0, 210 ), bar_rounding, ImDrawFlags_RoundCornersTop );

                ImFont* label_font = font.spacegrotesk_medium [1];
                const char* label_text = "[drag radar]";
                ImVec2 label_sz = label_font->CalcTextSizeA ( label_font->FontSize, FLT_MAX, 0.0f, label_text );
                ImVec2 label_pos (
                    radar_pos.x + ( radar_size - label_sz.x ) * 0.5f,
                    radar_pos.y - drag_bar_h + ( drag_bar_h - label_sz.y ) * 0.5f );
                dl->AddText ( label_font, label_font->FontSize, label_pos, IM_COL32 ( 235, 235, 240, 255 ), label_text );

                dl->AddLine (ImVec2 ( radar_pos.x, radar_pos.y ),ImVec2 ( radar_pos.x + radar_size, radar_pos.y ),IM_COL32 ( 48, 48, 58, 200 ), 1.0f );
            }

            dl->AddCircleFilled ( center, radar_half, IM_COL32 ( 10, 10, 15, 200 ), 64 );
            dl->AddCircle ( center, radar_half, IM_COL32 ( 30, 35, 45, 180 ), 64, 1.5f );

            for ( const auto& ent : entities ) {
                if ( ent.isDead ) continue;
                if ( ent.team == game->localTeam ) continue;
                if ( ent.i == game->index ) continue;

                float dx = ent.position.x - game->localPos.x;
                float dy = ent.position.y - game->localPos.y;

                float sin_yaw = sinf ( -yaw );
                float cos_yaw = cosf ( -yaw );
                float rx = dx * cos_yaw - dy * sin_yaw;
                float ry = dx * sin_yaw + dy * cos_yaw;

                float raw_len = sqrtf ( rx * rx + ry * ry );
                if ( raw_len > 0.001f ) { rx /= raw_len; ry /= raw_len; }

                float mapped = ( ent.distance / max_dist ) * radar_half;
                float px = ry * mapped;
                float py = -rx * mapped;

                float margin = 5.0f;
                float clamp_r = radar_half - margin;
                float dist_on_radar = sqrtf ( px * px + py * py );
                if ( dist_on_radar > clamp_r ) {
                    float ratio = clamp_r / dist_on_radar;
                    px *= ratio;
                    py *= ratio;
                }

                float t = std::clamp ( ent.distance / max_dist, 0.0f, 1.0f );
                float dot_r = 4.0f - t * 2.0f;

                ImU32 dot_color = ent.isVisible
                    ? IM_COL32 ( 255, 255, 255, 230 )
                    : IM_COL32 ( 220, 60, 60, 220 );

                dl->AddCircleFilled ( ImVec2 ( center.x + px, center.y + py ), dot_r, dot_color, 12 );
            }

            dl->AddCircleFilled ( center, 4.0f, IM_COL32 ( 255, 255, 255, 255 ), 12 );
        }

    } // namespace radar_ns

}