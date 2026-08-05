#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <iostream>

namespace MenuRaycasterUtils {

    inline void flushFrameForConsole(const std::string& output) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hConsole != INVALID_HANDLE_VALUE) {
            DWORD written;
            WriteConsoleA(hConsole, output.data(), (DWORD)output.size(), &written, NULL);
        } else {
            std::cout << output << std::flush;
        }
    }

}
