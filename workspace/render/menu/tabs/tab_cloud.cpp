#include "gui.h"
#include "gui_colors.h"
#include "../menu_text_input.h"
#include "../../../core/call of duty/settings/config.h"
#include "../../../../dependencies/oxorany/oxorany.h"
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>

namespace cloud_ui {

	inline std::mutex g_mtx;
	inline std::vector<cloud_config::ConfigEntry> g_mine;
	inline std::string g_status;
	inline std::chrono::steady_clock::time_point g_status_until {};
	inline std::atomic<bool> g_busy { false };
	inline std::atomic<bool> g_cloud_failed { false };   // true => fall back to local UI

	inline constexpr float k_status_seconds = 3.0f;

	inline bool profiles_loaded = false;
	inline menu_text_input::Field g_profile_name_field {};

	inline menu_text_input::Field g_name_field {};
	inline menu_text_input::Field g_hash_field {};

	inline void set_status ( const std::string& msg ) {
		std::lock_guard lock ( g_mtx );
		g_status = msg;
		g_status_until = std::chrono::steady_clock::now ( ) + std::chrono::milliseconds ( static_cast< int >( k_status_seconds * 1000.0f ) );
	}

	inline std::string get_status ( ) {
		std::lock_guard lock ( g_mtx );
		if ( !g_status.empty ( ) && std::chrono::steady_clock::now ( ) > g_status_until )
			g_status.clear ( );
		return g_status;
	}

	inline void run_async ( auto fn ) {
		if ( g_busy.exchange ( true ) )
			return;
		std::thread ( [fn] ( ) {
			fn ( );
			g_busy.store ( false );
		} ).detach ( );
	}

	inline void fetch_mine_locked ( ) {
		std::vector<cloud_config::ConfigEntry> list;
		std::string err;
		if ( cloud_client::list_mine ( list, err ) ) {
			g_cloud_failed.store ( false );
			std::lock_guard lock ( g_mtx );
			g_mine = std::move ( list );
		}
		else {
			g_cloud_failed.store ( true );
		}
	}

	inline void refresh_mine ( ) {
		run_async ( [] {
			std::vector<cloud_config::ConfigEntry> list;
			std::string err;
			if ( cloud_client::list_mine ( list, err ) ) {
				g_cloud_failed.store ( false );
				std::lock_guard lock ( g_mtx );
				g_mine = std::move ( list );
			}
			else {
				g_cloud_failed.store ( true );
				set_status ( err.empty ( ) ? oxorany ( "cloud unavailable — using local" ) : err );
			}
		} );
	}

	inline bool name_taken ( const std::string& name ) {
		std::lock_guard lock ( g_mtx );
		for ( const auto& cfg : g_mine ) {
			if ( cfg.name.size ( ) != name.size ( ) )
				continue;
			if ( _stricmp ( cfg.name.c_str ( ), name.c_str ( ) ) == 0 )
				return true;
		}
		return false;
	}

	inline void save_current ( ) {
		const std::string name = menu_text_input::text ( g_name_field );
		if ( name.size ( ) < 3 ) {
			set_status ( oxorany ( "name must be at least 3 characters" ) );
			return;
		}

		if ( name_taken ( name ) ) {
			set_status ( oxorany ( "a config with this name already exists" ) );
			return;
		}

		run_async ( [name] {
			std::string hash, err;
			if ( cloud_client::save_config ( name, hash, err ) ) {
				set_status ( oxorany ( "saved" ) );
				fetch_mine_locked ( );
			}
			else {
				set_status ( err.empty ( ) ? oxorany ( "save failed" ) : err );
			}
		} );
	}

	inline void load_hash_async ( const std::string& hash, const std::string& ok_msg = oxorany ( "loaded config" ) ) {
		if ( hash.size ( ) != cloud_config::k_hash_len ) {
			set_status ( oxorany ( "invalid config hash" ) );
			return;
		}

		run_async ( [hash, ok_msg] {
			std::string err;
			if ( cloud_client::load_by_hash ( hash, err ) )
				set_status ( ok_msg );
			else
				set_status ( err.empty ( ) ? oxorany ( "load failed" ) : err );
		} );
	}

	inline void load_hash ( ) {
		load_hash_async ( menu_text_input::text ( g_hash_field ) );
	}

	inline void load_owned ( const cloud_config::ConfigEntry& cfg ) {
		load_hash_async ( cfg.hash, std::string ( oxorany ( "loaded " ) ) + cfg.name );
	}

	inline void delete_entry ( const std::string& id ) {
		run_async ( [id] {
			std::string err;
			if ( cloud_client::delete_config ( id, err ) ) {
				set_status ( oxorany ( "deleted config" ) );
				fetch_mine_locked ( );
			}
			else {
				set_status ( err.empty ( ) ? oxorany ( "delete failed" ) : err );
			}
		} );
	}

	inline void init ( ) {
		g_name_field.filter = menu_text_input::Filter::text;
		g_name_field.max_len = cloud_config::k_max_name_len;
		g_hash_field.filter = menu_text_input::Filter::hex;
		g_hash_field.max_len = cloud_config::k_hash_len;
		g_profile_name_field.filter = menu_text_input::Filter::text;
		g_profile_name_field.max_len = 20;
	}

	inline void poll_inputs ( ) {
		menu_text_input::poll ( g_name_field, true );
		menu_text_input::poll ( g_hash_field, true );
		menu_text_input::poll ( g_profile_name_field, true );

		if ( g_hash_field.focused && menu_text_input::enter_pressed ( ) )
			load_hash ( );
	}

	inline void section ( const char* text ) {
		ImGui::PushFont ( font.spacegrotesk_medium [1] );
		ImGui::TextColored ( ui::colors::text, "%s", text );
		ImGui::PopFont ( );
	}

}

namespace cloud_input {
	void poll ( bool cloud_tab_active ) {
		if ( !menu_text_input::g_handlers_active ) {
			menu_text_input::deactivate_all ( );
			return;
		}

		if ( !cloud_tab_active ) {
			if ( menu_text_input::g_focused ) {
				menu_text_input::g_focused->focused = false;
				menu_text_input::g_focused->select_all = false;
				menu_text_input::g_focused = nullptr;
			}
			menu_text_input::sync_key_state ( );
			return;
		}
		cloud_ui::poll_inputs ( );
	}
}

void ui::tabs::cloud ( const TabCategory tab )
{
	if ( tab.name != "configs" )
		return;

	static bool initialized = false;
	static bool fetched_once = false;
	if ( !initialized ) {
		cloud_ui::init ( );
		initialized = true;
	}

	if ( !fetched_once && !cloud_ui::g_busy.load ( ) ) {
		fetched_once = true;
		cloud_ui::refresh_mine ( );
	}

	if ( !cloud_ui::profiles_loaded ) {
		cloud_ui::profiles_loaded = true;
		config::profiles::load_names ( );
	}

	const int full_height = static_cast< int >( ui::size.y - 95.0f );
	const bool enabled = !cloud_ui::g_busy.load ( );
	const bool cloud_down = cloud_ui::g_cloud_failed.load ( );

	if ( ui::begin_child_left ( oxorany ( "cloud" ), full_height ) ) {
		const float row_w = ImGui::GetContentRegionAvail ( ).x - 8.0f;

		if ( cloud_down ) {
			cloud_ui::section ( oxorany ( "cloud offline" ) );
			ImGui::Spacing ( );
			if ( ImGui::Button ( oxorany ( "save local" ), ImVec2 ( -1, 28 ) ) )
				config::save ( );
			if ( ImGui::Button ( oxorany ( "load local" ), ImVec2 ( -1, 28 ) ) )
				config::load ( );
			ImGui::Spacing ( );
			if ( ImGui::Button ( oxorany ( "retry" ), ImVec2 ( -1, 24 ) ) && enabled )
				cloud_ui::refresh_mine ( );
		}
		else {
			cloud_ui::section ( oxorany ( "cloud configs" ) );
			ImGui::Spacing ( );

			menu_text_input::draw ( oxorany ( "config name" ), cloud_ui::g_name_field, row_w, enabled, oxorany ( "name..." ) );
			if ( ImGui::Button ( oxorany ( "save" ), ImVec2 ( -1, 28 ) ) && enabled )
				cloud_ui::save_current ( );

			ImGui::Spacing ( );
			ImGui::Separator ( );
			ImGui::Spacing ( );

			std::vector<cloud_config::ConfigEntry> mine_copy;
			{ std::lock_guard lock ( cloud_ui::g_mtx ); mine_copy = cloud_ui::g_mine; }

			for ( const auto& cfg : mine_copy ) {
				ImGui::PushID ( cfg.id.c_str ( ) );
				cloud_ui::section ( cfg.name.c_str ( ) );
				if ( ImGui::Button ( oxorany ( "load" ), ImVec2 ( 72, 24 ) ) && enabled )
					cloud_ui::load_owned ( cfg );
				ImGui::SameLine ( );
				if ( menu_text_input::copy_label_button ( oxorany ( "copy" ), cfg.hash, enabled ) )
					cloud_ui::set_status ( oxorany ( "copied" ) );
				ImGui::SameLine ( );
				if ( ImGui::Button ( oxorany ( "delete" ), ImVec2 ( 72, 24 ) ) && enabled )
					cloud_ui::delete_entry ( cfg.id );
				ImGui::Separator ( );
				ImGui::PopID ( );
			}

			ImGui::Spacing ( );
			ImGui::Separator ( );
			ImGui::Spacing ( );

			cloud_ui::section ( oxorany ( "import hash" ) );
			menu_text_input::draw ( oxorany ( "config hash" ), cloud_ui::g_hash_field, row_w, enabled, oxorany ( "paste hash..." ) );
			if ( ImGui::Button ( oxorany ( "paste" ), ImVec2 ( 80, 28 ) ) && enabled )
				menu_text_input::paste_replace ( cloud_ui::g_hash_field );
			ImGui::SameLine ( );
			if ( ImGui::Button ( oxorany ( "load" ), ImVec2 ( -1, 28 ) ) && enabled )
				cloud_ui::load_hash ( );
		}

		const std::string status_copy = cloud_ui::get_status ( );
		if ( cloud_ui::g_busy.load ( ) )
			ImGui::TextWrapped ( oxorany ( "working..." ) );
		else if ( !status_copy.empty ( ) )
			ImGui::TextWrapped ( "%s", status_copy.c_str ( ) );
	}
	ImGui::EndChild ( );

	if ( ui::begin_child_right ( oxorany ( "profiles" ), full_height ) ) {
		const float row_w = ImGui::GetContentRegionAvail ( ).x - 8.0f;

		cloud_ui::section ( oxorany ( "local profiles" ) );
		ImGui::Spacing ( );

		for ( int i = 0; i < config::profiles::k_max_profiles; ++i ) {
			ImGui::PushID ( i + 100 );
			auto& slot = config::profiles::slots [i];
			if ( slot.exists ) {
				if ( config::profiles::active_profile == i )
					ImGui::TextColored ( ImVec4 ( ui::colors::main ), "%s", slot.name.c_str ( ) );
				else
					cloud_ui::section ( slot.name.c_str ( ) );

				if ( ImGui::Button ( oxorany ( "load" ), ImVec2 ( 60, 24 ) ) )
					if ( config::profiles::load_from ( i ) )
						cloud_ui::set_status ( std::string ( oxorany ( "loaded " ) ) + slot.name );
				ImGui::SameLine ( );
				if ( ImGui::Button ( oxorany ( "save" ), ImVec2 ( 60, 24 ) ) ) {
					config::profiles::save_to ( i, slot.name );
					cloud_ui::set_status ( std::string ( oxorany ( "saved " ) ) + slot.name );
				}
				ImGui::SameLine ( );
				if ( ImGui::Button ( oxorany ( "delete" ), ImVec2 ( 60, 24 ) ) ) {
					config::profiles::remove ( i );
					cloud_ui::set_status ( oxorany ( "deleted" ) );
				}
				ImGui::Separator ( );
			}
			ImGui::PopID ( );
		}

		ImGui::Spacing ( );
		menu_text_input::draw ( oxorany ( "profile name" ), cloud_ui::g_profile_name_field, row_w, true, oxorany ( "name..." ) );
		if ( ImGui::Button ( oxorany ( "save new" ), ImVec2 ( -1, 28 ) ) ) {
			std::string pname = menu_text_input::text ( cloud_ui::g_profile_name_field );
			if ( pname.size ( ) < 2 ) {
				cloud_ui::set_status ( oxorany ( "name too short" ) );
			} else {
				int free_slot = -1;
				for ( int i = 0; i < config::profiles::k_max_profiles; ++i )
					if ( !config::profiles::slots [i].exists ) { free_slot = i; break; }
				if ( free_slot == -1 )
					cloud_ui::set_status ( oxorany ( "all slots full" ) );
				else {
					config::profiles::save_to ( free_slot, pname );
					cloud_ui::set_status ( std::string ( oxorany ( "saved " ) ) + pname );
				}
			}
		}
	}
	ImGui::EndChild ( );
}
