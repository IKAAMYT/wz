#pragma once

#include <imgui_settings.h>

inline ID3D11ShaderResourceView* soldier;


namespace settings {

    namespace aimbot {

        inline bool enabled = false;
        inline float smoothness = 5.0f;
        inline float fov = 50.0f;
        inline bool show_fov = false;
        inline int target_bone = 0;
        inline int hitbox [9] = { 1, 2, 2, 3, 3, 0, 0, 0, 0 }; // head,neck,chest,stomach,pelvis,larm,rarm,lleg,rleg
        inline bool visible_check = false;
        inline bool prediction = false;
        inline float prediction_strength = 1.0f;
        inline bool controller_support = false;
        inline CustomWidgets::Keybind aimbot_key;
        inline float controller_scale = 1500.0f;
        inline bool sticky_aim = false;
        inline float aim_distance = 150.0f;
        inline bool bezier_aim = true;
        inline float curve_strength = 0.15f;
        inline float deadzone = 2.0f;
    }

    namespace exploits {
        inline bool no_recoil = false;
        inline bool full_auto = false;
        inline bool rapid_fire = false;
    }

    namespace overlay {
        inline bool streamproof = false;
    }

	namespace visuals {

		inline bool skeleton = false;
        inline bool skeleton_outline = false;
		inline bool box = false;
        inline int box_type = 0;
        inline bool box_outline = false;
        inline bool box_fill = true;
        inline bool chams = true;
        inline bool china_hat = false;
		inline bool username = false;
		inline bool health = false;
		inline bool distance = false;
        inline bool level = false;
        inline bool weapon = false;
        inline int  username_pos = 0;   // 0=top 1=bottom 2=left 3=right
        inline int  level_pos = 0;
        inline int  weapon_pos = 1;
        inline int  health_pos = 2;
        inline int   outline_type = 1;     // 0 = none, 1 = normal outline, 2 = drop shadow
        inline float text_size = 12.0f;    // base font pixel size
        inline bool  text_scaling = true;  // scale text with distance
        inline int   font_index = 0;
        inline int   skeleton_type = 0;    // 0 = straight, 1 = bezier
        inline ImColor gradient_top = ImColor ( 180, 180, 255, 255 );
        inline ImColor gradient_bottom = ImColor ( 60, 40, 140, 255 );
        inline ImColor gradient_top_hidden = ImColor ( 255, 100, 100, 255 );
        inline ImColor gradient_bottom_hidden = ImColor ( 140, 20, 20, 255 );
        inline float esp_distance = 150.0f;
        inline bool  offscreen_arrows = false;
        inline bool  show_prestige = false;
        inline bool  prestige_bg = true;
        inline int   prestige_style = 0;       // 0=BO7 1=BO6 2=BO3 3=CoD4
        inline bool  show_kills = false;
        inline bool  show_platform = false;
        inline int   kills_pos = 1;      // 0=top 1=bottom 2=left 3=right
        inline int   platform_pos = 3;   // default right side
        inline bool  snaplines = false;
        inline int   snapline_type = 0;   // 0=bottom, 1=crosshair
        inline bool  aim_direction = false;

        namespace radar {
            inline bool  enabled = false;
            inline float size = 220.0f;
            inline float max_distance = 200.0f;
        }
	}

    namespace loot {
        inline bool draw_loot = false;
        inline bool draw_equipment = false;
        inline bool show_icons = true;
        inline bool icons_only = false;
        inline bool weapons = true;
        inline bool ammo = false;
        inline bool armor = false;
        inline bool streaks = false;
        inline bool stims = false;
        inline bool crates = false;
        inline bool money = false;
        inline int  max_distance = 50;
    }
}