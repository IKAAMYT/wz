#pragma once

#include <Windows.h>
#include "find-window/window.h"
#include "../../../dependencies/oxorany/oxorany.h"
#include "../../../dependencies/protection/imports/lazy-importer.h"

#include "dwmapi.h"
#include <sstream>
#include <D3DX11.h>

#pragma comment ( lib , "d3d10.lib" )
#pragma comment ( lib , "d3d11.lib" )
#pragma comment ( lib , "d3dx11.lib" )
#pragma comment ( lib , "dwmapi.lib" )

namespace direct_x {
    inline IDXGISwapChain* p_swapChain = nullptr;
    inline ID3D11Device* p_device = nullptr;
    inline ID3D11DeviceContext* p_context = nullptr;
    inline ID3D11RenderTargetView* p_renderTargetView = nullptr;
    inline DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
    inline MSG messager = {};
    inline HWND my_wnd = nullptr;
    inline HWND game_wnd = nullptr;
    inline DWORD processID = 0;
}

struct find_window_data {
    unsigned long pid;
    std::string class_name;
    std::string window_name;
    HWND hwnd;
};

struct enum_data_t {
    DWORD pid;
    HWND hwnd;
};

BOOL CALLBACK enum_windows_proc ( HWND hwnd, LPARAM lParam ) {
    auto* data = reinterpret_cast< enum_data_t* >( lParam );

    DWORD window_pid = 0;
    GetWindowThreadProcessId ( hwnd, &window_pid );

    if ( window_pid == data->pid &&
        GetWindow ( hwnd, GW_OWNER ) == nullptr &&
        IsWindowVisible ( hwnd ) ) {
        data->hwnd = hwnd;
        return FALSE;
    }

    return TRUE;
}

HWND get_hwnd_from_pid ( DWORD pid ) {
    enum_data_t data { pid, nullptr };
    EnumWindows ( enum_windows_proc, reinterpret_cast< LPARAM >( &data ) );
    return data.hwnd;
}

HWND find_child_window_from_parent ( HWND parent, const char* class_name, const char* window_name ) {
    DWORD pid = 0;
    GetWindowThreadProcessId ( parent, &pid );

    if ( pid == 0 )
        return nullptr;

    find_window_data data = { pid, class_name, window_name, nullptr };
    EnumWindows ( enum_windows_proc, reinterpret_cast< LPARAM >( &data ) );
    return data.hwnd;
}

RECT get_client_area_and_size ( HWND hwnd ) {
    RECT rect;
    if ( GetClientRect ( hwnd, &rect ) ) {
        POINT top_left = { rect.left, rect.top };
        POINT bottom_right = { rect.right, rect.bottom };

        ClientToScreen ( hwnd, &top_left );
        ClientToScreen ( hwnd, &bottom_right );

        rect.left = top_left.x;
        rect.top = top_left.y;
        rect.right = bottom_right.x;
        rect.bottom = bottom_right.y;
    }
    else {
        rect = { 0, 0, 0, 0 };
    }

    return rect;
}

namespace hijack {


    inline uintptr_t get_module_base ( uint32_t process_id, LPCTSTR module_name )
    {
        uintptr_t base_address = 0;
        HANDLE snapshot_handle = CreateToolhelp32Snapshot ( TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, process_id );

        if ( snapshot_handle != INVALID_HANDLE_VALUE )
        {
            MODULEENTRY32 module_entry;
            module_entry.dwSize = sizeof ( module_entry );

            if ( Module32First ( snapshot_handle, &module_entry ) )
            {
                do
                {
                    if ( _tcsicmp ( module_entry.szModule, module_name ) == 0 )
                    {
                        base_address = reinterpret_cast< uintptr_t >( module_entry.modBaseAddr );
                        break;
                    }
                } while ( Module32Next ( snapshot_handle, &module_entry ) );
            }
            CloseHandle ( snapshot_handle );
        }
        return base_address;
    }

#define FortPTR reinterpret_cast<uint64_t>
    inline uintptr_t get_module_export ( HANDLE process_handle, uintptr_t module_base, const char* export_name )
    {
        SIZE_T dummy_read_size;
        IMAGE_DOS_HEADER dos_header = { 0 };
        IMAGE_NT_HEADERS64 nt_headers = { 0 };

        if ( !ReadProcessMemory ( process_handle, reinterpret_cast< void* >( module_base ), &dos_header, sizeof ( dos_header ), &dummy_read_size ) ||
            dos_header.e_magic != IMAGE_DOS_SIGNATURE ||
            !ReadProcessMemory ( process_handle, reinterpret_cast< void* >( module_base + dos_header.e_lfanew ), &nt_headers, sizeof ( nt_headers ), &dummy_read_size ) ||
            nt_headers.Signature != IMAGE_NT_SIGNATURE ) {
            return 0;
        }

        const auto export_base = nt_headers.OptionalHeader.DataDirectory [IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
        const auto export_base_size = nt_headers.OptionalHeader.DataDirectory [IMAGE_DIRECTORY_ENTRY_EXPORT].Size;

        if ( !export_base || !export_base_size ) {
            return 0;
        }

        const auto export_data = static_cast< PIMAGE_EXPORT_DIRECTORY >( VirtualAlloc ( nullptr, export_base_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE ) );
        if ( !export_data ) {
            return 0;
        }

        if ( !ReadProcessMemory ( process_handle, reinterpret_cast< void* >( module_base + export_base ), export_data, export_base_size, &dummy_read_size ) ) {
            VirtualFree ( export_data, 0, MEM_RELEASE );
            return 0;
        }

        const auto delta = FortPTR ( export_data ) - export_base;

        const auto name_table = reinterpret_cast< uint32_t* >( export_data->AddressOfNames + delta );
        const auto ordinal_table = reinterpret_cast< uint16_t* >( export_data->AddressOfNameOrdinals + delta );
        const auto function_table = reinterpret_cast< uint32_t* >( export_data->AddressOfFunctions + delta );

        for ( auto i = 0u; i < export_data->NumberOfNames; ++i )
        {
            const std::string current_function_name = std::string ( reinterpret_cast< char* > ( name_table [i] + delta ) );

            if ( !_stricmp ( current_function_name.c_str ( ), export_name ) )
            {
                const auto function_ordinal = ordinal_table [i];
                if ( function_table [function_ordinal] <= 0x1000 )
                    return 0;

                const auto function_address = module_base + function_table [function_ordinal];

                if ( function_address >= module_base + export_base && function_address <= module_base + export_base + export_base_size )
                {
                    VirtualFree ( export_data, 0, MEM_RELEASE );
                    return 0;
                }

                VirtualFree ( export_data, 0, MEM_RELEASE );
                return function_address;
            }
        }

        VirtualFree ( export_data, 0, MEM_RELEASE );
        return 0;
    }

    inline bool hide_window ( uint32_t process_id, HWND window_id, bool hide )
    {
        static HANDLE cached_process = nullptr;
        static void*  cached_alloc = nullptr;
        static uintptr_t cached_func = 0;
        static uint32_t cached_pid = 0;

        if ( cached_pid != process_id || !cached_process ) {
            if ( cached_process ) CloseHandle ( cached_process );
            cached_process = OpenProcess ( PROCESS_ALL_ACCESS, false, process_id );
            cached_pid = process_id;
            cached_alloc = nullptr;
            cached_func = 0;
        }

        if ( !cached_process || cached_process == INVALID_HANDLE_VALUE )
            return false;

        if ( !cached_func ) {
            uintptr_t user32_base = get_module_base ( process_id, L"user32.dll" );
            if ( user32_base )
                cached_func = get_module_export ( cached_process, user32_base, "SetWindowDisplayAffinity" );
        }
        if ( !cached_func )
            return false;

        unsigned char shellcode_buffer [] = "\x48\x89\x4C\x24\x08\x48\x89\x54\x24\x10\x4C\x89\x44\x24\x18\x4C\x89\x4C\x24"
            "\x20\x48\x83\xEC\x38\x48\xB9\xED\xFE\xAD\xDE\xED\xFE\x00\x00\x48\xC7\xC2\xAD"
            "\xDE\x00\x00\x48\xB8\xAD\xDE\xED\xFE\xAD\xDE\x00\x00\xFF\xD0\x48\x83\xC4\x38"
            "\x48\x8B\x4C\x24\x08\x48\x8B\x54\x24\x10\x4C\x8B\x44\x24\x18\x4C\x8B\x4C\x24"
            "\x20\xC3";

        *reinterpret_cast< uintptr_t* >( shellcode_buffer + 26 ) = reinterpret_cast< uintptr_t >( window_id );
        *reinterpret_cast< uint32_t* >( shellcode_buffer + 37 ) = hide ? 0x00000011u : 0x00000000u;
        *reinterpret_cast< uintptr_t* >( shellcode_buffer + 43 ) = cached_func;

        if ( !cached_alloc ) {
            cached_alloc = VirtualAllocEx ( cached_process, 0x0, sizeof ( shellcode_buffer ), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE );
            if ( !cached_alloc ) return false;
        }

        SIZE_T dummy;
        if ( !WriteProcessMemory ( cached_process, cached_alloc, shellcode_buffer, sizeof ( shellcode_buffer ), &dummy ) )
            return false;

        HANDLE th = CreateRemoteThread ( cached_process, nullptr, 0, static_cast< LPTHREAD_START_ROUTINE >( cached_alloc ), nullptr, 0, nullptr );
        if ( !th || th == INVALID_HANDLE_VALUE )
            return false;

        WaitForSingleObject ( th, 100 );
        CloseHandle ( th );

        return true;
    }

    void hide_overlay ( )
    {
        DWORD assid = 0;



        ITaskbarList* pTaskList = NULL;
        HRESULT initRet = CoInitialize ( NULL );
        HRESULT createRet = CoCreateInstance ( CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_ITaskbarList, ( LPVOID* ) &pTaskList );

        GetWindowThreadProcessId ( direct_x::my_wnd, &assid );
        hide_window ( assid, direct_x::my_wnd, true );

        if ( createRet == S_OK ) {
            pTaskList->DeleteTab ( direct_x::my_wnd );
            pTaskList->Release ( );
        }

        CoUninitialize ( );
    }

    void show_overlay ( )
    {
        DWORD assid = 0;



        ITaskbarList* pTaskList = NULL;
        HRESULT initRet = CoInitialize ( NULL );
        HRESULT createRet = CoCreateInstance ( CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_ITaskbarList, ( LPVOID* ) &pTaskList );

        GetWindowThreadProcessId ( direct_x::my_wnd, &assid );
        hide_window ( assid, direct_x::my_wnd, false );

        if ( createRet == S_OK ) {
            pTaskList->AddTab ( direct_x::my_wnd );
            pTaskList->Release ( );
        }

        CoUninitialize ( );
    }

    HWND FindWindowByTitle ( const char* title ) {
        struct FindData {
            const char* target;
            HWND result;
        } data = { title, nullptr };

        EnumWindows ( [] ( HWND hwnd, LPARAM lParam ) -> BOOL {
            FindData* data = reinterpret_cast< FindData* >( lParam );
            char window_text [256] = { 0 };
            GetWindowTextA ( hwnd, window_text, sizeof ( window_text ) - 1 );

            if ( strstr ( window_text, data->target ) != nullptr ) {
                data->result = hwnd;
                return FALSE;
            }
            return TRUE;
            }, reinterpret_cast< LPARAM >( &data ) );

        return data.result;
    }

    HWND find_ime_window_by_process ( DWORD process_id ) {
        struct FindData {
            DWORD pid;
            HWND result;
        } data = { process_id, nullptr };

        EnumWindows ( [] ( HWND hwnd, LPARAM lParam ) -> BOOL {
            FindData* data = reinterpret_cast< FindData* >( lParam );
            DWORD pid = 0;
            GetWindowThreadProcessId ( hwnd, &pid );

            if ( pid == data->pid ) {
                char class_name [256] = { 0 };
                char window_name [256] = { 0 };
                GetClassNameA ( hwnd, class_name, sizeof ( class_name ) - 1 );
                GetWindowTextA ( hwnd, window_name, sizeof ( window_name ) - 1 );

                if ( strcmp ( class_name, "IME" ) == 0 && strcmp ( window_name, "Default IME" ) == 0 ) {
                    data->result = hwnd;
                    return FALSE;
                }
            }
            return TRUE;
            }, reinterpret_cast< LPARAM >( &data ) );

        return data.result;
    }

    bool setup ( ) {

        direct_x::my_wnd = FindWindowA ( nullptr, "Discord Overlay" );


        UpdateWindow ( direct_x::my_wnd );
        ShowWindow ( direct_x::my_wnd, SW_SHOW );

        logging::print ( oxorany ( "hijack setup successful." ) );

        return true;
    }

}

