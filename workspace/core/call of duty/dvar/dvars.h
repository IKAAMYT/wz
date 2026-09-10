#pragma once
#include "../../../impl/driverless/day1.h"
#include "../offsets/offsets.h"
#include <cstdint>
#include <string_view>



namespace sdk::dvar {

    inline constexpr std::uint64_t k_table_rva = 0x14E25190;
    inline constexpr std::uint64_t k_bucket_count = 0x4814;
    inline constexpr std::uint64_t off_hash = 0x00;
    inline constexpr std::uint64_t off_next = 0x08;
    inline constexpr std::uint64_t off_type = 0x15;
    inline constexpr std::uint64_t off_value = 0x28;

    enum class dvar_type : std::uint8_t {
        boolean = 0,
        floating = 1,
        int32 = 5,
        int64 = 7,
    };


    // FNV-1a (current — xor then multiply)
    constexpr std::uint64_t hash ( std::string_view name ) {
        std::uint64_t h = 0xCBF29CE484222325ULL;
        for ( char ch : name ) {
            std::uint8_t c = static_cast< std::uint8_t >( ch );
            if ( c >= 'A' && c <= 'Z' )
                c += 32;
            h = 0x100000001B3ULL * ( static_cast< std::uint64_t >( c ) ^ h );
        }
        return h;
    }

    // FNV-1 (multiply then xor)
    constexpr std::uint64_t hash_fnv1 ( std::string_view name ) {
        std::uint64_t h = 0xCBF29CE484222325ULL;
        for ( char ch : name ) {
            std::uint8_t c = static_cast< std::uint8_t >( ch );
            if ( c >= 'A' && c <= 'Z' )
                c += 32;
            h = ( h * 0x100000001B3ULL ) ^ static_cast< std::uint64_t >( c );
        }
        return h;
    }

    // FNV-1a without lowercase conversion
    constexpr std::uint64_t hash_raw ( std::string_view name ) {
        std::uint64_t h = 0xCBF29CE484222325ULL;
        for ( char ch : name ) {
            h = 0x100000001B3ULL * ( static_cast< std::uint64_t >( static_cast<uint8_t>(ch) ) ^ h );
        }
        return h;
    }

    inline std::uint64_t find ( std::uint64_t name_hash ) {
        const std::uint64_t table = g_vm->m_base_address + k_table_rva;
        std::uint64_t node = g_vm->read<std::uint64_t> ( table + ( name_hash % k_bucket_count ) * 8 );

        for ( int guard = 0; node && guard < 256; ++guard ) {
            if ( g_vm->read<std::uint64_t> ( node + off_hash ) == name_hash )
                return node;
            node = g_vm->read<std::uint64_t> ( node + off_next );
        }
        return 0;
    }

    inline std::uint64_t find ( std::string_view name ) {
        return find ( hash ( name ) );
    }


    inline bool get_bool ( std::uint64_t node, bool fallback = false ) {
        if ( !node ) return fallback;
        return g_vm->read<std::uint8_t> ( node + off_value ) != 0;
    }
    inline float get_float ( std::uint64_t node, float fallback = 0.f ) {
        if ( !node ) return fallback;
        return g_vm->read<float> ( node + off_value );
    }
    inline std::int32_t get_int ( std::uint64_t node, std::int32_t fallback = 0 ) {
        if ( !node ) return fallback;
        return g_vm->read<std::int32_t> ( node + off_value );
    }

    inline bool set_bool ( std::uint64_t node, bool v ) {
        if ( !node ) return false;
        return g_vm->write<std::uint8_t> ( node + off_value, v ? 1 : 0 );
    }
    inline bool set_float ( std::uint64_t node, float v ) {
        if ( !node ) return false;
        return g_vm->write<float> ( node + off_value, v );
    }
    inline bool set_int ( std::uint64_t node, std::int32_t v ) {
        if ( !node ) return false;
        return g_vm->write<std::int32_t> ( node + off_value, v );
    }

    inline bool set_bool ( std::string_view name, bool v )  { return set_bool ( find ( name ), v ); }
    inline bool set_float ( std::string_view name, float v ) { return set_float ( find ( name ), v ); }
    inline bool set_int ( std::string_view name, std::int32_t v ) { return set_int ( find ( name ), v ); }


    struct forced_bool {
        std::uint64_t node = 0;
        std::uint64_t name_hash = 0;
        bool value = false;

        void init ( std::string_view name, bool v ) { name_hash = hash ( name ); value = v; node = 0; }
        void tick ( ) {
            if ( !node ) node = find ( name_hash );
            if ( node ) set_bool ( node, value );
        }
    };
}
