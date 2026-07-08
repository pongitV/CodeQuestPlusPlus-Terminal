#pragma once
#include <string>
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

namespace TerminalUtils {
    inline void limparTela() {
#ifdef _WIN32
        std::system("cls");
#else
        std::system("clear");
#endif
    }

    inline void ocultarCursor() {
        std::cout << "\033[?25l";
    }

    inline void mostrarCursor() {
        std::cout << "\033[?25h";
    }

    inline void moverCursor(int x, int y) {
        std::cout << "\033[" << y << ";" << x << "H";
    }

    inline int obterLarguraTerminal() {
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

    inline int obterAlturaTerminal() {
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

    inline int obterPosicaoCursorY() {
#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetStdHandle(STD_OUTPUT_HANDLE) != INVALID_HANDLE_VALUE &&
            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
            return csbi.dwCursorPosition.Y;
        }
#endif
        return 0;
    }

    inline std::string removerCoresANSI(const std::string& texto) {
        std::string resultado;
        resultado.reserve(texto.size());
        for (size_t i = 0; i < texto.size(); ++i) {
            if (texto[i] == '\033' && i + 1 < texto.size() && texto[i + 1] == '[') {
                size_t fim = texto.find('m', i + 2);
                if (fim != std::string::npos) { i = fim; continue; }
            }
            resultado += texto[i];
        }
        return resultado;
    }

    inline int obterComprimentoVisual(const std::string& texto) {
        std::string semAnsi = removerCoresANSI(texto);
        int comp = 0;
        for (size_t i = 0; i < semAnsi.size();) {
            unsigned char c = static_cast<unsigned char>(semAnsi[i]);
            if (c < 0x80) { comp++; i++; }
            else if (c < 0xC0) i++;
            else if (c < 0xE0) { comp++; i += 2; }
            else if (c < 0xF0) { comp++; i += 3; }
            else { comp++; i += 4; }
        }
        return comp;
    }

    inline std::string espacosParaCentralizar(int comprimentoTexto) {
        int largura = obterLarguraTerminal();
        int espacos = std::max(0, (largura - comprimentoTexto) / 2);
        return std::string(espacos, ' ');
    }

    inline std::string centralizarTexto(const std::string& texto) {
        return espacosParaCentralizar(obterComprimentoVisual(texto)) + texto;
    }
}
