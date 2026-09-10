#define IMGUI_DEFINE_MATH_OPERATORS

#include "gui.h"
#include "gui_font.h"
#include "gui_colors.h"
#include "gui_image.h"
#include <pixel7.hpp>

#include "imgui_freetype.h"
#include "../oxorany/oxorany.h"

extern ID3D11ShaderResourceView* loot_stim;
extern ID3D11ShaderResourceView* loot_cash;
extern ID3D11ShaderResourceView* loot_crate;
extern ID3D11ShaderResourceView* loot_armor;

std::vector<TabCategory> categories;

std::string to_lower(const std::string& str) {
	std::string result = str;
	std::transform(result.begin(), result.end(), result.begin(),
		[](unsigned char c) { return std::tolower(c); });
	return result;
}

void ui::initialize_images()
{
	D3DX11_IMAGE_LOAD_INFO info; ID3DX11ThreadPump* pump{ nullptr };
	if (images::background == nullptr) D3DX11CreateShaderResourceViewFromMemory(g_pd3dDevice, wallpaper, sizeof(wallpaper), &info, pump, &images::background, 0);

}

void ui::initialize_fonts()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = NULL;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImFontConfig spacegrotesk_cfg;
	spacegrotesk_cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_ForceAutoHint | ImGuiFreeTypeBuilderFlags_LoadColor;
	
	spacegrotesk_cfg.Density = ui::dpi_scale;
	spacegrotesk_cfg.OversampleH = 1;
	spacegrotesk_cfg.OversampleV = 1;

	font.montserrat_semibold[0] = io.Fonts->AddFontFromMemoryTTF(PoppinsMedium, sizeof(PoppinsMedium), 22, &spacegrotesk_cfg, io.Fonts->GetGlyphRangesCyrillic());

	font.spacegrotesk_medium[0] = io.Fonts->AddFontFromMemoryTTF(PoppinsMedium, sizeof(PoppinsMedium), 24.0f, &spacegrotesk_cfg, io.Fonts->GetGlyphRangesCyrillic());
	font.spacegrotesk_medium[1] = io.Fonts->AddFontFromMemoryTTF(PoppinsMedium, sizeof(PoppinsMedium), 16.0f, &spacegrotesk_cfg, io.Fonts->GetGlyphRangesCyrillic());
	font.spacegrotesk_medium[2] = io.Fonts->AddFontFromMemoryTTF(PoppinsMedium, sizeof(PoppinsMedium), 20.0f, &spacegrotesk_cfg, io.Fonts->GetGlyphRangesCyrillic());
	font.tab_icon = io.Fonts->AddFontFromMemoryTTF(tab_font, sizeof(tab_font), 16.0f, &spacegrotesk_cfg, io.Fonts->GetGlyphRangesCyrillic());
	font.widget_icon[0] = io.Fonts->AddFontFromMemoryTTF(tab_font, sizeof(tab_font), 12.0f, &spacegrotesk_cfg, io.Fonts->GetGlyphRangesCyrillic());
	font.widget_icon[1] = io.Fonts->AddFontFromMemoryTTF(tab_font, sizeof(tab_font), 16.0f, &spacegrotesk_cfg, io.Fonts->GetGlyphRangesCyrillic());
	font.notification_icon = io.Fonts->AddFontFromMemoryTTF(notification_font, sizeof(notification_font), 12.0f, &spacegrotesk_cfg, io.Fonts->GetGlyphRangesCyrillic());

	io.FontDefault = font.spacegrotesk_medium[1];
}

void ui::items::text_shadow(ImDrawList* draw_list, ImVec2 pos, ImColor text_color, const char* text, ImFont* font = nullptr, float font_size = 0.0f)
{
	ImVec2 shadow_offset = ImVec2(1.0f, 1.0f); 
	ImColor shadow_color = IM_COL32(0, 0, 0, text_color.Value.w * 255); 

	if (font == nullptr)
		font = ImGui::GetFont();

	if (font_size == 0.0f)
		font_size = ImGui::GetFontSize();

	draw_list->AddText(font, font_size, ImVec2(pos.x + shadow_offset.x, pos.y + shadow_offset.y), shadow_color, text);

	draw_list->AddText(font, font_size, pos, text_color, text);
}

namespace {
	struct info_tooltip_state {
		float alpha = 0.0f;
		float scale = 0.94f;
		float hover_time = 0.0f;
	};

	struct deferred_tooltip {
		std::string text;
		ImRect icon_bb;
		float alpha;
		float scale;
	};
	std::vector<deferred_tooltip> g_deferred_tooltips;

	void flush_deferred_tooltips ( ) {
		if ( g_deferred_tooltips.empty ( ) ) return;
		ImDrawList* fg = ImGui::GetForegroundDrawList ( );
		const ImVec2 display = GImGui->IO.DisplaySize;
		const float max_width = 248.0f;
		const ImVec2 padding ( 12.0f, 10.0f );

		for ( auto& t : g_deferred_tooltips ) {
			ImGui::PushFont ( font.spacegrotesk_medium [1] );
			const ImVec2 text_size = ImGui::CalcTextSize ( t.text.c_str ( ), nullptr, false, max_width );
			ImGui::PopFont ( );

			const ImVec2 pop_size ( text_size.x + padding.x * 2.0f, text_size.y + padding.y * 2.0f );
			ImVec2 pop_pos ( t.icon_bb.Max.x + 8.0f, t.icon_bb.GetCenter ( ).y - pop_size.y * 0.5f );

			if ( pop_pos.x + pop_size.x > display.x - 8.0f ) pop_pos.x = t.icon_bb.Min.x - pop_size.x - 8.0f;
			if ( pop_pos.y + pop_size.y > display.y - 8.0f ) pop_pos.y = display.y - pop_size.y - 8.0f;
			if ( pop_pos.y < 8.0f ) pop_pos.y = 8.0f;

			const ImVec2 pop_center ( pop_pos.x + pop_size.x * 0.5f, pop_pos.y + pop_size.y * 0.5f );
			const ImVec2 scaled_size ( pop_size.x * t.scale, pop_size.y * t.scale );
			const ImVec2 scaled_pos ( pop_center.x - scaled_size.x * 0.5f, pop_center.y - scaled_size.y * 0.5f + ( 1.0f - t.alpha ) * 6.0f );

			const ImRect pop_bb ( scaled_pos, scaled_pos + scaled_size );

			fg->AddRectFilled ( pop_bb.Min, pop_bb.Max,
				ImGui::GetColorU32 ( ImVec4 ( ui::colors::child::top_background.x, ui::colors::child::top_background.y, ui::colors::child::top_background.z, ui::colors::child::top_background.w * t.alpha ) ), 6.0f );
			fg->AddRect ( pop_bb.Min, pop_bb.Max,
				ImGui::GetColorU32 ( ImVec4 ( ui::colors::outline.x, ui::colors::outline.y, ui::colors::outline.z, ui::colors::outline.w * t.alpha * 2.0f ) ), 6.0f, 0, 1.0f );
			fg->AddRectFilled ( ImVec2 ( pop_bb.Min.x, pop_bb.Min.y + 4.0f ), ImVec2 ( pop_bb.Min.x + 2.5f, pop_bb.Max.y - 4.0f ),
				ImGui::GetColorU32 ( ImVec4 ( ui::colors::main.x, ui::colors::main.y, ui::colors::main.z, t.alpha ) ), 2.0f );

			ImGui::PushFont ( font.spacegrotesk_medium [1] );
			fg->AddText ( font.spacegrotesk_medium [1], font.spacegrotesk_medium [1]->FontSize, pop_bb.Min + padding,
				ImGui::GetColorU32 ( ImVec4 ( ui::colors::text.x, ui::colors::text.y, ui::colors::text.z, t.alpha * 0.92f ) ),
				t.text.c_str ( ), nullptr, max_width );
			ImGui::PopFont ( );
		}
		g_deferred_tooltips.clear ( );
	}

	void draw_info_popout ( ImGuiID id, const char* tooltip, const ImRect& icon_bb, info_tooltip_state& state ) {
		if ( !tooltip || !tooltip [0] ) return;

		ImGuiContext& g = *GImGui;
		const float dt = g.IO.DeltaTime;
		const bool show = state.hover_time >= 0.18f;
		state.alpha = ImLerp ( state.alpha, show ? 1.0f : 0.0f, dt * 16.0f );
		state.scale = ImLerp ( state.scale, show ? 1.0f : 0.94f, dt * 16.0f );
		if ( state.alpha <= 0.01f ) return;

		g_deferred_tooltips.push_back ( { std::string ( tooltip ), icon_bb, state.alpha, state.scale } );

		IM_UNUSED ( id );
	}

	void draw_information_glyph ( ImDrawList* draw_list, ImVec2 center, float diameter, ImU32 color ) {
		const float radius = diameter * 0.5f;
		const float ring_thickness = ImMax ( 1.0f, diameter * 0.075f );
		draw_list->AddCircle ( center, radius - ring_thickness * 0.5f, color, 48, ring_thickness );

		const float dot_y = center.y - diameter * 0.19f;
		draw_list->AddCircleFilled ( ImVec2 ( center.x, dot_y ), diameter * 0.085f, color, 16 );

		const float stem_w = diameter * 0.105f;
		draw_list->AddRectFilled (
			ImVec2 ( center.x - stem_w * 0.5f, center.y - diameter * 0.03f ),
			ImVec2 ( center.x + stem_w * 0.5f, center.y + diameter * 0.25f ),
			color, stem_w * 0.5f );
	}
}

void ui::flush_tooltips ( ) { flush_deferred_tooltips ( ); }

bool ui::items::info_icon ( const char* id, const char* tooltip )
{
	ImGuiWindow* window = ImGui::GetCurrentWindow ( );
	if ( window->SkipItems ) return false;

	ImGuiContext& g = *GImGui;
	const ImGuiID widget_id = window->GetID ( id );
	const float icon_size = 15.0f;
	const float hit_size = 17.0f;

	const ImVec2 pos = window->DC.CursorPos;
	const ImRect bb ( pos, pos + ImVec2 ( hit_size, hit_size ) );
	ImGui::ItemSize ( bb );
	if ( !ImGui::ItemAdd ( bb, widget_id ) ) return false;

	bool hovered = false, held = false;
	ImGui::ButtonBehavior ( bb, widget_id, &hovered, &held );

	static std::unordered_map<ImGuiID, info_tooltip_state> anim;
	auto& state = anim [widget_id];

	if ( hovered ) state.hover_time += g.IO.DeltaTime;
	else state.hover_time = 0.0f;

	const ImVec4 idle_col = ImVec4 ( 0.62f, 0.62f, 0.68f, 0.95f );
	const float hover_mix = ImClamp ( state.hover_time * 7.0f, 0.0f, 1.0f );
	const ImVec4 icon_col = ImLerp ( idle_col, ui::colors::main, hover_mix );

	draw_information_glyph ( window->DrawList, bb.GetCenter ( ), icon_size, ImGui::GetColorU32 ( icon_col ) );

	if ( hovered ) ImGui::SetMouseCursor ( ImGuiMouseCursor_Hand );

	const ImRect icon_bb ( bb.GetCenter ( ) - ImVec2 ( icon_size * 0.5f, icon_size * 0.5f ), bb.GetCenter ( ) + ImVec2 ( icon_size * 0.5f, icon_size * 0.5f ) );
	draw_info_popout ( widget_id, tooltip, icon_bb, state );

	return false;
}

namespace {
	void draw_safe_glyph ( ImDrawList* dl, ImVec2 center, float diameter, ImU32 color ) {
		const float r = diameter * 0.5f;
		const float thick = ImMax ( 1.4f, diameter * 0.12f );
		dl->AddCircle ( center, r - thick * 0.5f, color, 48, thick );
		ImVec2 a ( center.x - r * 0.30f, center.y + r * 0.02f );
		ImVec2 b ( center.x - r * 0.05f, center.y + r * 0.28f );
		ImVec2 c ( center.x + r * 0.32f, center.y - r * 0.24f );
		ImVec2 pts [] = { a, b, c };
		dl->AddPolyline ( pts, 3, color, ImDrawFlags_None, thick );
	}

	void draw_unsafe_glyph ( ImDrawList* dl, ImVec2 center, float diameter, ImU32 color ) {
		const float r = diameter * 0.5f;
		const float thick = ImMax ( 1.4f, diameter * 0.1f );
		ImVec2 top ( center.x, center.y - r );
		ImVec2 bl ( center.x - r * 0.9f, center.y + r * 0.7f );
		ImVec2 br ( center.x + r * 0.9f, center.y + r * 0.7f );
		dl->AddTriangle ( top, bl, br, color, thick );
		const float stem_w = diameter * 0.09f;
		dl->AddRectFilled (
			ImVec2 ( center.x - stem_w * 0.5f, center.y - r * 0.28f ),
			ImVec2 ( center.x + stem_w * 0.5f, center.y + r * 0.15f ),
			color, stem_w * 0.5f );
		dl->AddCircleFilled ( ImVec2 ( center.x, center.y + r * 0.36f ), diameter * 0.07f, color, 16 );
	}

	struct confirm_popup_state {
		bool pending = false;
		float alpha = 0.0f;
	};
}

bool ui::items::safe_icon ( const char* id, const char* tooltip )
{
	ImGuiWindow* window = ImGui::GetCurrentWindow ( );
	if ( window->SkipItems ) return false;

	ImGuiContext& g = *GImGui;
	const ImGuiID widget_id = window->GetID ( id );
	const float icon_size = 15.0f;
	const float hit_size = 17.0f;

	const ImVec2 pos = window->DC.CursorPos;
	const ImRect bb ( pos, pos + ImVec2 ( hit_size, hit_size ) );
	ImGui::ItemSize ( bb );
	if ( !ImGui::ItemAdd ( bb, widget_id ) ) return false;

	bool hovered = false, held = false;
	ImGui::ButtonBehavior ( bb, widget_id, &hovered, &held );

	static std::unordered_map<ImGuiID, info_tooltip_state> anim;
	auto& state = anim [widget_id];

	if ( hovered ) state.hover_time += g.IO.DeltaTime;
	else state.hover_time = 0.0f;

	const ImVec4 idle_col = ImVec4 ( 0.30f, 0.75f, 0.35f, 0.85f );
	const ImVec4 hover_col = ImVec4 ( 0.35f, 0.90f, 0.40f, 1.0f );
	const float hover_mix = ImClamp ( state.hover_time * 7.0f, 0.0f, 1.0f );
	const ImVec4 icon_col = ImLerp ( idle_col, hover_col, hover_mix );

	draw_safe_glyph ( window->DrawList, bb.GetCenter ( ), icon_size, ImGui::GetColorU32 ( icon_col ) );

	if ( hovered ) ImGui::SetMouseCursor ( ImGuiMouseCursor_Hand );

	const ImRect icon_bb ( bb.GetCenter ( ) - ImVec2 ( icon_size * 0.5f, icon_size * 0.5f ), bb.GetCenter ( ) + ImVec2 ( icon_size * 0.5f, icon_size * 0.5f ) );
	draw_info_popout ( widget_id, tooltip, icon_bb, state );

	return false;
}

bool ui::items::unsafe_icon ( const char* id, const char* tooltip )
{
	ImGuiWindow* window = ImGui::GetCurrentWindow ( );
	if ( window->SkipItems ) return false;

	ImGuiContext& g = *GImGui;
	const ImGuiID widget_id = window->GetID ( id );
	const float icon_size = 15.0f;
	const float hit_size = 17.0f;

	const ImVec2 pos = window->DC.CursorPos;
	const ImRect bb ( pos, pos + ImVec2 ( hit_size, hit_size ) );
	ImGui::ItemSize ( bb );
	if ( !ImGui::ItemAdd ( bb, widget_id ) ) return false;

	bool hovered = false, held = false;
	ImGui::ButtonBehavior ( bb, widget_id, &hovered, &held );

	static std::unordered_map<ImGuiID, info_tooltip_state> anim;
	auto& state = anim [widget_id];

	if ( hovered ) state.hover_time += g.IO.DeltaTime;
	else state.hover_time = 0.0f;

	const ImVec4 idle_col = ImVec4 ( 0.95f, 0.60f, 0.15f, 0.85f );
	const ImVec4 hover_col = ImVec4 ( 1.0f, 0.70f, 0.20f, 1.0f );
	const float hover_mix = ImClamp ( state.hover_time * 7.0f, 0.0f, 1.0f );
	const ImVec4 icon_col = ImLerp ( idle_col, hover_col, hover_mix );

	draw_unsafe_glyph ( window->DrawList, bb.GetCenter ( ), icon_size, ImGui::GetColorU32 ( icon_col ) );

	if ( hovered ) ImGui::SetMouseCursor ( ImGuiMouseCursor_Hand );

	const ImRect icon_bb ( bb.GetCenter ( ) - ImVec2 ( icon_size * 0.5f, icon_size * 0.5f ), bb.GetCenter ( ) + ImVec2 ( icon_size * 0.5f, icon_size * 0.5f ) );
	draw_info_popout ( widget_id, tooltip, icon_bb, state );

	return false;
}

bool ui::items::checkbox_unsafe ( const char* label, bool* v, const char* warning )
{
	ImGuiWindow* window = ImGui::GetCurrentWindow ( );
	ImGuiContext& g = *GImGui;
	const ImGuiID base_id = window->GetID ( label );

	static std::unordered_map<ImGuiID, confirm_popup_state> popups;
	auto& popup = popups [base_id];

	if ( !*v && popup.pending ) {
		popup.alpha = ImLerp ( popup.alpha, 1.0f, g.IO.DeltaTime * 14.0f );
		const float a = popup.alpha;

		ImDrawList* fg = ImGui::GetForegroundDrawList ( );
		ImVec2 display = g.IO.DisplaySize;

		fg->AddRectFilled ( ImVec2 ( 0, 0 ), display, ImGui::GetColorU32 ( ImVec4 ( 0, 0, 0, 0.45f * a ) ) );

		const float pw = 280.0f;
		const float pad = 20.0f;
		const float btn_h = 30.0f;
		const float gap = 8.0f;
		const float rnd = 10.0f;

		const ImVec4 orange ( 0.95f, 0.60f, 0.15f, 1.0f );
		const ImVec4 orange_dim ( 0.95f, 0.60f, 0.15f, 0.3f );

		const char* msg = warning && warning [0] ? warning : "this feature may be detected. are you sure?";
		ImGui::PushFont ( font.spacegrotesk_medium [1] );
		ImVec2 msg_size = ImGui::CalcTextSize ( msg, nullptr, false, pw - pad * 2.0f );
		ImGui::PopFont ( );

		const float title_h = font.spacegrotesk_medium [0] ? font.spacegrotesk_medium [0]->FontSize : 20.0f;
		const float ph = pad + title_h + 6.0f + msg_size.y + 16.0f + btn_h + pad;

		const ImVec2 wmin ( ( display.x - pw ) * 0.5f, ( display.y - ph ) * 0.5f );
		const ImVec2 wmax ( wmin.x + pw, wmin.y + ph );

		fg->AddRectFilled ( wmin, wmax, ImGui::GetColorU32 ( ImVec4 ( ui::colors::background.x, ui::colors::background.y, ui::colors::background.z, 0.97f * a ) ), rnd );
		fg->AddRect ( wmin, wmax, ImGui::GetColorU32 ( ImVec4 ( orange_dim.x, orange_dim.y, orange_dim.z, orange_dim.w * a ) ), rnd );

		fg->AddRectFilled ( ImVec2 ( wmin.x, wmin.y + 4.0f ), ImVec2 ( wmin.x + 3.0f, wmax.y - 4.0f ), ImGui::GetColorU32 ( ImVec4 ( orange.x, orange.y, orange.z, a ) ), 2.0f );

		float cx = wmin.x + pad;
		float cy = wmin.y + pad;

		ImVec2 tri_center ( cx + 8.0f, cy + title_h * 0.5f );
		draw_unsafe_glyph ( fg, tri_center, 16.0f, ImGui::GetColorU32 ( ImVec4 ( orange.x, orange.y, orange.z, a ) ) );

		ImGui::PushFont ( font.spacegrotesk_medium [0] );
		fg->AddText ( font.spacegrotesk_medium [0], font.spacegrotesk_medium [0]->FontSize,
			ImVec2 ( cx + 22.0f, cy ), ImGui::GetColorU32 ( ImVec4 ( orange.x, orange.y, orange.z, a ) ), "warning" );
		ImGui::PopFont ( );
		cy += title_h + 6.0f;

		ImGui::PushFont ( font.spacegrotesk_medium [1] );
		fg->AddText ( font.spacegrotesk_medium [1], font.spacegrotesk_medium [1]->FontSize,
			ImVec2 ( cx, cy ), ImGui::GetColorU32 ( ImVec4 ( ui::colors::text.x, ui::colors::text.y, ui::colors::text.z, 0.85f * a ) ),
			msg, nullptr, pw - pad * 2.0f );
		ImGui::PopFont ( );
		cy += msg_size.y + 16.0f;

		const float btn_w = ( pw - pad * 2.0f - gap ) * 0.5f;
		const ImVec2 enable_min ( cx, cy );
		const ImVec2 enable_max ( cx + btn_w, cy + btn_h );
		const ImVec2 cancel_min ( cx + btn_w + gap, cy );
		const ImVec2 cancel_max ( cx + btn_w + gap + btn_w, cy + btn_h );

		ImVec2 mouse = g.IO.MousePos;
		bool hover_enable = ( mouse.x >= enable_min.x && mouse.x <= enable_max.x && mouse.y >= enable_min.y && mouse.y <= enable_max.y );
		bool hover_cancel = ( mouse.x >= cancel_min.x && mouse.x <= cancel_max.x && mouse.y >= cancel_min.y && mouse.y <= cancel_max.y );
		bool clicked = ImGui::IsMouseClicked ( 0 );

		fg->AddRectFilled ( enable_min, enable_max,
			ImGui::GetColorU32 ( ImVec4 ( orange.x, orange.y, orange.z, ( hover_enable ? 1.0f : 0.75f ) * a ) ), 6.0f );
		fg->AddRectFilled ( cancel_min, cancel_max,
			ImGui::GetColorU32 ( ImVec4 ( ui::colors::checkbox::background.x, ui::colors::checkbox::background.y, ui::colors::checkbox::background.z, ( hover_cancel ? 1.0f : 0.8f ) * a ) ), 6.0f );
		fg->AddRect ( cancel_min, cancel_max,
			ImGui::GetColorU32 ( ImVec4 ( ui::colors::outline.x, ui::colors::outline.y, ui::colors::outline.z, a ) ), 6.0f );

		ImGui::PushFont ( font.spacegrotesk_medium [1] );
		{
			const char* el = "enable";
			ImVec2 es = ImGui::CalcTextSize ( el );
			fg->AddText ( ImVec2 ( enable_min.x + ( btn_w - es.x ) * 0.5f, enable_min.y + ( btn_h - es.y ) * 0.5f ),
				ImGui::GetColorU32 ( ImVec4 ( 0, 0, 0, a ) ), el );
		}
		{
			const char* cl = "cancel";
			ImVec2 cs = ImGui::CalcTextSize ( cl );
			fg->AddText ( ImVec2 ( cancel_min.x + ( btn_w - cs.x ) * 0.5f, cancel_min.y + ( btn_h - cs.y ) * 0.5f ),
				ImGui::GetColorU32 ( ImVec4 ( ui::colors::text.x, ui::colors::text.y, ui::colors::text.z, 0.9f * a ) ), cl );
		}
		ImGui::PopFont ( );

		if ( hover_enable || hover_cancel ) ImGui::SetMouseCursor ( ImGuiMouseCursor_Hand );

		if ( clicked && hover_enable ) {
			*v = true;
			popup.pending = false;
			popup.alpha = 0.0f;
		}
		if ( clicked && hover_cancel ) {
			popup.pending = false;
			popup.alpha = 0.0f;
		}

		return false;
	}

	bool was = *v;
	bool changed = ImGui::Checkbox ( label, v );

	if ( changed && *v && !was ) {
		*v = false;
		popup.pending = true;
		popup.alpha = 0.0f;
		changed = false;
	}

	const char* tip = warning && warning [0] ? warning : "this feature may be detected";
	ImGui::SameLine ( 0.0f, 0.0f );
	ImGui::SetCursorPosX ( ImGui::GetContentRegionMax ( ).x - 17.0f );
	unsafe_icon ( ( std::string ( label ) + "##unsafe" ).c_str ( ), tip );

	return changed;
}

bool ui::items::checkbox ( const char* label, bool* v, const char* info )
{
	const bool changed = ImGui::Checkbox ( label, v );
	if ( info && info [0] ) {
		ImGui::SameLine ( 0.0f, 0.0f );
		ImGui::SetCursorPosX ( ImGui::GetContentRegionMax ( ).x - 17.0f );
		info_icon ( ( std::string ( label ) + "##info" ).c_str ( ), info );
	}
	return changed;
}

struct Notification {
	std::string message;
	std::string icon;
	float alpha = 0.0f;
	float timer = 0.0f;
	bool fading_out = false;
	float duration = 3.0f;
	float target_y = 0.0f;
	float current_y = 0.0f;
	ImVec4 bg_color = ui::colors::background_dark;
	ImVec4 icon_color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
};

std::vector<Notification> notifications;

void ui::add_notification(const std::string& icon, const std::string& msg, const ImVec4& icon_color) {
	notifications.push_back({ msg, icon, 0.0f, 0.0f, false, 3.0f, 0.0f, 0.0f,
							   ImVec4(0.12f, 0.12f, 0.12f, 1.0f), icon_color });
}

void ui::render_notification() {
	const float padding = 14.0f;
	const float right_padding = 10.0f;
	const ImVec2 viewport_pos = ImGui::GetMainViewport()->WorkPos;
	const ImVec2 viewport_size = ImGui::GetMainViewport()->Size;
	float total_offset = 0.0f;

	for (int i = 0; i < notifications.size(); ++i) {
		Notification& note = notifications[i];
		float delta = ImGui::GetIO().DeltaTime;
		note.timer += delta;

		if (!note.fading_out) {
			note.alpha = ImClamp(note.alpha + delta * 3.0f, 0.0f, 1.0f);
			if (note.timer >= note.duration)
				note.fading_out = true;
		}
		else {
			note.alpha = ImClamp(note.alpha - delta * 2.0f, 0.0f, 1.0f);
		}

		if (note.fading_out && note.alpha <= 0.0f) {
			notifications.erase(notifications.begin() + i);
			--i;
			continue;
		}

		ImGui::PushFont(font.notification_icon);
		ImVec2 icon_size = ImGui::CalcTextSize(note.icon.c_str());
		ImGui::PopFont();

		ImGui::PushFont(font.spacegrotesk_medium[1]);
		ImVec2 text_size = ImGui::CalcTextSize(note.message.c_str());
		ImGui::PopFont();

		ImVec2 box_size(icon_size.x + text_size.x + 30.0f + right_padding, max(icon_size.y, text_size.y) + 10.0f);

		float target_y = viewport_pos.y + padding + total_offset;
		if (note.target_y == 0.0f)
			note.target_y = target_y;

		note.target_y = target_y;
		note.current_y += (note.target_y - note.current_y) * delta * 10.0f;

		ImVec2 pos(viewport_pos.x + viewport_size.x - box_size.x - padding, note.current_y);

		ImDrawList* draw = ImGui::GetForegroundDrawList();
		draw->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.x + box_size.x, pos.y + box_size.y), ImColor(note.bg_color.x, note.bg_color.y, note.bg_color.z, note.alpha), 6.0f);
		draw->AddRect(ImVec2(pos.x, pos.y), ImVec2(pos.x + box_size.x, pos.y + box_size.y), ImGui::GetColorU32(ui::colors::outline), 6.0f);

		ImGui::PushFont(font.notification_icon);
		draw->AddText(
			ImVec2(pos.x + 10, pos.y + 7),
			ImColor(note.icon_color.x, note.icon_color.y, note.icon_color.z, note.alpha),
			note.icon.c_str()
		);
		ImGui::PopFont();

		float line_x = pos.x + 15 + icon_size.x + 5.0f;
		float line_y_start = pos.y + 5.0f;
		float line_y_end = pos.y + box_size.y - 5.0f;
		draw->AddLine(
			ImVec2(line_x - 1, line_y_start),
			ImVec2(line_x - 1, line_y_end),
			ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, note.alpha * 0.3f)),
			1.0f
		);

		ImGui::PushFont(font.spacegrotesk_medium[1]);
		draw->AddText(
			ImVec2(line_x + 8, pos.y + 5),
			ImColor(1.0f, 1.0f, 1.0f, note.alpha),
			note.message.c_str()
		);
		ImGui::PopFont();

		total_offset += box_size.y + 5.0f;
	}
}

static float left_cursor_y = 0.0f;
static float right_cursor_y = 0.0f;
static ImVec2 base_pos_left = ImVec2(0, 0);
static ImVec2 base_pos_right = ImVec2(0, 0);
static float spacing_x = 12.0f; 
static float spacing_y = 8.0f; 

bool instant_switch = true; 

float ui::get_tab_alpha(int index)
{
	float target = (index == selected_tab_index) ? 1.0f : 0.0f;

	if (instant_switch)
		return target;

	float speed = 6.0f;
	float delta = ImGui::GetIO().DeltaTime;

	float& value = tab_lerp_values[index];
	value = ImLerp(value, target, 1.0f - expf(-speed * delta));

	if (fabsf(value - target) < 0.001f)
		value = target;

	return value;
}

void ui::reset_positions(ImVec2 base_left, ImVec2 base_right, float space_x, float space_y)
{
	base_pos_left = base_left;
	base_pos_right = base_right;
	spacing_x = space_x;
	spacing_y = space_y;
	left_cursor_y = 0.0f;
	right_cursor_y = 0.0f;
}

bool ui::begin_child_left(const char* id, int height)
{
	ImGui::SetCursorPos(ImVec2(base_pos_left.x, base_pos_left.y + left_cursor_y));
	bool ret = ImGui::BeginChild(id, id, ImVec2(child_width, height), true, ImGuiWindowFlags_NoScrollbar);
	ImVec2 child_size = ImGui::GetWindowSize();
	float child_height = child_size.y;

	if (ret)
		left_cursor_y += child_height + spacing_y;

	return ret;
}

bool ui::begin_child_right(const char* id, int height)
{
	ImGui::SetCursorPos(ImVec2(base_pos_right.x, base_pos_right.y + right_cursor_y));
	bool ret = ImGui::BeginChild(id, id, ImVec2(child_width, height), true, ImGuiWindowFlags_NoScrollbar);
	ImVec2 child_size = ImGui::GetWindowSize();
	float child_height = child_size.y;

	if (ret)
		right_cursor_y += child_height + spacing_y;

	return ret;
}

static float smoothstep01(float t)
{
	t = ImClamp(t, 0.0f, 1.0f);
	return t * t * (3.0f - 2.0f * t);
}

void ui::update_animations(float dt)
{
	static int last_tab_index = selected_tab_index;

	if (selected_tab_index != last_tab_index) {
		tab_enter_anim = 0.0f;            // restart slide-in on tab change
		last_tab_index = selected_tab_index;
	}

	tab_enter_anim = ImLerp(tab_enter_anim, 1.0f, ImClamp(dt * 14.0f, 0.0f, 1.0f));

	const float tab_speed = 10.0f;
	if (tab_lerp_values.size() != categories.size())
		tab_lerp_values.resize(categories.size(), 0.0f);
	for (size_t i = 0; i < categories.size(); ++i) {
		const float target = (selected_tab_index == (int)i) ? 1.0f : 0.0f;
		tab_lerp_values[i] = ImLerp(tab_lerp_values[i], target, ImClamp(dt * tab_speed, 0.0f, 1.0f));
	}
}

void ui::render_section_selector(const char* const* names, int count, int& selected)
{
	ImGui::SetCursorPos(ImVec2(20.0f, 11.0f));
	ImGui::PushFont(font.spacegrotesk_medium[1]);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 0.0f));
	for (int i = 0; i < count; ++i) {
		if (i > 0)
			ImGui::SameLine(0.0f, 4.0f);

		const ImVec2 label_size = ImGui::CalcTextSize(names[i]);
		const float tab_w = label_size.x + 16.0f;
		if (ImGui::Selectable(names[i], selected == i, 0, ImVec2(tab_w, 26.0f)))
			selected = i;
	}
	ImGui::PopStyleVar();
	ImGui::PopFont();
}

void ui::setup_section_layout()
{
	const float slide = (1.0f - ImClamp(tab_enter_anim, 0.0f, 1.0f)) * 8.0f;
	reset_positions(
		ImVec2(20.0f, 11.0f + k_section_row + slide),
		ImVec2(20.0f + child_width + 12.0f, 11.0f + k_section_row + slide),
		12.0f,
		40.0f);
}

void ui::render_tabs_content()
{
	ImVec2 pos = ImGui::GetWindowPos();
	ImVec2 window_size = ImGui::GetWindowSize();

	ImVec2 content_pos(pos.x - 9, pos.y + 21);
	ImVec2 content_size(ui::size.x + 18, 502);

	static float current_width = 320.0f;
	float target_width = has_scrollbar ? 312.0f : 320.0f;

	float lerp_speed = 10.0f;
	float delta = ImGui::GetIO().DeltaTime;

	current_width = ImLerp(current_width, target_width, ImClamp(delta * lerp_speed, 0.0f, 1.0f));

	child_width = current_width;

	ImGui::SetCursorScreenPos(content_pos);

	if (ImGui::BeginChild("", "main_content", content_size, false, ImGuiWindowFlags_NoBackground))
	{
		has_scrollbar = ImGui::GetCurrentWindow()->ScrollbarY;

		const float enter = smoothstep01(tab_enter_anim);
		const float slide = (1.0f - enter) * 8.0f;

		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, enter);

		float spacing_x_val = 12.0f;
		float spacing_y_val = 40.0f;
		ImVec2 base_left(20.0f, 11.0f + slide);
		ImVec2 base_right(20.0f + child_width + spacing_x_val, 11.0f + slide);
		reset_positions(base_left, base_right, spacing_x_val, spacing_y_val);

		if (selected_tab_index >= 0 && selected_tab_index < (int)categories.size())
		{
			const auto& tab = categories[selected_tab_index];
			ui::tabs::aimbot(tab);
			ui::tabs::visuals(tab);
			ui::tabs::exploits(tab);
			ui::tabs::cloud(tab);
		}

		ImGui::PopStyleVar();
	}

	ImGui::EndChild();
}

void ui::show_fps()
{
	const float top_height = 50.0f;
	const float left_padding = 12.0f;
	const ImVec2 extra_padding = ImVec2(10, 6);
	const ImVec2 pos = ImVec2(10, 4);

	const char* watermark_text = oxorany("unknowncheats.me");
	ImDrawList* draw_list = ImGui::GetForegroundDrawList();

	ImGui::PushFont(font.montserrat_semibold[0]);
	ImVec2 watermark_size = ImGui::CalcTextSize(watermark_text);
	ImGui::PopFont();

	ImGui::PushFont(font.spacegrotesk_medium[1]);
	std::string user_text = oxorany("fps: ") + std::to_string((int)ImGui::GetIO().Framerate);
	ImVec2 username_size = ImGui::CalcTextSize(user_text.c_str());
	ImGui::PopFont();

	float text_height = ImGui::GetFontSize();
	float centered_y = pos.y + (top_height - text_height) * 0.5f;
	ImVec2 start_pos = ImVec2(pos.x + left_padding, centered_y);

	float separator_width = 1.0f;
	float spacing = 8.0f;

	float total_width = watermark_size.x + spacing + separator_width + spacing + username_size.x;

	ImVec2 bg_min = ImVec2(start_pos.x - extra_padding.x, start_pos.y - extra_padding.y);
	ImVec2 bg_max = ImVec2(start_pos.x + total_width + extra_padding.x,
		start_pos.y + watermark_size.y + extra_padding.y);

	draw_list->AddRectFilled(bg_min, bg_max, ImGui::GetColorU32(ui::colors::background), 3.0f);
	draw_list->AddRect(bg_min, bg_max, ImGui::GetColorU32(ui::colors::outline), 3.0f);

	ImGui::PushFont(font.montserrat_semibold[0]);

	std::string text_str = watermark_text;
	size_t dot_pos = text_str.find('.');

	if (dot_pos != std::string::npos)
	{
		std::string first_part = text_str.substr(0, dot_pos);
		std::string second_part = text_str.substr(dot_pos);

		ui::items::text_shadow(draw_list, start_pos, ImColor(250, 250, 250), first_part.c_str());

		float first_width = ImGui::CalcTextSize(first_part.c_str()).x;
		ImVec2 second_pos = ImVec2(start_pos.x + first_width, start_pos.y);
		ui::items::text_shadow(draw_list, second_pos, ImGui::GetColorU32(ui::colors::main), second_part.c_str());
	}
	else
	{
		ui::items::text_shadow(draw_list, start_pos, ImColor(250, 250, 250), watermark_text);
	}
	ImGui::PopFont();

	ImVec4 white_transparent = ImVec4(1.f, 1.f, 1.f, 0.6f);
	ImU32 separator_color = ImGui::GetColorU32(white_transparent);

	float line_height = 10.0f;

	float offset_y = 1.0f;

	ImVec2 separator_start = ImVec2(
		start_pos.x + watermark_size.x + spacing,
		start_pos.y + (watermark_size.y - line_height) * 0.5f + offset_y
	);
	ImVec2 separator_end = ImVec2(
		separator_start.x,
		separator_start.y + line_height
	);

	draw_list->AddLine(separator_start, separator_end, separator_color, separator_width);

	ImGui::PushFont(font.spacegrotesk_medium[1]);
	ImVec2 user_pos = ImVec2(separator_start.x + separator_width + spacing, start_pos.y - 1);

	ImVec4 col_start = ui::colors::text;
	ImVec4 col_end = ui::colors::main;

	int length = (int)user_text.size();
	ImVec2 cursor_pos = user_pos;

	for (int i = 0; i < length; i++)
	{
		float t = (length > 1) ? (float)i / (length - 1) : 0.0f;

		ImVec4 col = ImVec4(
			col_start.x + (col_end.x - col_start.x) * t,
			col_start.y + (col_end.y - col_start.y) * t,
			col_start.z + (col_end.z - col_start.z) * t,
			1.0f);

		char letter[2] = { user_text[i], '\0' };

		ui::items::text_shadow(draw_list, cursor_pos + ImVec2(0, 5), ImColor(col), letter);

		ImVec2 letter_size = ImGui::CalcTextSize(letter);
		cursor_pos.x += letter_size.x;
	}

	ImGui::PopFont();
}

void ui::render_background()
{
	ImVec2 pos = ImGui::GetWindowPos();
	ImVec2 size = ImGui::GetWindowSize();
	ImVec2 center = ImVec2(size.x / 2.f, size.y / 2.f);
	float max_dist = sqrtf(center.x * center.x + center.y * center.y);

	ImGui::GetBackgroundDrawList()->AddRectFilled(pos, ImVec2(pos.x + ui::size.x, pos.y + ui::size.y), ImColor(ui::colors::background), ui::rounding, ImDrawFlags_RoundCornersAll);

	const float spacing = 28.0f;
	const float outer_radius = 3.0f;
	const float inner_radius = 2.0f;

	ImVec4 base_color = ui::colors::main;
	ImVec4 background_color = ui::colors::background;

	float time = ImGui::GetTime();
	float inv_max_dist = 1.0f / max_dist;

	ImDrawList* bg = ImGui::GetBackgroundDrawList();

	for (float y = 0; y < size.y; y += spacing)
	{
		float dy = y - center.y;
		float dy2 = dy * dy;

		for (float x = 0; x < size.x; x += spacing)
		{
			float dx = x - center.x;
			float dist_sq = dx * dx + dy2;

			float base_alpha = 0.1f * (1.0f - sqrtf(dist_sq) * inv_max_dist);
			if (base_alpha <= 0.005f)
				continue;

			float alpha = base_alpha * (0.5f + 0.5f * sinf(time * 2.0f + (x + y) * 0.05f));

			if (alpha > 0.005f)
			{
				ImU32 col_outer = ImGui::GetColorU32(ImVec4(base_color.x, base_color.y, base_color.z, alpha));
				ImU32 col_inner = ImGui::GetColorU32(ImVec4(background_color.x, background_color.y, background_color.z, alpha));

				ImVec2 circle_pos = ImVec2(pos.x + x + 1.0f, pos.y + y + 1.0f);

				bg->AddCircleFilled(circle_pos, outer_radius, col_outer);
				bg->AddCircleFilled(circle_pos, inner_radius, col_inner);
			}
		}
	}

	ImVec2 rect_top_pos = ImVec2(pos.x, pos.y);
	ImVec2 rect_top_end = ImVec2(rect_top_pos.x + ui::size.x, rect_top_pos.y + 32);

	ImGui::GetBackgroundDrawList()->AddRectFilled(rect_top_pos, rect_top_end, ImColor(ui::colors::background), ui::rounding, ImDrawFlags_RoundCornersTop);

	ImVec2 line_top_start(rect_top_pos.x + 1, rect_top_end.y - 1);
	ImVec2 line_top_end(rect_top_pos.x + ui::size.x - 1, rect_top_end.y - 1);

	ImGui::GetBackgroundDrawList()->AddLine(
		line_top_start,
		line_top_end,
		ImColor(ui::colors::outline),
		1.0f
	);

	ImVec2 rect_pos = ImVec2(pos.x, pos.y + size.y - 39);
	ImVec2 rect_end = ImVec2(rect_pos.x + ui::size.x, rect_pos.y + 39);

	ImGui::GetBackgroundDrawList()->AddRectFilled(rect_pos, rect_end, ImColor(ui::colors::background), ui::rounding, ImDrawFlags_RoundCornersBottom);

	ImVec2 line_start(rect_pos.x + 1, rect_pos.y);
	ImVec2 line_end(rect_pos.x + ui::size.x - 2, rect_pos.y);

	ImGui::GetBackgroundDrawList()->AddLine(line_start, line_end, ImColor(ui::colors::outline), 1.0f);
}

void ui::render_title()
{
	ImVec2 pos = ImGui::GetWindowPos();
	ImVec2 size = ImGui::GetWindowSize();

	ImGui::PushFont(font.montserrat_semibold[0]);

	const float padding_right = 13.0f;
	const float padding_bottom = 13.0f;

	const char* title = ImGui::GetCurrentWindow()->Name;
	std::string title_str = title;
	size_t dot_pos = title_str.find('.');

	float text_height = ImGui::GetFontSize();

	if (dot_pos != std::string::npos)
	{
		std::string first_part = title_str.substr(0, dot_pos);
		std::string second_part = title_str.substr(dot_pos);

		float first_part_width = ImGui::CalcTextSize(first_part.c_str()).x;
		float second_part_width = ImGui::CalcTextSize(second_part.c_str()).x;
		float total_width = first_part_width + second_part_width;

		float second_part_x = pos.x + size.x - second_part_width - padding_right;
		float first_part_x = second_part_x - first_part_width;
		float text_y = pos.y + size.y - text_height - padding_bottom;

		ui::items::text_shadow(ImGui::GetBackgroundDrawList(), ImVec2(first_part_x, text_y), ImColor(ui::colors::text), first_part.c_str());
		ui::items::text_shadow(ImGui::GetBackgroundDrawList(), ImVec2(second_part_x, text_y), ImGui::GetColorU32(ui::colors::main), second_part.c_str());
	}
	else
	{
		float text_width = ImGui::CalcTextSize(title).x;
		float text_x = pos.x + size.x - text_width - padding_right;
		float text_y = pos.y + size.y - text_height - padding_bottom;

		ui::items::text_shadow(ImGui::GetBackgroundDrawList(), ImVec2(text_x, text_y), ImColor(ui::colors::text), title);
	}

	ImGui::PopFont();
}

void ui::render_title_cheat(std::string game_name)
{
	ImVec2 pos = ImGui::GetWindowPos();
	ImVec2 size = ImGui::GetWindowSize();

	ImGui::PushFont(font.spacegrotesk_medium[1]);

	const float top_height = 32.0f;
	const float left_padding = 13.0f;

	float text_height = ImGui::GetFontSize();
	float centered_y = pos.y + (top_height - text_height) * 0.5f;

	float text_x = pos.x + left_padding;

	ImDrawList* draw_list = ImGui::GetBackgroundDrawList();

	ImVec2 cursor_pos = ImVec2(text_x, centered_y - 1);

	ImVec4 col_start = ui::colors::text;
	ImVec4 col_end = ui::colors::main;

	int length = (int)game_name.size();

	for (int i = 0; i < length; i++)
	{
		float t = (length > 1) ? (float)i / (length - 1) : 0.0f; 

		ImVec4 col = ImVec4(
			col_start.x + (col_end.x - col_start.x) * t,
			col_start.y + (col_end.y - col_start.y) * t,
			col_start.z + (col_end.z - col_start.z) * t,
			1.0f);

		char letter[2] = { game_name[i], '\0' };

		ui::items::text_shadow(draw_list, cursor_pos, ImColor(col), letter);

		ImVec2 letter_size = ImGui::CalcTextSize(letter);
		cursor_pos.x += letter_size.x;
	}

	ImGui::PopFont();
}

void ui::render_build_date()
{
	ImVec2 pos = ImGui::GetWindowPos();
	ImVec2 size = ImGui::GetWindowSize();

	ImGui::PushFont(font.spacegrotesk_medium[1]);

	const float top_height = 32.0f;
	const float right_padding = 13.0f;  

	float text_height = ImGui::GetFontSize();
	float centered_y = pos.y + (top_height - text_height) * 0.5f;

	std::string build_date = std::string("build: ") + __DATE__;
	std::string build_date_str = to_lower(build_date);

	float text_width = ImGui::CalcTextSize(build_date_str.c_str()).x;

	float text_x = pos.x + size.x - right_padding - text_width;

	ImVec2 text_pos = ImVec2(text_x, centered_y - 1);

	ui::items::text_shadow(ImGui::GetBackgroundDrawList(), text_pos, ImColor(ui::colors::text), build_date_str.c_str());

	ImGui::PopFont();
}

void ui::initialize_tabs()
{
	categories.clear();

	categories.push_back(TabCategory{
		"aimbot", "A"
		});

	categories.push_back(TabCategory{
		"visuals", "B"
		});

	categories.push_back(TabCategory{
		"exploits", "C"
		});

	categories.push_back(TabCategory{
		"configs", "D"
		});

	tab_lerp_values.resize(categories.size(), 0.0f);
}

bool ui::is_tab_selected(const std::string& tab_name) {
	if (selected_tab_index >= 0 && selected_tab_index < static_cast<int>(categories.size())) {
		return categories[selected_tab_index].name == tab_name;
	}
	return false;
}

void ui::render_tabs(float dt)
{
	ImVec2 pos = ImGui::GetWindowPos();
	ImVec2 size = ImGui::GetWindowSize();

	float tab_height = 39.0f;

	ImVec2 rect_pos = ImVec2(pos.x, pos.y + size.y - tab_height);

	float padding_x = 15.0f;
	float spacing = 30.0f;
	float space_between_icon_and_text = 5.0f;

	float x = rect_pos.x + padding_x;
	float y_center = rect_pos.y + tab_height * 0.5f - 1;

	ImVec2 mouse_pos = ImGui::GetIO().MousePos;

	bool mouse_hand_set = false;

	ImVec4 base_color = ui::colors::text;
	ImVec4 highlight_color = ui::colors::main;

	float anim_speed = 12.0f;

	for (size_t i = 0; i < categories.size(); ++i)
	{
		float target = (selected_tab_index == (int)i) ? 1.0f : 0.0f;
		tab_lerp_values[i] = ImLerp(tab_lerp_values[i], target, dt * anim_speed);
	}

	for (size_t i = 0; i < categories.size(); ++i)
	{
		const auto& tab = categories[i];

		std::string icon = tab.icon;
		std::string name = tab.name;

		ImGui::PushFont(font.tab_icon);
		ImVec2 icon_size = ImGui::CalcTextSize(icon.c_str());
		ImGui::PopFont();

		ImGui::PushFont(font.spacegrotesk_medium[1]);
		ImVec2 name_size = ImGui::CalcTextSize(name.c_str());
		ImGui::PopFont();

		float icon_offset_y = 0.0f;

		if (icon == "A") icon_offset_y = 0.0f;
		else if (icon == "B") icon_offset_y = 2.0f;
		else if (icon == "C") icon_offset_y = 2.0f;
		else if (icon == "D") icon_offset_y = 2.0f;

		ImVec2 icon_pos = ImVec2(x, y_center - icon_size.y * 0.5f + icon_offset_y);
		ImVec2 name_pos = ImVec2(icon_pos.x + icon_size.x + space_between_icon_and_text, y_center - name_size.y * 0.5f);

		ImRect icon_rect(icon_pos, ImVec2(icon_pos.x + icon_size.x, icon_pos.y + icon_size.y));
		ImRect name_rect(name_pos, ImVec2(name_pos.x + name_size.x, name_pos.y + name_size.y));

		ImVec4 lerp_color = ImLerp(base_color, highlight_color, tab_lerp_values[i]);
		ImColor color(lerp_color.x, lerp_color.y, lerp_color.z, lerp_color.w);

		ui::items::text_shadow(ImGui::GetBackgroundDrawList(), icon_pos, color, icon.c_str(), font.tab_icon);

		ui::items::text_shadow(ImGui::GetBackgroundDrawList(), name_pos, color, name.c_str(), font.spacegrotesk_medium[1]);

		if (icon_rect.Contains(mouse_pos) || name_rect.Contains(mouse_pos))
		{
			ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
			mouse_hand_set = true;

			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				selected_tab_index = (int)i;
		}

		x += icon_size.x + space_between_icon_and_text + name_size.x + spacing;
	}

	if (!mouse_hand_set)
		ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
}

void ui::render_outline()
{
	ImVec2 pos(ImGui::GetWindowPos());
	ImVec2 size(pos.x + ui::size.x, pos.y + ui::size.y);

	ImGui::GetBackgroundDrawList()->AddRect(
		pos,
		size,
		IM_COL32(
			ui::colors::outline.x * 255,
			ui::colors::outline.y * 255,
			ui::colors::outline.z * 255,
			20
		),
		ui::rounding - 1,
		ImDrawFlags_RoundCornersAll);
}
