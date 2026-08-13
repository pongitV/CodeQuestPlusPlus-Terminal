#include "Core/Terminal/Appearance/Appearance.h"
#include "Core/Utils/InputControl.h"
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#include <timeapi.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

namespace {
#ifdef _WIN32
    struct TimerResolutionHelper {
        TimerResolutionHelper() {
            timeBeginPeriod(1);
        }
        ~TimerResolutionHelper() {
            timeEndPeriod(1);
        }
    };
    TimerResolutionHelper g_timerResolutionHelper;
#endif
}

void Appearance::bootConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (GetConsoleMode(hOut, &dwMode)) {
        dwMode |= 0x0004;
        dwMode &= ~0x0002;
        SetConsoleMode(hOut, dwMode);
    }
    std::cout << "\033[?7l";
#else
    std::cout << "\033[?7l";
#endif
}

void Appearance::maximizeWindowTerminal() {
#ifdef _WIN32
    adjustSourceForResolution();

    HWND hwnd = GetConsoleWindow();

    SetWindowLong(hwnd, GWL_STYLE, WS_POPUP);

    ShowWindow(hwnd, SW_MAXIMIZE);
#endif
}

void Appearance::adjustSourceForResolution() {
#ifdef _WIN32
    int widthScreen = GetSystemMetrics(SM_CXSCREEN);
    int heightScreen = GetSystemMetrics(SM_CYSCREEN);
    
    if (widthScreen >= 1920 && heightScreen >= 1080) return;
    
    double scaleX = static_cast<double>(widthScreen) / 1920.0;
    double scaleY = static_cast<double>(heightScreen) / 1080.0;
    double scale = std::min(scaleX, scaleY);
    
    const int SOURCE_REFERENCE = 10;
    
    int newSize = static_cast<int>(SOURCE_REFERENCE * scale);
    
    if (newSize < 8) newSize = 8;
    if (newSize >= SOURCE_REFERENCE) return;
    
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_FONT_INFOEX fontInfo = {};
    fontInfo.cbSize = sizeof(fontInfo);
    fontInfo.dwFontSize.X = 0;
    fontInfo.dwFontSize.Y = static_cast<SHORT>(newSize);
    fontInfo.FontFamily = FF_DONTCARE;
    fontInfo.FontWeight = FW_NORMAL;
    wcscpy_s(fontInfo.FaceName, L"Consolas");
    
    SetCurrentConsoleFontEx(hOut, FALSE, &fontInfo);
#endif
}

void Appearance::hideCursor() {
#ifdef _WIN32
    HANDLE terminalManipulator = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInformation;
    cursorInformation.dwSize = 100;
    cursorInformation.bVisible = FALSE;
    SetConsoleCursorInfo(terminalManipulator, &cursorInformation);
#else
    std::cout << "\033[?25l";
#endif
}

void Appearance::concertCursor() {
#ifdef _WIN32
    HANDLE terminalManipulator = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInformation;
    cursorInformation.dwSize = 100;
    cursorInformation.bVisible = TRUE;
    SetConsoleCursorInfo(terminalManipulator, &cursorInformation);
#else
    std::cout << "\033[?25h";
#endif
}

void Appearance::setColorBackgroundTerminal(const std::string& hexColor) {
    std::cout << "\033]11;" << hexColor << "\007" << std::flush;
}

void Appearance::clearScreen() {
    if (colorBackgroundActive != Color::RESET) {
        std::cout << color(colorBackgroundActive);
    }
    std::cout << "\033[2J\033[3J\033[H" << std::flush;
}

int Appearance::getTerminalWidth() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
#endif
    return 120;
}

int Appearance::getTerminalHeight() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }
#endif
    return 30;
}

void Appearance::moveCursor(int x, int y) {
#ifdef _WIN32
    COORD coordinate = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordinate);
#else
    std::cout << "\033[" << (y + 1) << ";" << (x + 1) << "H";
#endif
}

int Appearance::getCursorPostY() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return csbi.dwCursorPosition.Y;
    }
#endif
    return 8;
}
