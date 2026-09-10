#pragma once

#define IMGUI_DEFINE_MATH_OPERATORS

#include "../overlay/overlay.h"
#include "../../metrics/monitor.h"
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <algorithm>
#include <imgui_settings.h>
#include <vector>
#include "../menu/menu.h"
#include "../menu/menu_text_input.h"
#include "../menu/menu_scroll_hook.h"
#include <gui.h>
#include <pixel7.hpp>
#include <prestige_data.h>
#include <crate_icons.h>

namespace cloud_input { void poll ( bool cloud_tab_active ); }

#include "font.h"
#include "../../../impl/controller/detection.h"
#include "../../core/call of duty/loop/loop.h"
#include "../../core/call of duty/settings/platform_config.h"

ImFont* JetBrains     = nullptr;
ImFont* Pixel7        = nullptr;
ImFont* PlatformIcons = nullptr;

ID3D11ShaderResourceView* loot_stim       = nullptr;
ID3D11ShaderResourceView* loot_cash       = nullptr;
ID3D11ShaderResourceView* loot_crate      = nullptr;
ID3D11ShaderResourceView* loot_armor      = nullptr;
ID3D11ShaderResourceView* loot_heartbeat  = nullptr;
ID3D11ShaderResourceView* loot_grapple    = nullptr;
ID3D11ShaderResourceView* crate_common    = nullptr;
ID3D11ShaderResourceView* crate_rare      = nullptr;
ID3D11ShaderResourceView* crate_epic      = nullptr;

constexpr int PRESTIGE_STYLES = 4;
constexpr int PRESTIGE_MAX   = 12;
ID3D11ShaderResourceView* prestige_sets [PRESTIGE_STYLES][PRESTIGE_MAX] = {};
int prestige_set_counts [PRESTIGE_STYLES] = {};

inline ImFont* visuals_font ( ) {
	switch ( settings::visuals::font_index ) {
	case 1:  return Pixel7 ? Pixel7 : JetBrains;
	default: return JetBrains;
	}
}


// WIC image loader using raw COM vtable calls — avoids #include <wincodec.h> header conflicts
inline ID3D11ShaderResourceView* LoadTextureWIC ( ID3D11Device* device, const void* data, size_t sz ) {
	if ( !data || !sz || !device ) return nullptr;

	CoInitializeEx ( nullptr, COINIT_MULTITHREADED );

	// CLSID_WICImagingFactory / IID_IWICImagingFactory / GUID_WICPixelFormat32bppRGBA
	static const GUID clsWic  = { 0xcacaf262, 0x9370, 0x4615, { 0xa1, 0x3b, 0x9f, 0x55, 0x39, 0xda, 0x4c, 0x0a } };
	static const GUID iidWic  = { 0xec5ec8a9, 0xc395, 0x4314, { 0x9c, 0x77, 0x54, 0xd7, 0xa9, 0x35, 0xff, 0x70 } };
	static const GUID fmtRGBA = { 0xf5c7ad2d, 0x6a8d, 0x43dd, { 0xa7, 0xa8, 0xa2, 0x99, 0x35, 0x26, 0x1a, 0xe9 } };

	auto vt = [] ( void* obj, int i ) -> void* { return ( *( void*** ) obj ) [i]; };
	auto rel = [&] ( void* o ) { if ( o ) ( ( HRESULT ( __stdcall* )( void* ) ) vt ( o, 2 ) )( o ); };

	void* fac = nullptr;
	if ( FAILED ( CoCreateInstance ( clsWic, nullptr, CLSCTX_INPROC_SERVER, iidWic, &fac ) ) || !fac ) return nullptr;

	// factory->CreateStream ( &stm )                  — vtable[14]
	void* stm = nullptr;
	( ( HRESULT ( __stdcall* )( void*, void** ) ) vt ( fac, 14 ) )( fac, &stm );
	if ( !stm ) { rel ( fac ); return nullptr; }

	// stream->InitializeFromMemory ( ptr, len )        — vtable[16]
	( ( HRESULT ( __stdcall* )( void*, BYTE*, DWORD ) ) vt ( stm, 16 ) )( stm, ( BYTE* ) data, ( DWORD ) sz );

	// factory->CreateDecoderFromStream ( stm, 0, 0, &dec )  — vtable[4]
	void* dec = nullptr;
	auto hr = ( ( HRESULT ( __stdcall* )( void*, void*, const GUID*, DWORD, void** ) ) vt ( fac, 4 ) )( fac, stm, nullptr, 0, &dec );
	if ( FAILED ( hr ) || !dec ) { rel ( stm ); rel ( fac ); return nullptr; }

	// decoder->GetFrame ( 0, &frm )                    — vtable[13]
	void* frm = nullptr;
	( ( HRESULT ( __stdcall* )( void*, UINT, void** ) ) vt ( dec, 13 ) )( dec, 0, &frm );
	if ( !frm ) { rel ( dec ); rel ( stm ); rel ( fac ); return nullptr; }

	// factory->CreateFormatConverter ( &cvt )           — vtable[10]
	void* cvt = nullptr;
	( ( HRESULT ( __stdcall* )( void*, void** ) ) vt ( fac, 10 ) )( fac, &cvt );
	if ( !cvt ) { rel ( frm ); rel ( dec ); rel ( stm ); rel ( fac ); return nullptr; }

	// converter->Initialize ( frm, RGBA, 0, null, 0, 0 ) — vtable[8]
	( ( HRESULT ( __stdcall* )( void*, void*, const GUID*, int, void*, double, int ) ) vt ( cvt, 8 ) )( cvt, frm, &fmtRGBA, 0, nullptr, 0.0, 0 );

	// converter->GetSize ( &w, &h )                     — vtable[3]  (IWICBitmapSource)
	UINT w = 0, h = 0;
	( ( HRESULT ( __stdcall* )( void*, UINT*, UINT* ) ) vt ( cvt, 3 ) )( cvt, &w, &h );
	if ( !w || !h ) { rel ( cvt ); rel ( frm ); rel ( dec ); rel ( stm ); rel ( fac ); return nullptr; }

	// converter->CopyPixels ( null, stride, size, buf )  — vtable[7]  (IWICBitmapSource)
	std::vector<uint8_t> px ( w * h * 4 );
	( ( HRESULT ( __stdcall* )( void*, void*, UINT, UINT, BYTE* ) ) vt ( cvt, 7 ) )( cvt, nullptr, w * 4, ( UINT ) px.size ( ), px.data ( ) );

	D3D11_TEXTURE2D_DESC td {};
	td.Width = w; td.Height = h; td.MipLevels = 1; td.ArraySize = 1;
	td.Format = DXGI_FORMAT_R8G8B8A8_UNORM; td.SampleDesc.Count = 1;
	td.Usage = D3D11_USAGE_DEFAULT; td.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA sd {};
	sd.pSysMem = px.data ( ); sd.SysMemPitch = w * 4;

	ID3D11Texture2D* tex = nullptr;
	device->CreateTexture2D ( &td, &sd, &tex );

	ID3D11ShaderResourceView* srv = nullptr;
	if ( tex ) { device->CreateShaderResourceView ( tex, nullptr, &srv ); tex->Release ( ); }

	rel ( cvt ); rel ( frm ); rel ( dec ); rel ( stm ); rel ( fac );
	return srv;
}

namespace render {


	bool setup ( ) {
		ImGui_ImplWin32_EnableDpiAwareness ( );

		metrics::init ( );

		ZeroMemory ( &direct_x::swapChainDesc , sizeof ( direct_x::swapChainDesc ) );
		direct_x::swapChainDesc.BufferCount = 1;
		direct_x::swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		direct_x::swapChainDesc.BufferDesc.Width = metrics::width;
		direct_x::swapChainDesc.BufferDesc.Height = metrics::height;
		direct_x::swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		direct_x::swapChainDesc.OutputWindow = direct_x::my_wnd;
		direct_x::swapChainDesc.SampleDesc.Count = 1;
		direct_x::swapChainDesc.Windowed = TRUE;
		direct_x::swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		D3D_FEATURE_LEVEL featureLevel;
		const D3D_FEATURE_LEVEL featureLevelArray [ 1 ] = { D3D_FEATURE_LEVEL_11_0 };

		if ( FAILED ( D3D11CreateDeviceAndSwapChain (
			nullptr ,
			D3D_DRIVER_TYPE_HARDWARE ,
			nullptr ,
			0 ,
			featureLevelArray ,
			1 ,
			D3D11_SDK_VERSION ,
			&direct_x::swapChainDesc ,
			&direct_x::p_swapChain ,
			&direct_x::p_device ,
			&featureLevel ,
			&direct_x::p_context ) ) )
		{
			exit ( 4 );
		}

		ID3D11Texture2D* pBackBuffer = nullptr;
		direct_x::p_swapChain->GetBuffer ( 0 , __uuidof( ID3D11Texture2D ) , ( LPVOID* ) &pBackBuffer );
		direct_x::p_device->CreateRenderTargetView ( pBackBuffer , NULL , &direct_x::p_renderTargetView );
		pBackBuffer->Release ( );

		direct_x::p_context->OMSetRenderTargets ( 1 , &direct_x::p_renderTargetView , NULL );

		ImGui::CreateContext ( );

		ImGuiStyle* s = &ImGui::GetStyle ( );
		s->WindowPadding = ImVec2 ( 0 , 0 ) , s->WindowBorderSize = 0;
		s->ItemSpacing = ImVec2 ( 20 , 20 );

		s->ScrollbarSize = 4.f;


		ui::initialize_fonts ( );
		ui::initialize_images ( );
		ui::initialize_tabs ( );

		ImGui_ImplWin32_Init ( direct_x::my_wnd );
		ImGuiIO& io = ImGui::GetIO ( ); ( void ) io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.IniFilename = nullptr;
		ImFontConfig cfg;

		ImGui_ImplDX11_Init ( direct_x::p_device , direct_x::p_context );


		static std::vector<std::vector<uint8_t>> g_FontBuffers;

		auto load_font = [ & ] ( const uint8_t* data, float size )
			{
				return io.Fonts->AddFontFromMemoryTTF (
					data ,
					framd_size ,
					size ,
					nullptr ,
					io.Fonts->GetGlyphRangesCyrillic ( )
				);
			};

		auto load_font_file = [ & ] ( const char* path, float size ) -> ImFont*
			{
				return io.Fonts->AddFontFromFileTTF (path,size,nullptr,io.Fonts->GetGlyphRangesCyrillic ( ));
			};


		JetBrains = load_font (20.0f);

		auto& buf = g_FontBuffers.emplace_back ( );
		buf.assign ( resources::fonts::pixel7, resources::fonts::pixel7 + sizeof ( resources::fonts::pixel7 ) );
		Pixel7 = io.Fonts->AddFontFromMemoryTTF ( buf.data ( ), ( int ) buf.size ( ), 10.0f, nullptr, io.Fonts->GetGlyphRangesCyrillic ( ) );

		{
			#include "../../dependencies/imgui/platform_font.h"
			auto& pbuf = g_FontBuffers.emplace_back ( );
			pbuf.assign ( PlatformFont, PlatformFont + sizeof ( PlatformFont ) );
			PlatformIcons = io.Fonts->AddFontFromMemoryTTF ( pbuf.data ( ), ( int ) pbuf.size ( ), 15.0f, nullptr, io.Fonts->GetGlyphRangesDefault ( ) );
		}

		{
			D3DX11_IMAGE_LOAD_INFO li; ID3DX11ThreadPump* pp { nullptr };
			D3DX11CreateShaderResourceViewFromMemory ( direct_x::p_device, resources::images::stim,             sizeof ( resources::images::stim ),             &li, pp, &loot_stim,      0 );
			D3DX11CreateShaderResourceViewFromMemory ( direct_x::p_device, resources::images::cash,             sizeof ( resources::images::cash ),             &li, pp, &loot_cash,      0 );
			D3DX11CreateShaderResourceViewFromMemory ( direct_x::p_device, resources::images::crate,            sizeof ( resources::images::crate ),            &li, pp, &loot_crate,     0 );
			D3DX11CreateShaderResourceViewFromMemory ( direct_x::p_device, resources::images::armor,            sizeof ( resources::images::armor ),            &li, pp, &loot_armor,     0 );
			D3DX11CreateShaderResourceViewFromMemory ( direct_x::p_device, resources::images::heartbeat_sensor, sizeof ( resources::images::heartbeat_sensor ), &li, pp, &loot_heartbeat, 0 );
			D3DX11CreateShaderResourceViewFromMemory ( direct_x::p_device, resources::images::grappling_hook,   sizeof ( resources::images::grappling_hook ),   &li, pp, &loot_grapple,   0 );
		}

		{
			crate_common = LoadTextureWIC ( direct_x::p_device, resources::images::crate_common, sizeof ( resources::images::crate_common ) );
			crate_rare   = LoadTextureWIC ( direct_x::p_device, resources::images::crate_rare,   sizeof ( resources::images::crate_rare ) );
			crate_epic   = LoadTextureWIC ( direct_x::p_device, resources::images::crate_epic,   sizeof ( resources::images::crate_epic ) );
		}


		{
			using namespace resources::prestige;
			for ( int s = 0; s < STYLE_COUNT; ++s ) {
				prestige_set_counts [s] = counts [s];
				for ( int i = 0; i < counts [s]; ++i ) {
					const unsigned char* ptr = all_ptrs [s][i];
					size_t sz = all_sizes [s][i];
					if ( ptr && sz > 1 )
						prestige_sets [s][i] = LoadTextureWIC ( direct_x::p_device, ptr, sz );
				}
			}
		}

		return S_OK;

	}

	bool stream_proof_active = false;

	bool loop ( ) {

		static RECT old_rc = {};
		static bool menu_open = true;
		static bool insert_was_down = false;
		ZeroMemory ( &direct_x::messager , sizeof ( MSG ) );

		while ( direct_x::messager.message != WM_QUIT )
		{

			while ( PeekMessage ( &direct_x::messager , direct_x::my_wnd , 0 , 0 , PM_REMOVE ) )
			{
				TranslateMessage ( &direct_x::messager );
				DispatchMessage ( &direct_x::messager );
			}


			RECT rc = {};
			rc.left = 0;
			rc.top = 0;
			rc.right = metrics::width;
			rc.bottom = metrics::height;
			POINT xy = { 0, 0 };

			ImGuiIO& io = ImGui::GetIO ( );
			static LARGE_INTEGER freq = {} , last = {};
			if ( freq.QuadPart == 0 ) QueryPerformanceFrequency ( &freq );
			LARGE_INTEGER now; QueryPerformanceCounter ( &now );
			if ( last.QuadPart != 0 ) {
				io.DeltaTime = ( float ) ( now.QuadPart - last.QuadPart ) / ( float ) freq.QuadPart;
				io.DeltaTime = std::clamp ( io.DeltaTime , 0.0001f , 0.5f );
			}
			else io.DeltaTime = 1.0f / 60.0f;
			last = now;

			const bool insert_is_down = ( GetAsyncKeyState ( VK_INSERT ) & 0x8000 ) != 0;
			if ( insert_is_down && !insert_was_down ) {
				menu::is_open = !menu::is_open;
			}
			insert_was_down = insert_is_down;

			const bool menu_input_active = menu::is_open || !platform_config::platform_chosen;

			POINT p;
			GetCursorPos ( &p );
			io.MousePos.x = static_cast< float >( p.x );
			io.MousePos.y = static_cast< float >( p.y );

			if ( menu_input_active && ( GetAsyncKeyState ( VK_LBUTTON ) & 0x8000 ) )
			{
				io.MouseDown [ 0 ] = true;
				io.MouseClicked [ 0 ] = true;
				io.MouseClickedPos [ 0 ].x = io.MousePos.x;
				io.MouseClickedPos [ 0 ].y = io.MousePos.y;
			}
			else
			{
				io.MouseDown [ 0 ] = false;
				io.MouseClicked [ 0 ] = false;
			}

			LONG ex_style = GetWindowLong ( direct_x::my_wnd, GWL_EXSTYLE );
			if ( menu_input_active ) {
				ex_style &= ~WS_EX_TRANSPARENT;
				io.MouseDrawCursor = true;
				menu_scroll_hook::update ( );
			}
			else {
				ex_style |= WS_EX_TRANSPARENT;
				io.MouseDrawCursor = false;
				menu_scroll_hook::shutdown ( );
			}
			SetWindowLong ( direct_x::my_wnd, GWL_EXSTYLE, ex_style );


			if ( rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom )
			{
				old_rc = rc;
				metrics::width = rc.right;
				metrics::height = rc.bottom;

				if ( direct_x::p_swapChain )
				{
					if ( direct_x::p_renderTargetView )
					{
						direct_x::p_renderTargetView->Release ( );
						direct_x::p_renderTargetView = nullptr;
					}

					HRESULT hr = direct_x::p_swapChain->ResizeBuffers ( 0 , metrics::width , metrics::height , DXGI_FORMAT_UNKNOWN , 0 );

					if ( FAILED ( hr ) )
						exit ( 5 );

					ID3D11Texture2D* pBackBuffer = nullptr;
					hr = direct_x::p_swapChain->GetBuffer ( 0 , __uuidof( ID3D11Texture2D ) , ( LPVOID* ) &pBackBuffer );
					if ( SUCCEEDED ( hr ) )
					{
						direct_x::p_device->CreateRenderTargetView ( pBackBuffer , nullptr , &direct_x::p_renderTargetView );
						pBackBuffer->Release ( );
					}

					SetWindowPos ( direct_x::my_wnd , ( HWND ) 0 , xy.x , xy.y , metrics::width , metrics::height , SWP_NOREDRAW );
				}
			}

			ImGui_ImplDX11_NewFrame ( );
			ImGui_ImplWin32_NewFrame ( );

			ImGui::NewFrame ( );

			detection::find_controllers ( io.DeltaTime );

			
			if ( menu_input_active )
				menu_scroll_hook::apply ( io );

			menu_text_input::set_handlers_active ( menu::is_open );
			cloud_input::poll ( menu::is_open && ui::is_tab_selected ( "configs" ) );

			if ( platform_config::platform_chosen ) {
				menu::show ( );
			} else {
				static int selected = 0;

				ImVec2 display = ImGui::GetIO ( ).DisplaySize;
				const float w = 280.0f;
				const float h = 230.0f;

				ImGui::SetNextWindowPos ( ImVec2 ( ( display.x - w ) * 0.5f, ( display.y - h ) * 0.5f ), ImGuiCond_Always );
				ImGui::SetNextWindowSize ( ImVec2 ( w, h ), ImGuiCond_Always );

				ImGui::PushStyleVar ( ImGuiStyleVar_WindowPadding, ImVec2 ( 20, 16 ) );
				ImGui::PushStyleVar ( ImGuiStyleVar_WindowRounding, ui::rounding );
				ImGui::PushStyleVar ( ImGuiStyleVar_FramePadding, ImVec2 ( 8, 6 ) );
				ImGui::PushStyleVar ( ImGuiStyleVar_FrameRounding, 6.0f );
				ImGui::PushStyleVar ( ImGuiStyleVar_ItemSpacing, ImVec2 ( 8, 8 ) );

				ImGui::PushStyleColor ( ImGuiCol_WindowBg, ui::colors::background );
				ImGui::PushStyleColor ( ImGuiCol_Border, ui::colors::outline );
				ImGui::PushStyleColor ( ImGuiCol_Text, ui::colors::text );
				ImGui::PushStyleColor ( ImGuiCol_FrameBg, ImVec4 ( ui::colors::checkbox::background ) );
				ImGui::PushStyleColor ( ImGuiCol_FrameBgHovered, ImVec4 ( 0.13f, 0.13f, 0.16f, 1.0f ) );
				ImGui::PushStyleColor ( ImGuiCol_FrameBgActive, ImVec4 ( 0.15f, 0.15f, 0.18f, 1.0f ) );
				ImGui::PushStyleColor ( ImGuiCol_PopupBg, ui::colors::background );
				ImGui::PushStyleColor ( ImGuiCol_Header, ImVec4 ( ui::colors::main.x, ui::colors::main.y, ui::colors::main.z, 0.25f ) );
				ImGui::PushStyleColor ( ImGuiCol_HeaderHovered, ImVec4 ( ui::colors::main.x, ui::colors::main.y, ui::colors::main.z, 0.35f ) );
				ImGui::PushStyleColor ( ImGuiCol_Button, ImVec4 ( ui::colors::main.x, ui::colors::main.y, ui::colors::main.z, 0.8f ) );
				ImGui::PushStyleColor ( ImGuiCol_ButtonHovered, ImVec4 ( ui::colors::main.x, ui::colors::main.y, ui::colors::main.z, 1.0f ) );
				ImGui::PushStyleColor ( ImGuiCol_ButtonActive, ImVec4 ( ui::colors::main.x * 0.8f, ui::colors::main.y * 0.8f, ui::colors::main.z * 0.8f, 1.0f ) );

				ImGui::PushFont ( font.spacegrotesk_medium [0] );

				ImGui::Begin ( "##platform_select", nullptr,
					ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
					ImGuiWindowFlags_NoCollapse );

				{
					ImVec2 wpos = ImGui::GetWindowPos ( );
					ImVec2 wsz = ImGui::GetWindowSize ( );
					ImGui::GetWindowDrawList ( )->AddRect ( wpos, ImVec2 ( wpos.x + wsz.x, wpos.y + wsz.y ),
						ImGui::GetColorU32 ( ui::colors::outline ), ui::rounding );
				}

				ImGui::TextColored ( ImVec4 ( ui::colors::main ), "slopware" );

				ImGui::PushFont ( font.spacegrotesk_medium [1] );
				ImGui::TextColored ( ImVec4 ( ui::colors::text_disabled ), "select your platform" );
				ImGui::PopFont ( );

				ImGui::Spacing ( );

				const char* plat_names [] = { "Steam", "Xbox", "Battle.net" };

				ImGui::PushFont ( font.spacegrotesk_medium [1] );
				ImGui::SetNextItemWidth ( -1 );
				ImGui::Combo ( " ", &selected, plat_names, 3 );
				ImGui::PopFont ( );

				ImGui::Spacing ( );
				ImGui::Spacing ( );

				float btn_w = ImGui::GetContentRegionAvail ( ).x;
				if ( ImGui::Button ( "confirm", ImVec2 ( btn_w, 32 ) ) ) {
					platform_config::apply ( static_cast<Platform> ( selected ) );
				}

				ImGui::End ( );

				ImGui::PopFont ( );
				ImGui::PopStyleColor ( 12 );
				ImGui::PopStyleVar ( 5 );
			}

			if ( platform_config::platform_chosen ) {
				{
					static bool sp_applied = false;
					bool sp_want = settings::overlay::streamproof;

					if ( sp_want ) {
						DWORD current_affinity = 0;
						GetWindowDisplayAffinity ( direct_x::my_wnd, &current_affinity );
						if ( current_affinity != 0x11 )
							hijack::hide_overlay ( );
						sp_applied = true;
					}
					else if ( sp_applied ) {
						hijack::show_overlay ( );
						sp_applied = false;
					}
				}

				if ( settings::aimbot::show_fov )
				{
					auto drawList = ImGui::GetBackgroundDrawList ( );

					ImVec2 display = ImGui::GetIO ( ).DisplaySize;
					ImVec2 center ( display.x * 0.5f, display.y * 0.5f );

					float finalFov = settings::aimbot::fov;

					int dynamicSegments = std::clamp ( static_cast< int >( finalFov * 1.5f ), 50, 256 );

					drawList->AddCircle ( center, finalFov, IM_COL32 ( 255, 255, 255, 255 ), dynamicSegments, 1.0f );
				}

				ImGui::PushFont ( visuals_font ( ) );
				actor::loop ( );
				{
					const sdk::gInfo* gi = cache::get_game_info ( );
					if ( gi && gi->inGame ) {
						auto* dl = ImGui::GetBackgroundDrawList ( );
						Vector3 cam = cache::get_camera_position ( );
						actor::render_loot ( dl, gi->refdef, cam );
					}
				}
				ImGui::PopFont ( );

				actor::radar_ns::draw ( );
			}

			ui::flush_tooltips ( );

			ImGui::EndFrame ( );

			if ( direct_x::p_context && direct_x::p_renderTargetView )
			{
				FLOAT clearColor [ 4 ] = { 0.0f, 0.0f, 0.0f, 0.0f };
				direct_x::p_context->ClearRenderTargetView ( direct_x::p_renderTargetView , clearColor );
				direct_x::p_context->OMSetRenderTargets ( 1 , &direct_x::p_renderTargetView , nullptr );

				D3D11_VIEWPORT vp;
				vp.TopLeftX = 0;
				vp.TopLeftY = 0;
				vp.Width = static_cast< FLOAT >( metrics::width );
				vp.Height = static_cast< FLOAT >( metrics::height );
				vp.MinDepth = 0.0f;
				vp.MaxDepth = 1.0f;
				direct_x::p_context->RSSetViewports ( 1 , &vp );

				ImGui::Render ( );
				ImGui_ImplDX11_RenderDrawData ( ImGui::GetDrawData ( ) );
			}

			HRESULT result = direct_x::p_swapChain->Present ( 1 , 0 );
			if ( FAILED ( result ) )
			{
				if ( result == DXGI_ERROR_DEVICE_REMOVED || result == DXGI_ERROR_DEVICE_RESET )
					break;
			}
		}

		menu_scroll_hook::shutdown ( );

		ImGui_ImplDX11_Shutdown ( );
		ImGui_ImplWin32_Shutdown ( );
		ImGui::DestroyContext ( );

		if ( direct_x::p_renderTargetView ) { direct_x::p_renderTargetView->Release ( ); direct_x::p_renderTargetView = nullptr; }
		if ( direct_x::p_swapChain ) { direct_x::p_swapChain->Release ( ); direct_x::p_swapChain = nullptr; }
		if ( direct_x::p_context ) { direct_x::p_context->Release ( ); direct_x::p_context = nullptr; }
		if ( direct_x::p_device ) { direct_x::p_device->Release ( ); direct_x::p_device = nullptr; }

		DestroyWindow ( direct_x::my_wnd );
		return direct_x::messager.wParam;

	}

}
