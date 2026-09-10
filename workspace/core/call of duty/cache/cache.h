#pragma once
#include <Windows.h>
#include <thread>
#include <vector>
#include <iostream>
#include <cstdio>
#include <future>
#include <mutex>
#include <chrono>
#include <array>
#include <unordered_map>
#include <string>
#include <cstring>
#include "../../../../impl/driverless/day1.h"
#include "../offsets/offsets.h"
#include "../sdk/sdk.h"
#include "../settings/settings.h"
#include "../dvar/dvars.h"

namespace cache {

    namespace listener {
        inline void update ( ) {
            while ( true ) {
                if ( settings::aimbot::controller_support ) {
                    CustomWidgets::UpdateControllerKeybindState ( &settings::aimbot::aimbot_key );
                }
                else {
                    CustomWidgets::UpdateKeybindState ( &settings::aimbot::aimbot_key );
                }
                std::this_thread::sleep_for ( std::chrono::milliseconds ( 1 ) );
            }
        }
    }

    struct game_info_buffer {
        sdk::gInfo data {};
        bool valid { false };
    };

    struct entity_buffer {
        std::vector<sdk::entInfo> data;
        entity_buffer ( ) {
            data.reserve ( 128 );
        }
    };

    struct LootInfo {
        uintptr_t address = 0;
        Vector3 position = {};
        std::string name = "Unknown";
        bool valid = false;
        float distance = 0.0f;
    };

    struct loot_buffer {
        std::vector<LootInfo> data;
        loot_buffer ( ) {
            data.reserve ( 256 );
        }
    };

    struct utilstr_64 {
        char str [64];
    };

    static game_info_buffer game_buffers [2] {};
    static entity_buffer    entity_buffers [2] {};
    static loot_buffer      loot_buffers [2] {};

    static std::atomic<int> game_write_idx { 0 };
    static std::atomic<int> game_read_idx { 1 };

    static std::atomic<int> entity_write_idx { 0 };
    static std::atomic<int> entity_read_idx { 1 };

    static std::atomic<int> loot_write_idx { 0 };
    static std::atomic<int> loot_read_idx { 1 };

    static Vector3 cached_camera_pos {};
    static std::chrono::steady_clock::time_point last_camera_update;

    static std::unordered_map<uint32_t, std::string> weapon_name_cache;
    static std::mutex weapon_cache_mutex;

    struct BoneValidation {
        bool isValid = false;
        std::chrono::steady_clock::time_point lastValidTime;
    };
    static BoneValidation boneValidation;
    static constexpr std::array<int, 14> BONE_INDICES = { 7, 2, 5, 6, 10, 11, 12, 14, 15, 16, 18, 19, 22, 23 };

    static std::unordered_map<int, bool> isPlayerDead;
    static std::unordered_map<int, std::vector<Vector3>> lastBonePositions;

    inline const sdk::gInfo* get_game_info ( ) {
        int idx = game_read_idx.load ( std::memory_order_acquire );
        return game_buffers [idx].valid ? &game_buffers [idx].data : nullptr;
    }

    inline const std::vector<sdk::entInfo>& get_entities ( ) {
        int idx = entity_read_idx.load ( std::memory_order_acquire );
        return entity_buffers [idx].data;
    }

    inline const std::vector<LootInfo>& get_loot ( ) {
        int idx = loot_read_idx.load ( std::memory_order_acquire );
        return loot_buffers [idx].data;
    }

    inline Vector3 get_camera_position ( ) {
        return cached_camera_pos;
    }



    void replace_string_in_place ( std::string& subject, const  std::string& search, const  std::string& replace )
    {
        size_t pos = 0;
        while ( ( pos = subject.find ( search, pos ) ) != std::string::npos )
        {
            subject.replace ( pos, search.length ( ), replace );
            pos += replace.length ( );
        }
    }

    bool replace_weapon_name ( std::string& weapon_name )
    {
        if ( weapon_name.find ( ( "WEAPON/" ) ) != std::string::npos )
        {
            replace_string_in_place ( weapon_name, ( "WEAPON/" ), "" );

            if ( weapon_name.find ( ( "JUP_JUP_" ) ) != std::string::npos )
            {
                replace_string_in_place ( weapon_name, ( "JUP_JUP_" ), "" );

                return true;
            }
        }
        return false;
    }



    inline std::string get_weapon_name ( uint32_t weaponIndex, uint16_t player ) {
        if ( weaponIndex == 0 ) {
            return "";
        }

        uint32_t maskedIndex = ( weaponIndex >> 10 ) & 0x7FF;

        std::lock_guard<std::mutex> lock ( weapon_cache_mutex );

        auto it = weapon_name_cache.find ( maskedIndex );
        if ( it != weapon_name_cache.end ( ) ) {
            return it->second;
        }

        auto base_addr = g_vm->m_base_address + offsets::weapon_definitions;
        auto wep_base = g_vm->read<uintptr_t> ( base_addr + ( maskedIndex * 0x8 ) );

        if ( !wep_base ) {
            weapon_name_cache [maskedIndex] = "";
            return "";
        }

        auto name_ptr = g_vm->read<uintptr_t> ( wep_base + 0x78 );
        if ( !name_ptr ) {
            weapon_name_cache [maskedIndex] = "";
            return "";
        }

        char name_buf [64] {};
        g_vm->read_memory ( name_ptr, name_buf, sizeof ( name_buf ) - 1 );
        std::string internalName ( name_buf );

        if ( internalName.empty ( ) || internalName == "none" ) {
            weapon_name_cache [maskedIndex] = "Unknown";
            return "Unknown";
        }

        std::string raw = internalName;
        replace_weapon_name ( raw );

        std::string lower = raw;
        for ( auto& c : lower ) c = ( char ) std::tolower ( ( unsigned char ) c );

        auto has = [&] ( const char* s ) { return lower.find ( s ) != std::string::npos; };

        std::string clean;

        if ( has("_fists_") || has("fist") || has("ohand") ) clean = "Fists";

        else if ( has("xm4") )        clean = "XM4";
        else if ( has("ak74") )       clean = "AK-74";
        else if ( has("ames85") )     clean = "Ames 85";
        else if ( has("gpr91") )      clean = "GPR 91";
        else if ( has("goblin") )     clean = "Goblin Mk2";
        else if ( has("as_val") || has("asval") ) clean = "AS VAL";
        else if ( has("galil") )      clean = "Galil";
        else if ( has("model_l") )    clean = "Model L";
        else if ( has("tanto") )      clean = "Tanto .22";

        else if ( has("c9") )         clean = "C9";
        else if ( has("pp919") )      clean = "PP-919";
        else if ( has("jackal") )     clean = "Jackal PDW";
        else if ( has("kompakt") )    clean = "Kompakt 92";
        else if ( has("saug") )       clean = "Saug";
        else if ( has("ksx") )        clean = "KSX";

        else if ( has("asb") || has("asb300") ) clean = "ASG-89";
        else if ( has("lr762") || has("lr7") )  clean = "LR 7.62";
        else if ( has("svd") )        clean = "SVD";
        else if ( has("lw3") )        clean = "LW3 Tundra";

        else if ( has("xmg") )        clean = "XMG";
        else if ( has("pul") )        clean = "PU-21";

        else if ( has("9mm") || has("gs45") )  clean = "GS45";
        else if ( has("stryder") )    clean = "Stryder .22";
        else if ( has("grekhova") )   clean = "Grekhova";

        else if ( has("marine") )     clean = "Marine SP";
        else if ( has("maelstrom") )  clean = "Maelstrom";
        else if ( has("asz") )        clean = "ASZ-94";

        else if ( has("tsec") || has("mr_") || has("_dm_") || has("dmr") ) clean = "Marksman";
        else if ( has("sirin") )      clean = "Sirin 9mm";

        else if ( has("knife") || has("bowie") || has("stiletto") ) clean = "Knife";
        else if ( has("sword") || has("katana") || has("machete") ) clean = "Melee";
        else if ( has("bat") || has("sledge") ) clean = "Melee";

        else if ( has("cigma") || has("_la_") ) clean = "Launcher";

        else if ( has("frag") || has("semtex") || has("molotov") || has("thermite") ) clean = "Lethal";
        else if ( has("flash") || has("stun") || has("smoke") || has("decoy") ) clean = "Tactical";
        else if ( has("trophy") || has("field_upgrade") ) clean = "Field Upgrade";
        else if ( has("grenade") || has("_eq_") ) clean = "Equipment";

        else if ( has("streak") || has("killstreak") || has("scorestreak") ) clean = "Streak";
        else if ( has("minigun") || has("turret") || has("sentry") ) clean = "Streak";
        else if ( has("chopper") || has("gunship") ) clean = "Streak";

        else if ( has("_ar_") )  clean = "Assault Rifle";
        else if ( has("_sh_") || has("_sm_") || has("smg") ) clean = "SMG";
        else if ( has("_sn_") || has("sniper") ) clean = "Sniper";
        else if ( has("_lm_") || has("lmg") )   clean = "LMG";
        else if ( has("_pi_") || has("pistol") ) clean = "Pistol";
        else if ( has("_sg_") || has("shotgun") ) clean = "Shotgun";
        else if ( has("_me_") || has("melee") )  clean = "Melee";
        else clean = raw;

        weapon_name_cache [maskedIndex] = clean;
        return clean;
    }


    bool is_player_dead ( int playerIndex ) {
        auto it = isPlayerDead.find ( playerIndex );
        if ( it != isPlayerDead.end ( ) ) {
            return it->second;
        }
        return false;
    }

    bool validate_bones ( const std::vector<Vector3>& bones ) {
        if ( bones.empty ( ) ) return false;

        int validCount = 0;
        for ( const auto& bone : bones ) {
            if ( bone.x != 0.0f || bone.y != 0.0f || bone.z != 0.0f ) {
                validCount++;
            }
        }
        return validCount >= bones.size ( ) * 0.7f;
    }


    uint32_t inverse_xor_for_viewangle ( uint32_t xorkey, uint64_t pointer ) {
        uint32_t xorvalue1 = xorkey;
        uint32_t xorvalue2 = pointer;

        uint32_t value = ( ( ( xorvalue1 ^ xorvalue2 ) + 2 ) * ( xorvalue1 ^ xorvalue2 ) );

        return value;
    }

    void no_recoil ( uintptr_t client_info ) {
        if ( !client_info || !g_vm->is_valid ( client_info + offsets::player::recoil ) ) return;

        static uint8_t recoil_data [0x10] {};
        if ( !g_vm->read_memory ( client_info + offsets::player::recoil, recoil_data, 0x10 ) ) return;

        uint32_t xor_key = *reinterpret_cast<uint32_t*> ( recoil_data + 0xC );
        if ( xor_key == 0 ) return;

        uint64_t addr_y = client_info + offsets::player::recoil;
        uint64_t addr_x = client_info + offsets::player::recoil + 0x4;

        uint32_t dec_y = inverse_xor_for_viewangle ( xor_key, addr_y ) ^ *reinterpret_cast<uint32_t*> ( recoil_data );
        uint32_t dec_x = inverse_xor_for_viewangle ( xor_key, addr_x ) ^ *reinterpret_cast<uint32_t*> ( recoil_data + 4 );

        float pitch = *reinterpret_cast<float*> ( &dec_y );
        float yaw = *reinterpret_cast<float*> ( &dec_x );

        if ( std::isnan(pitch) || std::isnan(yaw) ) return;
        if ( fabsf(pitch) < 0.01f && fabsf(yaw) < 0.01f ) return;

        float zero = 0.0f;
        uint32_t enc_y = inverse_xor_for_viewangle ( xor_key, addr_y ) ^ *reinterpret_cast<uint32_t*> ( &zero );
        uint32_t enc_x = inverse_xor_for_viewangle ( xor_key, addr_x ) ^ *reinterpret_cast<uint32_t*> ( &zero );

        g_vm->write<uint32_t> ( addr_y, enc_y );
        g_vm->write<uint32_t> ( addr_x, enc_x );
    }


    inline uintptr_t cached_client_active = 0;

    uintptr_t get_client_active ( ) {
        cached_client_active = decryptions::decrypt_client_active ( );
        return cached_client_active;
    }

    Vector3 get_recoil_angles ( uintptr_t client_info ) {
        if ( !client_info ) return {};
        uintptr_t base = client_info + offsets::player::recoil;
        if ( !g_vm->is_valid ( base ) ) return {};

        uint8_t block [0x10] {};
        if ( !g_vm->read_memory ( base, block, 0x10 ) ) return {};

        uint32_t xor_key = *reinterpret_cast<uint32_t*> ( block + 0xC );
        if ( xor_key == 0 ) return {};

        uint32_t raw_y = *reinterpret_cast<uint32_t*> ( block );
        uint32_t raw_x = *reinterpret_cast<uint32_t*> ( block + 4 );

        uint32_t dec_y = inverse_xor_for_viewangle ( xor_key, base ) ^ raw_y;
        uint32_t dec_x = inverse_xor_for_viewangle ( xor_key, base + 0x4 ) ^ raw_x;

        float pitch = *reinterpret_cast<float*> ( &dec_y );
        float yaw   = *reinterpret_cast<float*> ( &dec_x );

        if ( std::isnan(pitch) || std::isnan(yaw) ) return {};
        return { yaw, pitch, 0 };
    }

    void set_recoil_angles ( uintptr_t client_info, float pitch, float yaw ) {
        if ( !client_info ) return;
        uintptr_t base = client_info + offsets::player::recoil;
        if ( !g_vm->is_valid ( base ) ) return;

        uint32_t xor_key = g_vm->read<uint32_t> ( base + 0xC );
        if ( xor_key == 0 ) return;

        uint32_t enc_y = inverse_xor_for_viewangle ( xor_key, base ) ^ *reinterpret_cast<uint32_t*> ( &yaw );
        uint32_t enc_x = inverse_xor_for_viewangle ( xor_key, base + 0x4 ) ^ *reinterpret_cast<uint32_t*> ( &pitch );

        g_vm->write<uint32_t> ( base, enc_y );
        g_vm->write<uint32_t> ( base + 0x4, enc_x );
    }

    Vector3 calc_angle ( const Vector3& src, const Vector3& dst, const RefDef_T& refdef ) {
        Vector3 dir = dst - src;
        float len = dir.length ( );
        if ( len < 0.001f ) return {};
        dir = dir / len;
        float y = asinf ( dir.Dot ( refdef.axis [RIGHT_VEC] ) ) * ( 180.0f / 3.14159265f );
        float p = -asinf ( dir.Dot ( refdef.axis [UP_VEC] ) ) * ( 180.0f / 3.14159265f );
        return { y, p, 0 };
    }

    float norm_angle ( float a ) {
        return std::isfinite ( a ) ? std::remainderf ( a, 360.0f ) : 0.0f;
    }



    void thread_cache_game ( ) {
        static int game_frame = 0;
        while ( true ) {
            game_frame++;
            const int w = game_write_idx.load ( std::memory_order_relaxed );
            auto& buf = game_buffers [w];

            buf.valid = false;


            int32_t game_mode_val = g_vm->read<int32_t> ( g_vm->m_base_address + offsets::game_mode );
            if ( game_mode_val > 1 ) {
                sdk::gInfo info {};
                info.inGame = true;
                info.playerCount = game_mode_val;

                if ( info.playerCount > 0 ) {
                    info.clientInfo = decryptions::decrypt_client_info ( );
                    info.clientBase = decryptions::decrypt_client_base ( info.clientInfo );
                    info.index = sdk::engine::player::local_player_index ( info.clientBase, info.clientInfo );
                    info.localPlayer = info.clientBase + ( info.index * offsets::player::size );
                    info.boneBase = decryptions::decrypt_bone_base ( );
                    Vector3 bone_pos = sdk::engine::bone::retrieve_bone_position_vec ( info.clientInfo );
                    auto bone_index = decryptions::get_bone_index ( info.index );
                    auto bone_ptr = sdk::engine::bone::bone_pointer ( info.boneBase, bone_index );


                    auto now = std::chrono::steady_clock::now ( );
                    auto elapsed = std::chrono::duration_cast< std::chrono::seconds >( now - boneValidation.lastValidTime ).count ( );

                    if ( bone_ptr != 0 && bone_ptr != -1 ) {
                        boneValidation.isValid = true;
                        boneValidation.lastValidTime = now;
                    }
                    else if ( !boneValidation.isValid && elapsed < 2 ) {
                        boneValidation.isValid = true;
                    }
                    else if ( !boneValidation.isValid || elapsed >= 2 ) {
                        boneValidation.isValid = false;
                    }

                    uintptr_t refdef_ptr = DecryptRefDef->GetRefDef ( );
                    if ( refdef_ptr != 0 ) {
                        info.refdef = g_vm->read<RefDef_T> ( refdef_ptr );
                    }

                    info.bone_pos = bone_pos;
                    info.bone_ptr = bone_ptr;
                    info.localPos = sdk::engine::player::get_position ( info.localPlayer );

                    sdk::engine::prediction::update_velocity ( info.index, info.localPos );

                    info.localTeam = sdk::engine::player::team_id_check ( info.localPlayer );

                    {
                        uint32_t lw = g_vm->read<uint32_t> ( info.localPlayer + offsets::player::weapon_index );
                        std::string wn = get_weapon_name ( lw, 0 );
                        sdk::engine::prediction::update_bullet_params ( wn );
                    }

                    buf.data = info;
                    buf.valid = true;
                }
            }

  
            cached_camera_pos = sdk::engine::client::retrieve_camera_posistion ( );

            game_read_idx.store ( w, std::memory_order_release );
            game_write_idx.store ( w ^ 1, std::memory_order_relaxed );

            std::this_thread::sleep_for ( std::chrono::milliseconds ( 1 ) );
        }
    }

    void thread_cache_entities ( ) {
        static int entity_frame = 0;
        while ( true ) {
            entity_frame++;
            const int w = entity_write_idx.load ( std::memory_order_relaxed );
            auto& buf = entity_buffers [w];
            buf.data.clear ( );

            const sdk::gInfo* game = get_game_info ( );
            if ( game && game->inGame && game->playerCount > 0 ) {
                buf.data.reserve ( game->playerCount );

                const bool bonesValid = boneValidation.isValid;
                const uint64_t name_list_base = sdk::engine::player::get_name_list ( );
                const auto vis_bits = g_vm->read<sdk::client_bits_t> ( game->clientInfo + offsets::visible_bits );
                const Vector3 bone_pos = sdk::engine::bone::retrieve_bone_position_vec ( game->clientInfo );
                static const std::vector<int> bone_idx_vec ( BONE_INDICES.begin ( ), BONE_INDICES.end ( ) );

                for ( int i = 0; i < game->playerCount; ++i ) {
                    uintptr_t player = game->clientBase + ( i * offsets::player::size );

                    struct {
                        bool valid;
                        uint8_t pad0 [7];
                        uint8_t team;
                        uint8_t pad1 [3];
                        uint32_t weaponIdx;
                        uint8_t pad2 [4];
                        uintptr_t posPtr;
                    } pre {};

                    batch_read_entry pre_entries [] = {
                        { player + offsets::player::valid,        sizeof ( bool ),      0 },
                        { player + offsets::player::team,         sizeof ( uint8_t ),   8 },
                        { player + offsets::player::weapon_index, sizeof ( uint32_t ), 12 },
                        { player + offsets::player::pos,          sizeof ( uintptr_t ), 24 },
                    };
                    g_vm->read_batch ( pre_entries, 4, &pre, sizeof ( pre ) );

                    if ( !pre.valid )
                        continue;
                    if ( pre.team == game->localTeam )
                        continue;
                    if ( player == game->localPlayer )
                        continue;

                    Vector3 pos = {};
                    if ( pre.posPtr )
                        pos = g_vm->read<Vector3> ( pre.posPtr + 0x80 );
                    if ( pos.is_zero ( ) )
                        continue;

                    auto bone_index = decryptions::get_bone_index ( i );
                    auto bone_ptr = sdk::engine::bone::bone_pointer ( game->boneBase, bone_index );

                    bool hasValidBones = false;
                    std::vector<Vector3> tempBones;

                    if ( bonesValid && bone_ptr != 0 && bone_ptr != ( uint64_t ) -1 ) {
                        tempBones = sdk::engine::bone::get_multiple_bones ( bone_ptr, bone_pos, bone_idx_vec );
                        hasValidBones = validate_bones ( tempBones );
                    }

                    auto deadIt = isPlayerDead.find ( i );
                    bool isDead = ( deadIt != isPlayerDead.end ( ) ) ? deadIt->second : false;

                    static std::unordered_map<int, std::chrono::steady_clock::time_point> boneInvalidStartTime;
                    auto now = std::chrono::steady_clock::now ( );

                    if ( !hasValidBones ) {
                        auto it = boneInvalidStartTime.find ( i );
                        if ( it == boneInvalidStartTime.end ( ) ) {
                            boneInvalidStartTime [i] = now;
                        }
                        else {
                            auto elapsed = std::chrono::duration_cast< std::chrono::seconds >( now - it->second ).count ( );
                            if ( elapsed >= 2 && !isDead ) {
                                isDead = true;
                                isPlayerDead [i] = true;
                            }
                        }
                    }
                    else {
                        if ( isDead ) {
                            isDead = false;
                            isPlayerDead [i] = false;
                            lastBonePositions [i] = tempBones;
                        }
                        boneInvalidStartTime.erase ( i );
                    }

                    uintptr_t sb_entry = game->clientInfo + offsets::scoreboard + ( i * offsets::scoreboard_size );

                    struct {
                        Vector3 boundsPos;
                        float boundsRadius;
                        uintptr_t transform;
                        uint16_t kills;
                        uint16_t deaths;
                        uint16_t assists;
                        uint16_t pad;
                        uint32_t score;
                    } batch_out {};

                    batch_read_entry batch_entries [] = {
                        { sb_entry + offsets::sb::kills,                     sizeof ( uint16_t ),  24 },
                        { sb_entry + offsets::sb::deaths,                    sizeof ( uint16_t ),  26 },
                        { sb_entry + offsets::sb::assists,                   sizeof ( uint16_t ),  28 },
                        { sb_entry + offsets::sb::score,                     sizeof ( uint32_t ),  32 },
                    };
                    g_vm->read_batch ( batch_entries, 7, &batch_out, sizeof ( batch_out ) );



                    sdk::entInfo ent {};
                    ent.player = player;
                    ent.localPlayer = game->localPlayer;
                    ent.position = pos;
                    ent.i = i;
                    ent.isDead = isDead;
                    ent.kills = batch_out.kills;
                    ent.deaths = batch_out.deaths;
                    ent.assists = batch_out.assists;
                    ent.score = batch_out.score;

                    ent.weaponName = get_weapon_name ( pre.weaponIdx, player );
                    ent.distance = sdk::engine::client::units_to_m ( game->localPos.distance_to ( pos ) );
                    ent.team = pre.team;

                    ent.nameEntry = g_vm->read<sdk::name_entry> ( name_list_base + ( i * offsets::name_array_size ) );
                    ent.name = std::string ( ent.nameEntry.name );

                    if ( !isDead ) {
                        auto bitmask = 0x80000000 >> ( i & 0x1F );
                        ent.isVisible = ( vis_bits.array [i >> 5] & bitmask ) != 0;
                        ent.health = std::clamp ( ( int ) ent.nameEntry.health, 0, 100 );

                        sdk::engine::prediction::update_velocity ( i, pos );

                        if ( hasValidBones ) {
                            ent.bonePositions = tempBones;
                            lastBonePositions [i] = tempBones;
                        }
                        else {
                            auto it = lastBonePositions.find ( i );
                            if ( it != lastBonePositions.end ( ) )
                                ent.bonePositions = it->second;
                        }
                    }
                    else {
                        ent.isVisible = false;
                        ent.health = 0;
                        auto it = lastBonePositions.find ( i );
                        if ( it != lastBonePositions.end ( ) )
                            ent.bonePositions = it->second;
                    }

                    buf.data.emplace_back ( ent );
                }
            }

            if ( game && game->inGame && settings::exploits::no_recoil ) {
                bool aim_held = settings::aimbot::aimbot_key.is_down.load ( std::memory_order_relaxed );

                if ( aim_held ) {
                    uint32_t local_wep = g_vm->read<uint32_t> ( game->localPlayer + offsets::player::weapon_index );
                    std::string wep_name = get_weapon_name ( local_wep, 0 );
                    bool is_fists = wep_name.empty ( ) || wep_name == "Unknown" || wep_name.find ( "fist" ) != std::string::npos || wep_name.find ( "Fist" ) != std::string::npos;

                    if ( !is_fists )
                        no_recoil ( game->clientInfo );
                }
            }

            entity_read_idx.store ( w, std::memory_order_release );
            entity_write_idx.store ( w ^ 1, std::memory_order_relaxed );

            std::this_thread::sleep_for ( std::chrono::milliseconds ( 1 ) );
        }
    }


    void thread_cache_loot ( ) {
        while ( true ) {
            const int w = loot_write_idx.load ( std::memory_order_relaxed );
            auto& buf = loot_buffers [w];
            buf.data.clear ( );

            const sdk::gInfo* game = get_game_info ( );
            if ( game && game->inGame && ( settings::loot::draw_loot || settings::loot::draw_equipment ) ) {

                if ( settings::loot::draw_loot ) {
                    uintptr_t loot_array = g_vm->m_base_address + offsets::loot_ptr;

                    for ( int c = 0; c < 2; ++c ) {
                        uintptr_t chunk = g_vm->read<uintptr_t> ( loot_array + c * 8 );
                        if ( !chunk || !g_vm->is_valid ( chunk ) ) continue;

                        for ( int i = 0; i < 512; ++i ) {
                            uintptr_t entry = chunk + ( uint64_t ) i * offsets::loot_size;

                            uint8_t valid = g_vm->read<uint8_t> ( entry + offsets::loot_valid1 );
                            if ( valid != 1 ) continue;

                            Vector3 pos = g_vm->read<Vector3> ( entry + offsets::loot_pos );
                            if ( pos.is_zero ( ) ) continue;

                            float dist = sdk::engine::client::units_to_m ( game->localPos.distance_to ( pos ) );
                            if ( dist > ( float ) settings::loot::max_distance ) continue;

                            sdk::item_info_data info = sdk::read_loot_item ( entry );

                            LootInfo li {};
                            li.address = entry;
                            li.position = pos;
                            li.distance = dist;
                            li.valid = true;
                            li.name = info.name.empty ( ) ? "Unknown" : info.name;

                            buf.data.emplace_back ( li );
                        }
                    }
                }

                if ( settings::loot::draw_equipment ) {
                    uintptr_t equip_base = game->clientInfo + 0x439D4;
                    uint16_t ent_nums [20] {};
                    g_vm->read_memory ( equip_base - 60, ent_nums, sizeof ( ent_nums ) );

                    for ( int e = 0; e < 20; ++e ) {
                        if ( ent_nums [e] == 0 || ent_nums [e] == 2047 ) continue;

                        uintptr_t ent_addr = game->clientBase + ( ( uint64_t ) ent_nums [e] * offsets::player::size );
                        if ( !g_vm->is_valid ( ent_addr ) ) continue;

                        uintptr_t pos_ptr = g_vm->read<uintptr_t> ( ent_addr + offsets::player::pos );
                        if ( !pos_ptr ) continue;

                        Vector3 pos = g_vm->read<Vector3> ( pos_ptr + 0x80 );
                        if ( pos.is_zero ( ) ) continue;

                        float dist = sdk::engine::client::units_to_m ( game->localPos.distance_to ( pos ) );
                        if ( dist > ( float ) settings::loot::max_distance ) continue;

                        LootInfo li {};
                        li.address = ent_addr;
                        li.position = pos;
                        li.distance = dist;
                        li.valid = true;
                        li.name = "Equipment";

                        buf.data.emplace_back ( li );
                    }
                }
            }

            loot_read_idx.store ( w, std::memory_order_release );
            loot_write_idx.store ( w ^ 1, std::memory_order_relaxed );

            std::this_thread::sleep_for ( std::chrono::milliseconds ( 100 ) );
        }
    }

    inline void clear_all_caches ( ) {
        for ( int i = 0; i < 2; ++i ) {
            game_buffers [i].valid = false;
            entity_buffers [i].data.clear ( );
            loot_buffers [i].data.clear ( );
        }
        isPlayerDead.clear ( );
        lastBonePositions.clear ( );
        weapon_name_cache.clear ( );
        boneValidation.isValid = false;
    }

}