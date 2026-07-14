#include "Aparencia.h"
#include "../../../Core/Utilidades/ControleDeInput.h"
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

void Aparencia::inicializarConsole() {
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

void Aparencia::maximizarJanelaTerminal() {
#ifdef _WIN32
    ajustarFonteParaResolucao();

    HWND hwnd = GetConsoleWindow();

    SetWindowLong(hwnd, GWL_STYLE, WS_POPUP);

    ShowWindow(hwnd, SW_MAXIMIZE);
#endif
}

void Aparencia::ajustarFonteParaResolucao() {
#ifdef _WIN32
    int larguraTela = GetSystemMetrics(SM_CXSCREEN);
    int alturaTela = GetSystemMetrics(SM_CYSCREEN);
    
    if (larguraTela >= 1920 && alturaTela >= 1080) return;
    
    double escalaX = static_cast<double>(larguraTela) / 1920.0;
    double escalaY = static_cast<double>(alturaTela) / 1080.0;
    double escala = std::min(escalaX, escalaY);
    
    const int FONTE_REFERENCIA = 10;
    
    int novoTamanho = static_cast<int>(FONTE_REFERENCIA * escala);
    
    if (novoTamanho < 8) novoTamanho = 8;
    if (novoTamanho >= FONTE_REFERENCIA) return;
    
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_FONT_INFOEX fontInfo = {};
    fontInfo.cbSize = sizeof(fontInfo);
    fontInfo.dwFontSize.X = 0;
    fontInfo.dwFontSize.Y = static_cast<SHORT>(novoTamanho);
    fontInfo.FontFamily = FF_DONTCARE;
    fontInfo.FontWeight = FW_NORMAL;
    wcscpy_s(fontInfo.FaceName, L"Consolas");
    
    SetCurrentConsoleFontEx(hOut, FALSE, &fontInfo);
#endif
}

void Aparencia::ocultarCursor() {
#ifdef _WIN32
    HANDLE manipuladorDoTerminal = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO informacoesDoCursor;
    informacoesDoCursor.dwSize = 100;
    informacoesDoCursor.bVisible = FALSE;
    SetConsoleCursorInfo(manipuladorDoTerminal, &informacoesDoCursor);
#else
    std::cout << "\033[?25l";
#endif
}

void Aparencia::mostrarCursor() {
#ifdef _WIN32
    HANDLE manipuladorDoTerminal = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO informacoesDoCursor;
    informacoesDoCursor.dwSize = 100;
    informacoesDoCursor.bVisible = TRUE;
    SetConsoleCursorInfo(manipuladorDoTerminal, &informacoesDoCursor);
#else
    std::cout << "\033[?25h";
#endif
}

void Aparencia::definirCorFundoTerminal(const std::string& hexColor) {
    std::cout << "\033]11;" << hexColor << "\007" << std::flush;
}

void Aparencia::limparTela() {
    if (corFundoAtiva != Cor::RESET) {
        std::cout << cor(corFundoAtiva);
    }
    std::cout << "\033[2J\033[3J\033[H" << std::flush;
}

int Aparencia::obterLarguraTerminal() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
#endif
    return 120;
}

int Aparencia::obterAlturaTerminal() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }
#endif
    return 30;
}

void Aparencia::moverCursor(int x, int y) {
#ifdef _WIN32
    COORD coord = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
    std::cout << "\033[" << (y + 1) << ";" << (x + 1) << "H";
#endif
}

int Aparencia::obterPosicaoCursorY() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return csbi.dwCursorPosition.Y;
    }
#endif
    return 8;
}
