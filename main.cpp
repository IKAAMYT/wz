#include <Windows.h>

#include "impl/utils/utils.h"
#include "dependencies/oxorany/oxorany.h"
#include "impl/driverless/day1.h"
#include "workspace/render/overlay/overlay.h"
#include "workspace/render/loop/render.h"
#include <iostream>
#include <functional>
#include "workspace/app/app.h"
#include "workspace/core/call of duty/cache/cache.h"
#include "workspace/core/call of duty/settings/config.h"
#include "dependencies/vigem/setup/controller.h"
#include "impl/blocker/driver.h"
#include "workspace/core/call of duty/settings/platform_config.h"
//#include "workspace/core/unreal-engine/caching/cache.h"

int main ( ) {

    auto std_handle = GetStdHandle ( STD_OUTPUT_HANDLE );
    DWORD mode;
    GetConsoleMode ( std_handle, &mode );
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode ( std_handle, mode );

   // platform_config::try_load_saved ( );

    vigem::setup ( 0 );



    if ( !g_vm->ping ( ) ) {
        MessageBoxA ( nullptr, oxorany ( "failed to find driver." ), oxorany ( "error" ), MB_OK );
        return 1;
    }

    //if ( !blocker::init ( ) ) {
    //    MessageBoxA ( nullptr, oxorany ( "failed to find blocker." ), oxorany ( "error" ), MB_OK );
    //    return 1;
    //}

    MessageBoxA ( nullptr, oxorany ( "press ok once inside cod lobby." ), oxorany ( "success" ), MB_OK );


    if ( !g_vm->attach ( oxorany ( "cod.exe" ) ) ) {
        MessageBoxA ( nullptr, oxorany ( "failed to attach to cod." ), oxorany ( "error" ), MB_OK );
        return 1;
    }


    {
        DWORD cod_pid = g_vm->m_pid;
        EnumWindows ( [] ( HWND hwnd, LPARAM lParam ) -> BOOL {
            DWORD pid;
            GetWindowThreadProcessId ( hwnd, &pid );
            if ( pid != ( DWORD ) lParam ) return TRUE;

            char cls [MAX_PATH] {};
            GetClassNameA ( hwnd, cls, MAX_PATH );
            std::string cn ( cls );

            if ( cn.empty ( ) || !IsWindow ( hwnd ) ) return TRUE;
            if ( cn.find ( "IME" ) != std::string::npos ) return TRUE;
            if ( cn.find ( "#32770" ) != std::string::npos ) return TRUE;
            if ( cn.find ( "MSCTFIME UI" ) != std::string::npos ) return TRUE;
            if ( cn.find ( "CoD Splash Screen" ) != std::string::npos ) return TRUE;
            if ( cn.find ( "CoD Focused Mode Window" ) != std::string::npos ) return TRUE;
            if ( cn.find ( "DXGIWatchdogThreadWindow" ) != std::string::npos ) return TRUE;

            direct_x::game_wnd = hwnd;
            return TRUE;
        }, ( LPARAM ) cod_pid );
    }


    hijack::setup ( );

    render::setup ( );

    std::thread ( cache::listener::update ).detach ( );
    std::thread ( cache::thread_cache_game ).detach ( );
    std::thread ( cache::thread_cache_entities ).detach ( );
    std::thread ( cache::thread_cache_loot ).detach ( );

    std::thread ( app::watch_cod ).detach ( );

    render::loop ( );

    return 1;
}

