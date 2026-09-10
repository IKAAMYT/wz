#pragma once

#include <Windows.h>

namespace metrics {
    inline int width;
    inline int height;

    inline void init ( ) {
        metrics::height = GetSystemMetrics ( SM_CYSCREEN );
        metrics::width = GetSystemMetrics ( SM_CXSCREEN );
    }
}
