#pragma once

#include <Windows.h>
#include <tlhelp32.h>
#include <thread>
#include "../../impl/blocker/driver.h"
namespace app {

	inline bool is_cod_running ( ) {
		HANDLE hSnapshot = CreateToolhelp32Snapshot ( TH32CS_SNAPPROCESS, 0 );
		if ( hSnapshot == INVALID_HANDLE_VALUE )
			return false;

		PROCESSENTRY32W pe = { sizeof ( PROCESSENTRY32W ) };
		bool found = false;

		if ( Process32FirstW ( hSnapshot, &pe ) ) {
			do {
				if ( wcscmp ( pe.szExeFile, L"cod.exe" ) == 0 ) {
					found = true;
					break;
				}
			} while ( Process32NextW ( hSnapshot, &pe ) );
		}

		CloseHandle ( hSnapshot );
		return found;
	}

	inline void watch_cod ( ) {
		while ( true ) {

			if ( !is_cod_running ( ) ) {
			//	blocker::restore_ob_callbacks ( );
				ExitProcess ( 0 );
			}

			std::this_thread::sleep_for ( std::chrono::seconds ( 1 ) );
		}
	}

}