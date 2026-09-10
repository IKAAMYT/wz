#pragma once

#include <Windows.h>
#include <imgui.h>
#include <gui.h>
#include "gui_colors.h"
#include <cstring>
#include <cctype>
#include <string>

namespace menu_text_input {

	enum class Filter {
		text,
		hex,
	};

	struct Field {
		char buf [128] = {};
		int len = 0;
		bool focused = false;
		bool select_all = false;
		Filter filter = Filter::text;
		int max_len = 127;
	};

	inline Field* g_focused = nullptr;
	inline bool g_prev_down [256] = {};
	inline bool g_handlers_active = false;
	inline Field* g_backspace_repeat_field = nullptr;
	inline ULONGLONG g_backspace_next_repeat = 0;

	inline void sync_key_state ( ) {
		for ( int i = 0; i < 256; ++i )
			g_prev_down [i] = ( GetAsyncKeyState ( i ) & 0x8000 ) != 0;
	}

	inline void deactivate_all ( ) {
		if ( g_focused ) {
			g_focused->focused = false;
			g_focused->select_all = false;
			g_focused = nullptr;
		}
		g_backspace_repeat_field = nullptr;
		sync_key_state ( );
	}

	inline void set_handlers_active ( bool active ) {
		if ( g_handlers_active == active )
			return;
		g_handlers_active = active;
		if ( !active )
			deactivate_all ( );
	}

	inline bool key_pressed ( int vk ) {
		const bool down = ( GetAsyncKeyState ( vk ) & 0x8000 ) != 0;
		const bool pressed = down && !g_prev_down [vk];
		g_prev_down [vk] = down;
		return pressed;
	}

	inline bool ctrl_down ( ) {
		return ( GetAsyncKeyState ( VK_CONTROL ) & 0x8000 ) != 0;
	}

	inline bool shift_down ( ) {
		return ( GetAsyncKeyState ( VK_SHIFT ) & 0x8000 ) != 0;
	}

	inline char vk_to_char ( int vk ) {
		const BYTE keyboard [256] = {};
		if ( !GetKeyboardState ( const_cast< LPBYTE >( keyboard ) ) )
			return 0;

		wchar_t chars [8] = {};
		const int count = ToUnicode ( vk, MapVirtualKeyA ( vk, MAPVK_VK_TO_VSC ), keyboard, chars, 8, 0 );
		if ( count != 1 )
			return 0;
		if ( chars [0] < 32 || chars [0] > 126 )
			return 0;
		return static_cast< char >( chars [0] );
	}

	inline bool is_hex_char ( char c ) {
		return std::isxdigit ( static_cast< unsigned char >( c ) ) != 0;
	}

	inline char normalize_hex ( char c ) {
		if ( c >= 'A' && c <= 'F' ) return static_cast< char >( c - 'A' + 'a' );
		return c;
	}

	inline void clear_field ( Field& field ) {
		field.len = 0;
		field.buf [0] = '\0';
		field.select_all = false;
	}

	inline void append_char ( Field& field, char c ) {
		if ( field.len >= field.max_len )
			return;

		if ( field.select_all ) {
			clear_field ( field );
			field.select_all = false;
		}

		if ( field.filter == Filter::hex ) {
			if ( !is_hex_char ( c ) )
				return;
			c = normalize_hex ( c );
		}

		field.buf [field.len++] = c;
		field.buf [field.len] = '\0';
	}

	inline void backspace ( Field& field ) {
		if ( field.select_all ) {
			clear_field ( field );
			field.select_all = false;
			return;
		}
		if ( field.len <= 0 )
			return;
		field.buf [--field.len] = '\0';
	}

	inline void poll_backspace ( Field& field ) {
		if ( !g_handlers_active )
			return;

		const bool down = ( GetAsyncKeyState ( VK_BACK ) & 0x8000 ) != 0;
		const bool was_down = g_prev_down [VK_BACK];

		if ( !down ) {
			if ( was_down )
				g_prev_down [VK_BACK] = false;
			if ( g_backspace_repeat_field == &field )
				g_backspace_repeat_field = nullptr;
			return;
		}

		const ULONGLONG now = GetTickCount64 ( );

		if ( !was_down ) {
			backspace ( field );
			g_backspace_repeat_field = &field;
			g_backspace_next_repeat = now + 400;
			g_prev_down [VK_BACK] = true;
			return;
		}

		if ( g_backspace_repeat_field != &field )
			return;

		if ( now < g_backspace_next_repeat )
			return;

		backspace ( field );
		g_backspace_next_repeat = now + 35;
	}

	inline void set_text ( Field& field, const std::string& value ) {
		clear_field ( field );
		for ( char c : value )
			append_char ( field, c );
	}

	inline bool copy_clipboard ( const std::string& text ) {
		if ( !OpenClipboard ( nullptr ) )
			return false;

		EmptyClipboard ( );

		const size_t size = text.size ( ) + 1;
		HGLOBAL mem = GlobalAlloc ( GMEM_MOVEABLE, size );
		if ( !mem ) {
			CloseClipboard ( );
			return false;
		}

		char* dst = static_cast< char* >( GlobalLock ( mem ) );
		if ( !dst ) {
			GlobalFree ( mem );
			CloseClipboard ( );
			return false;
		}

		memcpy ( dst, text.c_str ( ), size );
		GlobalUnlock ( mem );

		const bool ok = SetClipboardData ( CF_TEXT, mem ) != nullptr;
		CloseClipboard ( );
		return ok;
	}

	inline bool paste_replace ( Field& field ) {
		if ( !OpenClipboard ( nullptr ) )
			return false;

		clear_field ( field );

		HANDLE data = GetClipboardData ( CF_TEXT );
		if ( data ) {
			const char* clip = static_cast< const char* >( GlobalLock ( data ) );
			if ( clip ) {
				for ( const char* p = clip; *p; ++p ) {
					if ( field.filter == Filter::hex && !is_hex_char ( *p ) )
						continue;
					append_char ( field, *p );
				}
				GlobalUnlock ( data );
			}
		}

		CloseClipboard ( );
		return field.len > 0;
	}

	inline void paste_clipboard ( Field& field ) {
		paste_replace ( field );
	}

	inline void poll ( Field& field, bool allow_input ) {
		if ( !g_handlers_active || !allow_input || !field.focused )
			return;

		poll_backspace ( field );

		if ( ctrl_down ( ) && ( key_pressed ( 'A' ) || key_pressed ( 'a' ) ) ) {
			field.select_all = field.len > 0;
			return;
		}

		if ( ctrl_down ( ) && key_pressed ( 'V' ) ) {
			field.select_all = false;
			paste_clipboard ( field );
			return;
		}

		if ( key_pressed ( VK_ESCAPE ) ) {
			field.focused = false;
			field.select_all = false;
			if ( g_focused == &field )
				g_focused = nullptr;
			return;
		}

		if ( ctrl_down ( ) )
			return;

		static const int keys [] = {
			'0','1','2','3','4','5','6','7','8','9',
			'A','B','C','D','E','F','G','H','I','J','K','L','M',
			'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
			VK_NUMPAD0, VK_NUMPAD1, VK_NUMPAD2, VK_NUMPAD3, VK_NUMPAD4,
			VK_NUMPAD5, VK_NUMPAD6, VK_NUMPAD7, VK_NUMPAD8, VK_NUMPAD9,
			VK_SPACE, VK_OEM_MINUS, VK_OEM_1, VK_OEM_2, VK_OEM_3,
			VK_OEM_4, VK_OEM_5, VK_OEM_6, VK_OEM_7, VK_OEM_COMMA, VK_OEM_PERIOD,
		};

		for ( int vk : keys ) {
			if ( !key_pressed ( vk ) )
				continue;
			const char c = vk_to_char ( vk );
			if ( c )
				append_char ( field, c );
		}
	}

	inline bool draw ( const char* label, Field& field, float width, bool allow_focus, const char* placeholder = "click to type..." ) {
		ImGui::PushFont ( font.spacegrotesk_medium [1] );
		ImGui::TextColored ( ui::colors::text, "%s", label );
		ImGui::PopFont ( );

		const ImVec2 pos = ImGui::GetCursorScreenPos ( );
		const float height = 28.0f;
		const ImRect rect ( pos, ImVec2 ( pos.x + width, pos.y + height ) );

		ImGui::PushID ( label );
		ImGui::InvisibleButton ( "##input", ImVec2 ( width, height ) );
		if ( allow_focus && g_handlers_active && ImGui::IsItemClicked ( ) ) {
			if ( g_focused && g_focused != &field )
				g_focused->focused = false;
			field.focused = true;
			field.select_all = false;
			g_focused = &field;
		}
		ImGui::PopID ( );

		ImDrawList* dl = ImGui::GetWindowDrawList ( );
		const ImU32 main_col = ImGui::GetColorU32 ( ui::colors::main );
		const ImU32 main_col_select = ImGui::GetColorU32 ( ImVec4 (
			ui::colors::main.x, ui::colors::main.y, ui::colors::main.z, 0.28f ) );
		const ImU32 bg = field.focused ? IM_COL32 ( 34, 34, 42, 255 ) : IM_COL32 ( 24, 24, 30, 255 );
		const ImU32 border = field.focused ? main_col : IM_COL32 ( 48, 48, 58, 255 );
		dl->AddRectFilled ( rect.Min, rect.Max, bg, 4.0f );
		dl->AddRect ( rect.Min, rect.Max, border, 4.0f );

		const char* display = field.len > 0 ? field.buf : placeholder;
		const ImU32 text_col = field.len > 0 ? IM_COL32 ( 230, 230, 235, 255 ) : IM_COL32 ( 120, 120, 130, 255 );

		if ( field.select_all && field.len > 0 ) {
			const ImVec2 text_size = ImGui::CalcTextSize ( field.buf );
			dl->AddRectFilled (
				ImVec2 ( rect.Min.x + 7.0f, rect.Min.y + 4.0f ),
				ImVec2 ( rect.Min.x + 9.0f + text_size.x, rect.Max.y - 4.0f ),
				main_col_select, 3.0f );
		}

		dl->AddText ( ImVec2 ( rect.Min.x + 8.0f, rect.Min.y + 6.0f ), text_col, display );

		if ( field.focused && !field.select_all ) {
			const float t = static_cast< float >( ImGui::GetTime ( ) );
			if ( static_cast< int >( t * 2.0f ) % 2 == 0 ) {
				const ImVec2 text_size = ImGui::CalcTextSize ( field.buf );
				dl->AddLine (
					ImVec2 ( rect.Min.x + 8.0f + text_size.x + 1.0f, rect.Min.y + 5.0f ),
					ImVec2 ( rect.Min.x + 8.0f + text_size.x + 1.0f, rect.Max.y - 5.0f ),
					main_col );
			}
		}

		ImGui::Spacing ( );
		return field.focused;
	}

	inline std::string text ( const Field& field ) {
		return std::string ( field.buf, field.len );
	}

	inline bool copy_label_button ( const char* id, const std::string& value, bool enabled ) {
		if ( ImGui::Button ( id, ImVec2 ( 72, 24 ) ) && enabled && !value.empty ( ) ) {
			copy_clipboard ( value );
			return true;
		}
		return false;
	}

	inline bool enter_pressed ( ) {
		if ( !g_handlers_active )
			return false;
		return key_pressed ( VK_RETURN );
	}

}
