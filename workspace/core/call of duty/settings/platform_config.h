#pragma once

#include <fstream>
#include <filesystem>
#include "../offsets/offsets.h"

namespace platform_config {

    inline const char* file ( ) { return "C:\\desire\\bo7\\platform.ini"; }

    inline bool platform_chosen = false;

    inline const char* name ( Platform p ) {
        switch ( p ) {
        case Platform::Xbox:      return "Xbox";
        case Platform::BattleNet: return "Battle.net";
        default:                  return "Steam";
        }
    }

    inline bool load ( Platform& out ) {
        std::ifstream in ( file ( ) );
        if ( !in ) return false;
        int v = -1;
        in >> v;
        if ( v < 0 || v > 2 ) return false;
        out = static_cast<Platform> ( v );
        return true;
    }

    inline void save ( Platform p ) {
        std::error_code ec;
        std::filesystem::create_directories ( "C:\\desire\\bo7", ec );
        std::ofstream out ( file ( ), std::ios::trunc );
        if ( out ) out << static_cast<int> ( p );
    }

    inline void apply ( Platform p ) {
        offsets::init ( p );
        save ( p );
        platform_chosen = true;
    }

    inline void try_load_saved ( ) {
        Platform saved;
        if ( load ( saved ) ) {
            offsets::init ( saved );
            platform_chosen = true;
        }
    }
}
