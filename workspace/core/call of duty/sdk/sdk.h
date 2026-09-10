#pragma once

#include <windows.h>
#include "vectors/vectors.h"
#include "../../../../impl/driverless/day1.h"
#include "../offsets/offsets.h"
#include "lelecore/lelecore.h"
#include <map>

namespace sdk {

	struct name_entry
	{
		char     pad_0000 [12];
		int32_t  index;
		char     name [36];
		char     nameWithHash [64];
		int32_t  rank_mp;
		int32_t  prestige_mp;
		int32_t  rank_alien;
		char     clanAbbrev [9];
		uint8_t  clanTagType;
		int32_t  location;
		int32_t  health;
		bool     isMLGSpectator;
		bool     isMLGFollower;
		int32_t  indexMLGFollower;
		char     bountyCount;
		uint32_t perkIconName;
		int32_t  squadIndex;
	};

	enum ClientPlatform : __int8
	{
		steam = 0x1,
		bnet = 0x2,
		xb1 = 0x3,
		xbsx = 0x4,
		ps4 = 0x5,
		ps5 = 0x6,
		we = 0x7,
		ios = 0x8,
		wingdk = 0xC,
		ubi = 0xD
	};

	enum HardwarePlatform : __int8
	{
		hw_ps3 = 0,
		hw_xbox360 = 1,
		hw_psvita = 2,
		hw_lrb = 3,
		hw_wiiu = 4,
		hw_ps4 = 5,
		hw_xboxone = 6,
		hw_nx = 7,
		hw_gdk = 8,
		hw_ps5 = 9,
	};

	struct item_info_data
	{
		bool item_valid;
		Vector3 pos;
		std::string name;
	};


	inline item_info_data read_loot_item ( uintptr_t entry ) {
		item_info_data buf {};
		if ( !entry ) return buf;

		uint8_t v1 = g_vm->read<uint8_t> ( entry + offsets::loot_valid1 );
		buf.item_valid = ( v1 == 1 );
		buf.pos = g_vm->read<Vector3> ( entry + offsets::loot_pos );

		static const int ptr_offsets [] = { 0x128, 0x130, 0x8, 0x0 };
		static const int name_offsets [] = { 0x0, 0x8, 0x10, 0x78 };

		for ( int po : ptr_offsets ) {
			uintptr_t def = g_vm->read<uintptr_t> ( entry + po );
			if ( !def || !g_vm->is_valid ( def ) ) continue;

			for ( int no : name_offsets ) {
				uintptr_t np = g_vm->read<uintptr_t> ( def + no );
				if ( !np || !g_vm->is_valid ( np ) ) continue;

				char nb [128] {};
				g_vm->read_memory ( np, nb, 64 );
				if ( nb [0] >= 0x20 && nb [0] < 0x7F && nb [1] != '\0' ) {
					buf.name = std::string ( nb );
					return buf;
				}
			}
		}

		return buf;
	}

	class entInfo
	{
	public:
		uintptr_t player;
		uintptr_t localPlayer;
		Vector3 position;
		Vector3 boundsPosition;     // NEW: Position from bounds data (player + 0x2F0)
		float boundsRadius;         // NEW: Radius from bounds data (player + 0x2FC)
		float boundsHeight;         // NEW: Height from transform (tf + 0x98, ~160 standing)
		bool isVisible;
		int i;
		float distance;
		std::string name;
		std::string weaponName;
		int health;
		name_entry nameEntry;
		std::vector<Vector3> bonePositions;
		std::vector<Vector2> boneScreenPositions;
		Vector2 screenHead;
		Vector2 screenFeet;
		Vector4 bbox;
		int team;
		bool isDead;
		uint16_t kills;
		uint16_t deaths;
		uint16_t assists;
		uint32_t score;
		uint8_t  platform;   // ClientPlatform enum
	};

	class gInfo
	{
	public:
		bool inGame;
		int playerCount;
		uintptr_t clientInfo;
		uintptr_t clientBase;
		uintptr_t boneBase;
		Vector3 bone_pos;
		uint64_t bone_ptr;
		int index;
		uintptr_t localPlayer;
		uint8_t localTeam;
		Vector3 localPos;
		RefDef_T refdef;
	};

	struct client_bits_t
	{
		int array [7];
	};


	namespace engine
	{


		namespace prediction {

			struct velocity_info {
				Vector3 velocity;       // smoothed velocity in units/sec
				Vector3 last_pos;
				LARGE_INTEGER last_time;
				bool initialized = false;
			};

			static std::map<DWORD, velocity_info> velocity_map;

			inline Vector3 get_speed ( int32_t i ) {
				auto it = velocity_map.find ( i );
				if ( it != velocity_map.end ( ) )
					return it->second.velocity;
				return Vector3 { 0, 0, 0 };
			}

			inline void update_velocity ( int32_t index, const Vector3& current_pos ) {
				auto& vel = velocity_map [index];

				LARGE_INTEGER now, freq;
				QueryPerformanceCounter ( &now );
				QueryPerformanceFrequency ( &freq );

				if ( vel.initialized ) {
					float dt = ( float ) ( now.QuadPart - vel.last_time.QuadPart ) / ( float ) freq.QuadPart;
					if ( dt > 0.001f && dt < 1.0f ) {
						Vector3 raw = {
							( current_pos.x - vel.last_pos.x ) / dt,
							( current_pos.y - vel.last_pos.y ) / dt,
							( current_pos.z - vel.last_pos.z ) / dt
						};

						// EMA smoothing — responsive but kills jitter
						const float a = 0.35f;
						vel.velocity.x += a * ( raw.x - vel.velocity.x );
						vel.velocity.y += a * ( raw.y - vel.velocity.y );
						vel.velocity.z += a * ( raw.z - vel.velocity.z );
					}
				}

				vel.last_pos  = current_pos;
				vel.last_time = now;
				vel.initialized = true;
			}

			inline float cached_bullet_speed = 30000.f;
			inline float cached_bullet_gravity = 3.f;

			struct weapon_ballistics { float speed; float gravity; };

			inline weapon_ballistics get_ballistics ( const std::string& weapon_name ) {
				std::string lower = weapon_name;
				for ( auto& c : lower ) c = ( char ) tolower ( ( unsigned char ) c );
				auto has = [&] ( const char* s ) { return lower.find ( s ) != std::string::npos; };

				if ( has ( "sniper" ) || has ( "svd" ) || has ( "lw3" ) || has ( "lr 7" ) )
					return { 40000.f, 2.f };
				if ( has ( "marksman" ) || has ( "sirin" ) )
					return { 35000.f, 2.f };
				if ( has ( "lmg" ) || has ( "xmg" ) || has ( "pu-21" ) )
					return { 30000.f, 1.5f };
				if ( has ( "smg" ) || has ( "c9" ) || has ( "pp-919" ) || has ( "jackal" ) || has ( "kompakt" ) || has ( "saug" ) || has ( "ksx" ) )
					return { 25000.f, 1.5f };
				if ( has ( "pistol" ) || has ( "gs45" ) || has ( "stryder" ) || has ( "grekhova" ) )
					return { 22000.f, 1.5f };
				if ( has ( "shotgun" ) || has ( "marine" ) || has ( "maelstrom" ) || has ( "asz" ) )
					return { 18000.f, 1.f };
				if ( has ( "launcher" ) )
					return { 8000.f, 6.f };
				return { 30000.f, 1.5f };
			}

			inline void update_bullet_params ( const std::string& weapon_name ) {
				auto b = get_ballistics ( weapon_name );
				cached_bullet_speed = b.speed;
				cached_bullet_gravity = b.gravity;
			}

			inline Vector3 get_prediction_players ( int32_t index, Vector3 destination, const Vector3& local_pos, const Vector3& local_vel )
			{
				Vector3 target_vel = get_speed ( index );
				float bullet_speed  = cached_bullet_speed;
				float bullet_gravity = cached_bullet_gravity;

				float strength = settings::aimbot::prediction_strength;

				Vector3 predicted = destination;
				for ( int i = 0; i < 3; ++i ) {
					float dist  = local_pos.distance_to ( predicted );
					float tof   = dist / bullet_speed;

					predicted.x = destination.x + ( target_vel.x - local_vel.x ) * tof;
					predicted.y = destination.y + ( target_vel.y - local_vel.y ) * tof;
					predicted.z = destination.z + ( target_vel.z - local_vel.z ) * tof
					            + 0.5f * bullet_gravity * tof * tof;
				}

				predicted.x = destination.x + ( predicted.x - destination.x ) * strength;
				predicted.y = destination.y + ( predicted.y - destination.y ) * strength;
				predicted.z = destination.z + ( predicted.z - destination.z ) * strength;

				return predicted;
			}
		}


		namespace loot {


			void replace_string_in_place ( std::string& subject, const std::string& search, const std::string& replace )
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

			bool replace_weapon_loot ( std::string& weapon_name )
			{
				if ( weapon_name.find ( "wpn_t10_wm_" ) != std::string::npos ) {
					replace_string_in_place ( weapon_name, "wpn_t10_wm_", "" );
					return true;
				}
				return false;
			}

			inline std::string clean_loot_name ( const std::string& raw ) {
				std::string n = raw;

				const char* strip [] = {
					"wpn_sat_vm_", "wpn_sat_wm_", "wpn_t10_vm_", "wpn_t10_wm_",
					"eqp_sat_vm_", "eqp_sat_wm_", "eqp_t10_vm_", "eqp_t10_wm_",
					"wpn_", "eqp_", "sat_", "t10_", "vm_", "wm_",
				};
				for ( auto prefix : strip ) {
					size_t pos = n.find ( prefix );
					if ( pos != std::string::npos )
						n.erase ( pos, strlen ( prefix ) );
				}

				const char* suffixes [] = { "_root", "_loot", "_pickup", "_drop" };
				for ( auto suf : suffixes ) {
					size_t pos = n.rfind ( suf );
					if ( pos != std::string::npos && pos + strlen ( suf ) == n.size ( ) )
						n.erase ( pos );
				}

				std::string lower = n;
				for ( auto& c : lower ) c = ( char ) tolower ( ( unsigned char ) c );

				std::string raw_lower = raw;
				for ( auto& c : raw_lower ) c = ( char ) tolower ( ( unsigned char ) c );

				auto has = [&] ( const char* s ) { return lower.find ( s ) != std::string::npos || raw_lower.find ( s ) != std::string::npos; };

				std::string clean;

				if ( has ( "_fists_" ) || has ( "fist" ) || has ( "ohand" ) ) clean = "Fists";

				else if ( has ( "xm4" ) )        clean = "XM4";
				else if ( has ( "ak74" ) )       clean = "AK-74";
				else if ( has ( "ames85" ) )     clean = "Ames 85";
				else if ( has ( "gpr91" ) )      clean = "GPR 91";
				else if ( has ( "goblin" ) )     clean = "Goblin Mk2";
				else if ( has ( "as_val" ) || has ( "asval" ) ) clean = "AS VAL";
				else if ( has ( "galil" ) )      clean = "Galil";
				else if ( has ( "model_l" ) )    clean = "Model L";
				else if ( has ( "tanto" ) )      clean = "Tanto .22";

				else if ( has ( "c9" ) )         clean = "C9";
				else if ( has ( "pp919" ) )      clean = "PP-919";
				else if ( has ( "jackal" ) )     clean = "Jackal PDW";
				else if ( has ( "kompakt" ) )    clean = "Kompakt 92";
				else if ( has ( "saug" ) )       clean = "Saug";
				else if ( has ( "ksx" ) )        clean = "KSX";

				else if ( has ( "asb" ) || has ( "asb300" ) ) clean = "ASG-89";
				else if ( has ( "lr762" ) || has ( "lr7" ) )  clean = "LR 7.62";
				else if ( has ( "svd" ) )        clean = "SVD";
				else if ( has ( "lw3" ) )        clean = "LW3 Tundra";

				else if ( has ( "xmg" ) )        clean = "XMG";
				else if ( has ( "pul" ) )        clean = "PU-21";

				else if ( has ( "9mm" ) || has ( "gs45" ) )  clean = "GS45";
				else if ( has ( "stryder" ) )    clean = "Stryder .22";
				else if ( has ( "grekhova" ) )   clean = "Grekhova";

				else if ( has ( "marine" ) )     clean = "Marine SP";
				else if ( has ( "maelstrom" ) )  clean = "Maelstrom";
				else if ( has ( "asz" ) )        clean = "ASZ-94";

				else if ( has ( "tsec" ) || has ( "mr_" ) || has ( "_dm_" ) || has ( "dmr" ) ) clean = "Marksman";
				else if ( has ( "sirin" ) )      clean = "Sirin 9mm";

				else if ( has ( "knife" ) || has ( "bowie" ) || has ( "stiletto" ) ) clean = "Knife";
				else if ( has ( "sword" ) || has ( "katana" ) || has ( "machete" ) ) clean = "Melee";
				else if ( has ( "bat" ) || has ( "sledge" ) ) clean = "Melee";

				else if ( has ( "cigma" ) || has ( "_la_" ) ) clean = "Launcher";

				else if ( has ( "frag" ) || has ( "semtex" ) || has ( "molotov" ) || has ( "thermite" ) ) clean = "Lethal";
				else if ( has ( "flash" ) || has ( "stun" ) || has ( "smoke" ) || has ( "decoy" ) ) clean = "Tactical";
				else if ( has ( "trophy" ) || has ( "field_upgrade" ) ) clean = "Field Upgrade";
				else if ( has ( "grenade" ) || has ( "_eq_" ) ) clean = "Equipment";
				else if ( has ( "stim" ) )       clean = "Stim";

				else if ( has ( "claymore" ) )   clean = "Claymore";
				else if ( has ( "mine" ) )       clean = "Mine";
				else if ( has ( "c4" ) )         clean = "C4";
				else if ( has ( "razor_wire" ) || has ( "razor" ) ) clean = "Razor Wire";
				else if ( has ( "trophy" ) )     clean = "Trophy System";
				else if ( has ( "field_mic" ) )  clean = "Field Mic";
				else if ( has ( "sensor" ) )     clean = "Sensor";
				else if ( has ( "jammer" ) )     clean = "Jammer";
				else if ( has ( "barricade" ) )  clean = "Barricade";
				else if ( has ( "deployable" ) ) clean = "Deployable";

				else if ( has ( "airstrike" ) || has ( "precision" ) ) clean = "Airstrike";
				else if ( has ( "decon" ) )       clean = "Decon Station";
				else if ( has ( "bunker" ) )      clean = "Bunker Buster";
				else if ( has ( "heartbeat" ) )   clean = "Heartbeat";
				else if ( has ( "offhand" ) && has ( "tablet" ) ) clean = "Tablet";
				else if ( has ( "tablet" ) )      clean = "Tablet";
				else if ( has ( "uav" ) )         clean = "UAV";
				else if ( has ( "chopper" ) || has ( "gunship" ) ) clean = "Chopper";
				else if ( has ( "minigun" ) || has ( "turret" ) || has ( "sentry" ) ) clean = "Sentry";
				else if ( has ( "streak" ) || has ( "killstreak" ) || has ( "scorestreak" ) ) clean = "Streak";

				else if ( has ( "_ar_" ) )  clean = "Assault Rifle";
				else if ( has ( "_sh_" ) || has ( "_sm_" ) || has ( "smg" ) ) clean = "SMG";
				else if ( has ( "_sn_" ) || has ( "sniper" ) ) clean = "Sniper";
				else if ( has ( "_lm_" ) || has ( "lmg" ) )   clean = "LMG";
				else if ( has ( "_pi_" ) || has ( "pistol" ) ) clean = "Pistol";
				else if ( has ( "_sg_" ) || has ( "shotgun" ) ) clean = "Shotgun";
				else if ( has ( "_me_" ) || has ( "melee" ) )  clean = "Melee";
				else {
					for ( auto& c : n ) if ( c == '_' ) c = ' ';
					if ( !n.empty ( ) ) n [0] = ( char ) toupper ( ( unsigned char ) n [0] );
					for ( size_t i = 1; i < n.size ( ); ++i )
						if ( n [i - 1] == ' ' ) n [i] = ( char ) toupper ( ( unsigned char ) n [i] );
					clean = n;
				}

				return clean;
			}


		}

		namespace client
		{
			inline bool is_user_in_game ( )
			{
				return g_vm->read<int32_t> ( g_vm->m_base_address + offsets::game_mode ) > 1;
			}

			inline int32_t player_count ( )
			{
				const int32_t count = g_vm->read<int32_t> ( g_vm->m_base_address + offsets::game_mode );

				return count;
			}

			Vector3 retrieve_camera_posistion ( )
			{
				const uintptr_t player_camera =
					g_vm->read<uintptr_t> ( g_vm->m_base_address + offsets::camera_base );

				if ( player_camera == 0 )
					return {};

				return g_vm->read<Vector3> ( player_camera + offsets::camera_pos );
			}

			auto units_to_m ( float units ) -> float {
				return units * 0.0254;
			}

			bool world_to_screen (
				Vector3 world,
				Vector2& out,
				Vector3 cam_pos,
				const RefDef_T& refdef
			)
			{
				Vector3 local = world - cam_pos;

				Vector3 trans;
				trans.x = local.Dot ( refdef.axis [RIGHT_VEC] );
				trans.y = local.Dot ( refdef.axis [UP_VEC] );
				trans.z = local.Dot ( refdef.axis [FORWARD_VEC] );

				if ( trans.z < 0.01f )
					return false;

				out.x = ( refdef.width * 0.5f ) * ( 1.f - ( trans.x / refdef.fov.x / trans.z ) );
				out.y = ( refdef.height * 0.5f ) * ( 1.f - ( trans.y / refdef.fov.y / trans.z ) );

				if ( out.x < 1.f || out.y < 1.f ||
					out.x > refdef.width || out.y > refdef.height )
					return false;

				return true;
			}

			bool w2s ( Vector3 world, Vector2& screen )
			{
				uintptr_t refdef_ptr = DecryptRefDef->GetRefDef ( );
				if ( refdef_ptr == 0 )
					return false;

				RefDef_T refdef = g_vm->read<RefDef_T> ( refdef_ptr );

				return world_to_screen ( world, screen, retrieve_camera_posistion ( ), refdef );
			}
		}

		namespace bone {

			bool validate_against ( Vector3 start, Vector3 end )
			{
				return start.distance_to ( end ) < 65.f;
			}

			auto get_bone ( const uintptr_t pointer, const Vector3& bone_position, const int Bone ) -> Vector3
			{
				Vector3 position = g_vm->read<Vector3> ( pointer + ( ( uint64_t ) Bone * 0x20 ) + 0x10 );

				if ( position.x == 0.0f && position.y == 0.0f && position.z == 0.0f ) {
					return Vector3 { 0, 0, 0 };
				}
				if ( position.x > 10000.0f || position.x < -10000.0f ||
					position.y > 10000.0f || position.y < -10000.0f ||
					position.z > 10000.0f || position.z < -10000.0f ) {
					return Vector3 { 0, 0, 0 };
				}

				if ( position.x != position.x || position.y != position.y || position.z != position.z ) {
					return Vector3 { 0, 0, 0 };
				}

				position.x += bone_position.x;
				position.y += bone_position.y;
				position.z += bone_position.z;

				return position;
			}

			std::vector<Vector3> get_multiple_bones ( const uintptr_t pointer, const Vector3& bone_position, const std::vector<int>& bone_indices ) {
				const int count = ( int ) bone_indices.size ( );
				std::vector<Vector3> raw ( count );
				std::vector<batch_read_entry> entries ( count );

				for ( int i = 0; i < count; ++i ) {
					entries [i].address = pointer + ( ( uint64_t ) bone_indices [i] * 0x20 ) + 0x10;
					entries [i].size    = sizeof ( Vector3 );
					entries [i].offset  = ( uint32_t ) ( i * sizeof ( Vector3 ) );
				}

				g_vm->read_batch ( entries.data ( ), ( uint32_t ) count, raw.data ( ), count * sizeof ( Vector3 ) );

				std::vector<Vector3> bone_positions;
				bone_positions.reserve ( count );

				for ( int i = 0; i < count; ++i ) {
					Vector3& p = raw [i];
					if ( ( p.x == 0.0f && p.y == 0.0f && p.z == 0.0f ) ||
						 p.x > 10000.0f || p.x < -10000.0f ||
						 p.y > 10000.0f || p.y < -10000.0f ||
						 p.z > 10000.0f || p.z < -10000.0f ||
						 p.x != p.x || p.y != p.y || p.z != p.z ) {
						bone_positions.emplace_back ( Vector3 { 0, 0, 0 } );
					} else {
						bone_positions.emplace_back ( Vector3 { p.x + bone_position.x, p.y + bone_position.y, p.z + bone_position.z } );
					}
				}

				return bone_positions;
			}

			auto retrieve_bone_position_vec ( const uintptr_t Client_Information ) -> Vector3
			{
				Vector3 information = g_vm->read<Vector3> ( Client_Information + offsets::bone::bone_base );
				return information;
			}


			auto bone_pointer ( uint64_t base, uint64_t index ) -> uint64_t
			{
				auto bone = g_vm->read<uintptr_t> ( base + ( index * offsets::bone::size ) + offsets::bone::offset );
				return bone;
			}

			bool bones_to_screen ( const Vector3* BonePosArray, Vector2* ScreenPosArray, const long Count )
			{
				for ( long i = 0; i < Count; ++i )
				{
					if ( !client::w2s ( BonePosArray [i], ScreenPosArray [i] ) )
						return false;
				}
				return true;
			}

		}

		namespace player
		{
			inline bool is_player_valid ( uintptr_t pAddr )
			{
				bool response = g_vm->read<bool> ( pAddr + offsets::player::valid );
				return response;
			}

			inline int local_player_index ( uintptr_t clientBase, uintptr_t clientInfo )
			{
				auto index = g_vm->read<uintptr_t> ( clientInfo + offsets::local_index );
				int read_return = g_vm->read<int> ( index + offsets::local_index_pos );
				return read_return;
			}

			inline Vector3 get_position ( uintptr_t pAddr )
			{
				auto local_position = g_vm->read<uintptr_t> ( pAddr + offsets::player::pos );
				Vector3 final_pos = g_vm->read<Vector3> ( local_position + 0x80 );
				return final_pos;
			}

			int is_visible ( uint32_t p_index )
			{
				auto sighted_enemy_fools = g_vm->read<client_bits_t> ( ( uintptr_t ) decryptions::decrypt_client_info ( ) + offsets::visible_bits );
				auto bitmask = 0x80000000 >> ( p_index & 0x1F );
				return sighted_enemy_fools.array [p_index >> 5] & bitmask;
			}

			// fast version using pre-cached client_info
			int is_visible_cached ( uintptr_t client_info, uint32_t p_index )
			{
				auto sighted = g_vm->read<client_bits_t> ( client_info + offsets::visible_bits );
				auto bitmask = 0x80000000 >> ( p_index & 0x1F );
				return sighted.array [p_index >> 5] & bitmask;
			}

			uint8_t team_id_check ( uintptr_t player ) {
				return g_vm->read<uint8_t> ( player + offsets::player::team );
			}

			inline std::string ConvertDistanceToString ( float dist ) {
				char buf [16];
				snprintf ( buf, sizeof ( buf ), "%.1fM", dist );
				return std::string ( buf );
			}

			inline bool box ( const Vector3& ent_position, Vector4& box ) {
				Vector2 headScreen, feetScreen;
				Vector3 headPos = ent_position;
				headPos.z += 70.0f;

				Vector3 feetPos = ent_position;
				if ( !engine::client::w2s ( headPos, headScreen ) || !engine::client::w2s ( feetPos, feetScreen ) ) {
					return false;
				}

				float height = abs ( headScreen.y - feetScreen.y );
				float width = height * 0.45f;

				if ( height <= 0 || width <= 0 ) {
					return false;
				}

				box.x = headScreen.x - width / 2;
				box.y = headScreen.y;
				box.z = width;
				box.w = height;

				return true;
			}

			inline uint64_t get_name_list ( ) {
				auto ptr = g_vm->read<uint64_t> ( g_vm->m_base_address + offsets::name_array );
				return ptr + offsets::name_array_pos;
			}

			inline name_entry get_name_entry ( uint32_t index ) {
				return g_vm->read<name_entry> ( get_name_list ( ) + ( index * offsets::name_array_size ) );
			}

			inline std::string get_name ( uintptr_t clientInfo, int index )
			{
				name_entry entry = get_name_entry ( index );
				if ( !entry.name [0] )
					return "unknown";
				return std::string ( entry.name );
			}

			inline int get_health ( uintptr_t clientInfo, int index )
			{
				name_entry entry = get_name_entry ( index );
				return entry.health;
			}

			inline bool w2s_cached ( Vector3 world, Vector2& screen, const RefDef_T& refdef, const Vector3& cam_pos )
			{
				Vector3 local = world - cam_pos;

				Vector3 trans;
				trans.x = local.Dot ( refdef.axis [RIGHT_VEC] );
				trans.y = local.Dot ( refdef.axis [UP_VEC] );
				trans.z = local.Dot ( refdef.axis [FORWARD_VEC] );

				if ( trans.z < 0.01f )
					return false;

				screen.x = ( refdef.width * 0.5f ) * ( 1.f - ( trans.x / refdef.fov.x / trans.z ) );
				screen.y = ( refdef.height * 0.5f ) * ( 1.f - ( trans.y / refdef.fov.y / trans.z ) );

				if ( screen.x < 1.f || screen.y < 1.f ||
					screen.x > refdef.width || screen.y > refdef.height )
					return false;

				return true;
			}
		}
	}

}