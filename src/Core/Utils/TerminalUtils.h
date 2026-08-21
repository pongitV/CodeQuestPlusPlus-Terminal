// [PT-BR] Utilidades multiplataforma para terminal
// [EN-US] Cross-platform terminal utilities
#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

// [PT-BR] Funcoes de baixo nivel para controle e formatacao do terminal
// [EN-US] Low-level functions for terminal control and formatting
namespace TerminalUtils {
    inline void clearScreen() {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif
    }

    inline void hideCursor() {
        std::cout << "\033[?25l";
    }

    inline void concertCursor() {
        std::cout << "\033[?25h";
    }

    inline void moveCursor(int x, int y) {
        std::cout << "\033[" << y << ";" << x << "H";
    }

    inline int getTerminalWidth() {
#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetStdHandle(STD_OUTPUT_HANDLE) != INVALID_HANDLE_VALUE &&
            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
            return csbi.srWindow.Right - csbi.srWindow.Left + 1;
        }
        return 120;
#else
        struct winsize w;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) return w.ws_col;
        return 120;
#endif
    }

    inline int getTerminalHeight() {
#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetStdHandle(STD_OUTPUT_HANDLE) != INVALID_HANDLE_VALUE &&
            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
            return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        }
        return 40;
#else
        struct winsize w;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) return w.ws_row;
        return 40;
#endif
    }

    inline int getCursorPostY() {
#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetStdHandle(STD_OUTPUT_HANDLE) != INVALID_HANDLE_VALUE &&
            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
            return csbi.dwCursorPosition.Y;
        }
#endif
        return 0;
    }

    // [PT-BR] Remove sequencias de escape ANSI de uma string_view
    // [EN-US] Strips ANSI escape sequences from a string_view
    inline std::string removeANSIColors(std::string_view text) {
        std::string result;
        result.reserve(text.size());
        for (size_t i = 0; i < text.size(); ++i) {
            if (text[i] == '\033' && i + 1 < text.size() && text[i + 1] == '[') {
                size_t end = text.find('m', i + 2);
                if (end != std::string_view::npos) { i = end; continue; }
            }
            result += text[i];
        }
        return result;
    }

    // [PT-BR] Calcula o comprimento visual real considerando caracteres multibyte UTF-8 e ignorando ANSI
    // [EN-US] Calculates actual visual length considering UTF-8 multibyte characters and ignoring ANSI
    inline int getVisualLength(std::string_view text) {
        std::string noAnsi = removeANSIColors(text);
        int comp = 0;
        for (size_t i = 0; i < noAnsi.size();) {
            unsigned char c = static_cast<unsigned char>(noAnsi[i]);
            if (c < 0x80) { comp++; i++; }
            else if (c < 0xC0) i++;
            else if (c < 0xE0) { comp++; i += 2; }
            else if (c < 0xF0) { comp++; i += 3; }
            else { comp++; i += 4; }
        }
        return comp;
    }

    inline std::string spacesToCenter(int textLength) {
        int width = getTerminalWidth();
        int spaces = std::max(0, (width - textLength) / 2);
        return std::string(spaces, ' ');
    }

    inline std::string centerText(std::string_view text) {
        return spacesToCenter(getVisualLength(text)) + std::string(text);
    }
}
