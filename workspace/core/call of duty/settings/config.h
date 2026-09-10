#pragma once

#include <Windows.h>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <array>
#include <unordered_map>
#include "settings.h"

// Encrypted key=value persistence for all settings:: fields.
// File: C:\desire\bo7\config.ini (whole buffer encrypted).
// Note: aimbot::aimbot_key (Keybind) is intentionally not serialized.
namespace config {

    inline const char* dir_path  ( ) { return "C:\\desire\\bo7"; }
    inline std::string file_path ( ) { return std::string ( dir_path ( ) ) + "\\config.ini"; }

    // Machine-locked seed: FNV-1a over the computer name + system-drive volume
    // serial. Deterministic on a given PC, different across PCs, no hardcoded key.
    inline uint64_t machine_seed ( ) {
        std::string id;

        char name [ MAX_COMPUTERNAME_LENGTH + 1 ] {};
        DWORD nlen = sizeof ( name );
        if ( GetComputerNameA ( name, &nlen ) )
            id.append ( name, nlen );

        DWORD volser = 0;
        if ( GetVolumeInformationA ( "C:\\", nullptr, 0, &volser, nullptr, nullptr, nullptr, 0 ) ) {
            char vbuf [ 16 ] {};
            id.append ( vbuf, static_cast<size_t>( snprintf ( vbuf, sizeof ( vbuf ), "%08lX", volser ) ) );
        }

        uint64_t h = 0xCBF29CE484222325ull;          // FNV-1a 64 offset basis
        for ( unsigned char c : id ) {
            h ^= c;
            h *= 0x100000001B3ull;                    // FNV prime
        }
        return h ? h : 0x9E3779B97F4A7C15ull;         // guard against all-zero
    }

    // Symmetric keystream cipher (xorshift64). Same call encrypts and decrypts.
    inline void crypt ( std::string& data ) {
        uint64_t state = machine_seed ( );
        for ( size_t i = 0; i < data.size ( ); ++i ) {
            state ^= state << 13;
            state ^= state >> 7;
            state ^= state << 17;
            data [ i ] = static_cast<char>( static_cast<uint8_t>( data [ i ] ) ^ static_cast<uint8_t>( state & 0xFF ) );
        }
    }

    // Builds the plaintext key=value blob (same content save() persists, but
    // returned in memory and un-encrypted). Shared by save() and to_blob().
    inline std::string build_blob ( ) {
        std::ostringstream f;

        auto wb = [ & ] ( const char* k, bool v )  { f << k << '=' << ( v ? 1 : 0 ) << '\n'; };
        auto wi = [ & ] ( const char* k, int v )   { f << k << '=' << v << '\n'; };
        auto wf = [ & ] ( const char* k, float v ) { f << k << '=' << v << '\n'; };
        auto wc = [ & ] ( const char* k, const ImColor& c ) {
            f << k << '=' << c.Value.x << ' ' << c.Value.y << ' ' << c.Value.z << ' ' << c.Value.w << '\n';
        };

        wb ( "aim.enabled",            settings::aimbot::enabled );
        wf ( "aim.smoothness",         settings::aimbot::smoothness );
        wf ( "aim.fov",                settings::aimbot::fov );
        wb ( "aim.show_fov",           settings::aimbot::show_fov );
        wi ( "aim.target_bone",        settings::aimbot::target_bone );
        for ( int i = 0; i < 9; ++i ) { std::string k = "aim.hb" + std::to_string(i); wi ( k.c_str(), settings::aimbot::hitbox[i] ); }
        wb ( "aim.visible_check",      settings::aimbot::visible_check );
        wb ( "aim.prediction",         settings::aimbot::prediction );
        wf ( "aim.pred_strength",      settings::aimbot::prediction_strength );
        wb ( "aim.controller_support", settings::aimbot::controller_support );
        wf ( "aim.controller_scale",   settings::aimbot::controller_scale );
        wi ( "aim.key",                settings::aimbot::aimbot_key.key );
        wb ( "aim.sticky_aim",         settings::aimbot::sticky_aim );
        wf ( "aim.aim_distance",       settings::aimbot::aim_distance );
        wb ( "aim.bezier_aim",         settings::aimbot::bezier_aim );
        wf ( "aim.curve_strength",     settings::aimbot::curve_strength );
        wf ( "aim.deadzone",           settings::aimbot::deadzone );

        wb ( "exp.no_recoil",          settings::exploits::no_recoil );
        wb ( "exp.full_auto",          settings::exploits::full_auto );
        wb ( "exp.rapid_fire",         settings::exploits::rapid_fire );

        wb ( "vis.skeleton",           settings::visuals::skeleton );
        wb ( "vis.skeleton_outline",   settings::visuals::skeleton_outline );
        wb ( "vis.box",                settings::visuals::box );
        wi ( "vis.box_type",           settings::visuals::box_type );
        wb ( "vis.box_outline",        settings::visuals::box_outline );
        wb ( "vis.box_fill",           settings::visuals::box_fill );
        wb ( "vis.chams",              settings::visuals::chams );
        wb ( "vis.china_hat",          settings::visuals::china_hat );
        wb ( "vis.username",           settings::visuals::username );
        wb ( "vis.health",             settings::visuals::health );
        wb ( "vis.distance",           settings::visuals::distance );
        wb ( "vis.level",              settings::visuals::level );
        wb ( "vis.weapon",             settings::visuals::weapon );
        wi ( "vis.username_pos",       settings::visuals::username_pos );
        wi ( "vis.level_pos",          settings::visuals::level_pos );
        wi ( "vis.weapon_pos",         settings::visuals::weapon_pos );
        wi ( "vis.health_pos",         settings::visuals::health_pos );
        wi ( "vis.outline_type",       settings::visuals::outline_type );
        wf ( "vis.text_size",          settings::visuals::text_size );
        wb ( "vis.text_scaling",       settings::visuals::text_scaling );
        wi ( "vis.font_index",         settings::visuals::font_index );
        wi ( "vis.skeleton_type",      settings::visuals::skeleton_type );
        wc ( "vis.gradient_top",       settings::visuals::gradient_top );
        wc ( "vis.gradient_bottom",    settings::visuals::gradient_bottom );
        wc ( "vis.gradient_top_hid",   settings::visuals::gradient_top_hidden );
        wc ( "vis.gradient_bot_hid",   settings::visuals::gradient_bottom_hidden );
        wf ( "vis.esp_distance",       settings::visuals::esp_distance );
        wb ( "vis.offscreen_arrows",   settings::visuals::offscreen_arrows );
        wb ( "vis.show_prestige",      settings::visuals::show_prestige );
        wb ( "vis.prestige_bg",        settings::visuals::prestige_bg );
        wi ( "vis.prestige_style",     settings::visuals::prestige_style );
        wb ( "vis.show_kills",         settings::visuals::show_kills );
        wb ( "vis.show_platform",      settings::visuals::show_platform );
        wi ( "vis.kills_pos",          settings::visuals::kills_pos );
        wi ( "vis.platform_pos",       settings::visuals::platform_pos );
        wb ( "vis.snaplines",          settings::visuals::snaplines );
        wi ( "vis.snapline_type",      settings::visuals::snapline_type );
        wb ( "vis.aim_direction",      settings::visuals::aim_direction );
        wb ( "vis.radar_enabled",      settings::visuals::radar::enabled );
        wf ( "vis.radar_size",         settings::visuals::radar::size );
        wf ( "vis.radar_max_dist",     settings::visuals::radar::max_distance );

        wb ( "loot.draw_loot",         settings::loot::draw_loot );
        wb ( "loot.draw_equipment",    settings::loot::draw_equipment );
        wb ( "loot.show_icons",        settings::loot::show_icons );
        wb ( "loot.icons_only",        settings::loot::icons_only );
        wb ( "loot.weapons",           settings::loot::weapons );
        wb ( "loot.ammo",              settings::loot::ammo );
        wb ( "loot.armor",             settings::loot::armor );
        wb ( "loot.streaks",           settings::loot::streaks );
        wb ( "loot.stims",             settings::loot::stims );
        wb ( "loot.crates",            settings::loot::crates );
        wb ( "loot.money",             settings::loot::money );
        wi ( "loot.max_distance",      settings::loot::max_distance );

        return f.str ( );
    }

    inline void save ( ) {
        std::error_code ec;
        std::filesystem::create_directories ( dir_path ( ), ec );

        std::string blob = build_blob ( );
        crypt ( blob );

        std::ofstream out ( file_path ( ), std::ios::binary | std::ios::trunc );
        if ( out ) out.write ( blob.data ( ), static_cast<std::streamsize>( blob.size ( ) ) );
    }

    // Applies a plaintext key=value blob (as produced by build_blob()) onto the
    // settings:: fields. Shared by load() and from_blob().
    inline void apply_blob ( const std::string& blob ) {
        std::unordered_map<std::string, std::string> kv;
        std::istringstream f ( blob );
        std::string line;
        while ( std::getline ( f, line ) ) {
            const auto eq = line.find ( '=' );
            if ( eq == std::string::npos ) continue;
            kv [ line.substr ( 0, eq ) ] = line.substr ( eq + 1 );
        }

        auto gb = [ & ] ( const char* k, bool& v )  { auto it = kv.find ( k ); if ( it != kv.end ( ) ) v = ( it->second != "0" ); };
        auto gi = [ & ] ( const char* k, int& v )   { auto it = kv.find ( k ); if ( it != kv.end ( ) ) { try { v = std::stoi ( it->second ); } catch ( ... ) {} } };
        auto gf = [ & ] ( const char* k, float& v ) { auto it = kv.find ( k ); if ( it != kv.end ( ) ) { try { v = std::stof ( it->second ); } catch ( ... ) {} } };
        auto gc = [ & ] ( const char* k, ImColor& c ) {
            auto it = kv.find ( k );
            if ( it == kv.end ( ) ) return;
            std::istringstream ss ( it->second );
            float r, g, b, a;
            if ( ss >> r >> g >> b >> a ) c.Value = ImVec4 ( r, g, b, a );
        };

        gb ( "aim.enabled",            settings::aimbot::enabled );
        gf ( "aim.smoothness",         settings::aimbot::smoothness );
        gf ( "aim.fov",                settings::aimbot::fov );
        gb ( "aim.show_fov",           settings::aimbot::show_fov );
        gi ( "aim.target_bone",        settings::aimbot::target_bone );
        for ( int i = 0; i < 9; ++i ) { std::string k = "aim.hb" + std::to_string(i); gi ( k.c_str(), settings::aimbot::hitbox[i] ); settings::aimbot::hitbox[i] = std::clamp(settings::aimbot::hitbox[i], 0, 4); }
        gb ( "aim.visible_check",      settings::aimbot::visible_check );
        gb ( "aim.prediction",         settings::aimbot::prediction );
        gf ( "aim.pred_strength",      settings::aimbot::prediction_strength );
        settings::aimbot::prediction_strength = std::clamp ( settings::aimbot::prediction_strength, 0.1f, 2.0f );
        gb ( "aim.controller_support", settings::aimbot::controller_support );
        gf ( "aim.controller_scale",   settings::aimbot::controller_scale );
        gi ( "aim.key",                settings::aimbot::aimbot_key.key );
        gb ( "aim.sticky_aim",         settings::aimbot::sticky_aim );
        gf ( "aim.aim_distance",       settings::aimbot::aim_distance );
        settings::aimbot::aim_distance = std::clamp ( settings::aimbot::aim_distance, 10.0f, 500.0f );
        gb ( "aim.bezier_aim",         settings::aimbot::bezier_aim );
        gf ( "aim.curve_strength",     settings::aimbot::curve_strength );
        settings::aimbot::curve_strength = std::clamp ( settings::aimbot::curve_strength, 0.0f, 0.4f );
        gf ( "aim.deadzone",           settings::aimbot::deadzone );
        settings::aimbot::deadzone = std::clamp ( settings::aimbot::deadzone, 0.0f, 20.0f );

        gb ( "exp.no_recoil",          settings::exploits::no_recoil );
        gb ( "exp.full_auto",          settings::exploits::full_auto );
        gb ( "exp.rapid_fire",         settings::exploits::rapid_fire );

        gb ( "vis.skeleton",           settings::visuals::skeleton );
        gb ( "vis.skeleton_outline",   settings::visuals::skeleton_outline );
        gb ( "vis.box",                settings::visuals::box );
        gi ( "vis.box_type",           settings::visuals::box_type );
        gb ( "vis.box_outline",        settings::visuals::box_outline );
        gb ( "vis.box_fill",           settings::visuals::box_fill );
        gb ( "vis.chams",              settings::visuals::chams );
        gb ( "vis.china_hat",          settings::visuals::china_hat );
        gb ( "vis.username",           settings::visuals::username );
        gb ( "vis.health",             settings::visuals::health );
        gb ( "vis.distance",           settings::visuals::distance );
        gb ( "vis.level",              settings::visuals::level );
        gb ( "vis.weapon",             settings::visuals::weapon );
        gi ( "vis.username_pos",       settings::visuals::username_pos );
        settings::visuals::username_pos = std::clamp ( settings::visuals::username_pos, 0, 3 );
        gi ( "vis.level_pos",          settings::visuals::level_pos );
        settings::visuals::level_pos = std::clamp ( settings::visuals::level_pos, 0, 3 );
        gi ( "vis.weapon_pos",         settings::visuals::weapon_pos );
        settings::visuals::weapon_pos = std::clamp ( settings::visuals::weapon_pos, 0, 3 );
        gi ( "vis.health_pos",         settings::visuals::health_pos );
        settings::visuals::health_pos = std::clamp ( settings::visuals::health_pos, 0, 3 );
        gi ( "vis.outline_type",       settings::visuals::outline_type );
        gf ( "vis.text_size",          settings::visuals::text_size );
        settings::visuals::text_size = std::clamp ( settings::visuals::text_size, 8.0f, 20.0f );
        gb ( "vis.text_scaling",       settings::visuals::text_scaling );
        gi ( "vis.font_index",         settings::visuals::font_index );
        settings::visuals::font_index = std::clamp ( settings::visuals::font_index, 0, 1 );
        gi ( "vis.skeleton_type",      settings::visuals::skeleton_type );
        settings::visuals::skeleton_type = std::clamp ( settings::visuals::skeleton_type, 0, 1 );
        gc ( "vis.gradient_top",       settings::visuals::gradient_top );
        gc ( "vis.gradient_bottom",    settings::visuals::gradient_bottom );
        gc ( "vis.gradient_top_hid",   settings::visuals::gradient_top_hidden );
        gc ( "vis.gradient_bot_hid",   settings::visuals::gradient_bottom_hidden );
        gf ( "vis.esp_distance",       settings::visuals::esp_distance );
        settings::visuals::esp_distance = std::clamp ( settings::visuals::esp_distance, 10.0f, 500.0f );
        gb ( "vis.offscreen_arrows",   settings::visuals::offscreen_arrows );
        gb ( "vis.show_prestige",      settings::visuals::show_prestige );
        gb ( "vis.prestige_bg",        settings::visuals::prestige_bg );
        gi ( "vis.prestige_style",     settings::visuals::prestige_style );
        settings::visuals::prestige_style = std::clamp ( settings::visuals::prestige_style, 0, 3 );
        gb ( "vis.show_kills",         settings::visuals::show_kills );
        gb ( "vis.show_platform",      settings::visuals::show_platform );
        gi ( "vis.kills_pos",          settings::visuals::kills_pos );
        gi ( "vis.platform_pos",       settings::visuals::platform_pos );
        gb ( "vis.snaplines",          settings::visuals::snaplines );
        gi ( "vis.snapline_type",      settings::visuals::snapline_type );
        gb ( "vis.aim_direction",      settings::visuals::aim_direction );
        gb ( "vis.radar_enabled",      settings::visuals::radar::enabled );
        gf ( "vis.radar_size",         settings::visuals::radar::size );
        settings::visuals::radar::size = std::clamp ( settings::visuals::radar::size, 100.0f, 400.0f );
        gf ( "vis.radar_max_dist",     settings::visuals::radar::max_distance );
        settings::visuals::radar::max_distance = std::clamp ( settings::visuals::radar::max_distance, 50.0f, 500.0f );

        gb ( "loot.draw_loot",         settings::loot::draw_loot );
        gb ( "loot.draw_equipment",    settings::loot::draw_equipment );
        gb ( "loot.show_icons",        settings::loot::show_icons );
        gb ( "loot.icons_only",        settings::loot::icons_only );
        gb ( "loot.weapons",           settings::loot::weapons );
        gb ( "loot.ammo",              settings::loot::ammo );
        gb ( "loot.armor",             settings::loot::armor );
        gb ( "loot.streaks",           settings::loot::streaks );
        gb ( "loot.stims",             settings::loot::stims );
        gb ( "loot.crates",            settings::loot::crates );
        gb ( "loot.money",             settings::loot::money );
        gi ( "loot.max_distance",      settings::loot::max_distance );
    }

    inline void load ( ) {
        std::ifstream in ( file_path ( ), std::ios::binary );
        if ( !in ) return;

        std::string blob ( ( std::istreambuf_iterator<char> ( in ) ), std::istreambuf_iterator<char> ( ) );
        crypt ( blob );

        apply_blob ( blob );
    }

    // In-memory, un-encrypted plaintext of the current settings (for cloud sync).
    inline std::string to_blob ( ) { return build_blob ( ); }

    // Parse a plaintext blob (from cloud) into the live settings.
    inline void from_blob ( const std::string& blob ) { apply_blob ( blob ); }

    namespace profiles {

        inline constexpr int k_max_profiles = 5;

        struct Profile {
            std::string name;
            bool exists = false;
        };

        inline std::array<Profile, k_max_profiles> slots;
        inline int active_profile = -1;

        inline std::string profile_path ( int index ) {
            return std::string ( dir_path ( ) ) + "\\profile_" + std::to_string ( index ) + ".ini";
        }
        inline std::string names_path ( ) {
            return std::string ( dir_path ( ) ) + "\\profiles.txt";
        }

        inline void save_names ( ) {
            std::error_code ec;
            std::filesystem::create_directories ( dir_path ( ), ec );
            std::ofstream out ( names_path ( ) );
            for ( int i = 0; i < k_max_profiles; ++i )
                out << ( slots [i].exists ? slots [i].name : "" ) << '\n';
        }

        inline void load_names ( ) {
            std::ifstream in ( names_path ( ) );
            if ( !in ) return;
            for ( int i = 0; i < k_max_profiles; ++i ) {
                std::string name;
                if ( std::getline ( in, name ) && !name.empty ( ) ) {
                    slots [i].name = name;
                    slots [i].exists = std::filesystem::exists ( profile_path ( i ) );
                } else {
                    slots [i].name.clear ( );
                    slots [i].exists = false;
                }
            }
        }

        inline void save_to ( int index, const std::string& name ) {
            std::error_code ec;
            std::filesystem::create_directories ( dir_path ( ), ec );
            std::string blob = build_blob ( );
            crypt ( blob );
            std::ofstream out ( profile_path ( index ), std::ios::binary | std::ios::trunc );
            if ( out ) out.write ( blob.data ( ), static_cast< std::streamsize >( blob.size ( ) ) );
            slots [index].name = name;
            slots [index].exists = true;
            active_profile = index;
            save_names ( );
        }

        inline bool load_from ( int index ) {
            if ( !slots [index].exists ) return false;
            std::ifstream in ( profile_path ( index ), std::ios::binary );
            if ( !in ) return false;
            std::string blob ( ( std::istreambuf_iterator<char> ( in ) ), std::istreambuf_iterator<char> ( ) );
            crypt ( blob );
            apply_blob ( blob );
            active_profile = index;
            return true;
        }

        inline void remove ( int index ) {
            std::error_code ec;
            std::filesystem::remove ( profile_path ( index ), ec );
            slots [index].name.clear ( );
            slots [index].exists = false;
            if ( active_profile == index ) active_profile = -1;
            save_names ( );
        }
    }
}
