#pragma once

#include <Windows.h>
#include <atomic>
#include "../../../dependencies/imgui/imgui.h"
#include "menu.h"

namespace menu_scroll_hook {

    inline HHOOK g_hook = nullptr;
    inline std::atomic<float> g_pending_wheel { 0.0f };

    inline LRESULT CALLBACK mouse_proc ( int code, WPARAM w_param, LPARAM l_param ) {
        if ( code >= 0 && menu::is_open && w_param == WM_MOUSEWHEEL ) {
            const auto* ms = reinterpret_cast< MSLLHOOKSTRUCT* >( l_param );
            const short delta = static_cast< short >( HIWORD ( ms->mouseData ) );
            g_pending_wheel.fetch_add ( static_cast< float >( delta ) / WHEEL_DELTA );
        }

        return CallNextHookEx ( g_hook, code, w_param, l_param );
    }

    inline void update ( ) {
        if ( menu::is_open ) {
            if ( !g_hook )
                g_hook = SetWindowsHookExW ( WH_MOUSE_LL, mouse_proc, GetModuleHandleW ( nullptr ), 0 );
        }
        else if ( g_hook ) {
            UnhookWindowsHookEx ( g_hook );
            g_hook = nullptr;
            g_pending_wheel.store ( 0.0f );
        }
    }

    inline void apply ( ImGuiIO& io ) {
        const float delta = g_pending_wheel.exchange ( 0.0f );
        if ( delta != 0.0f )
            io.AddMouseWheelEvent ( 0.0f, delta );
    }

    inline void shutdown ( ) {
        if ( g_hook ) {
            UnhookWindowsHookEx ( g_hook );
            g_hook = nullptr;
        }
        g_pending_wheel.store ( 0.0f );
    }

}
