#include "SimplificacoesAparencia.h"
#include <iostream>
#include <limits>
#include <chrono>
#include <thread>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif
#include "ControleDeInput.h"

std::string SimplificacoesAparencia::cor(int codigo) {
    if (codigo == Cor::RESET) return "\033[0m";
    return "\033[" + std::to_string(codigo) + "m";
}

std::string SimplificacoesAparencia::cor(int estilo, int codigo) {
    return "\033[" + std::to_string(estilo) + ";" + std::to_string(codigo) + "m";
}

std::string SimplificacoesAparencia::corRGBFundo(int idANSI) {
    return "\033[48;5;" + std::to_string(idANSI) + "m";
}

std::string SimplificacoesAparencia::corRGBTexto(int idANSI) {
    return "\033[38;5;" + std::to_string(idANSI) + "m";
}

void SimplificacoesAparencia::maximizarJanelaTerminal() {
#ifdef _WIN32
    HWND hwnd = GetConsoleWindow();
    ShowWindow(hwnd, SW_MAXIMIZE);
#endif
}

void SimplificacoesAparencia::limparTela() {
#ifdef _WIN32
    SetConsoleOutputCP(65001); // Centralizado para todas as telas do jogo UTF-8
    system("cls");
#else
    system("clear");
#endif
}

void SimplificacoesAparencia::aguardarEnter() {
    std::cout << "\nPressione Enter para continuar...";
    ControleDeInput::limparBuffer();
    while (true) {
        char c = ControleDeInput::lerTecla();
        if (c == '\r' || c == '\n') break;
    }
}

int SimplificacoesAparencia::obterLarguraTerminal() {
    static int cachedLargura = 0;
    if (cachedLargura > 0) return cachedLargura;
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        cachedLargura = csbi.srWindow.Right - csbi.srWindow.Left; // Removido o +1 para evitar line wrap duplo invisivel
    }
#endif
    if (cachedLargura <= 0) cachedLargura = 119;
    return cachedLargura;
}

std::string SimplificacoesAparencia::removerCoresANSI(const std::string& texto) {
    std::string resultado;
    bool inEscape = false;
    for (char c : texto) {
        if (c == '\033' || c == '\x1b') inEscape = true;
        else if (inEscape && c == 'm') inEscape = false;
        else if (!inEscape) resultado += c;
    }
    return resultado;
}

void SimplificacoesAparencia::imprimirCentralizado(const std::string& texto, const std::string& corAnsi) {
    int larguraTerminal = obterLarguraTerminal();
    std::string textoPuro = removerCoresANSI(texto);
    int espacos = (larguraTerminal - static_cast<int>(textoPuro.length())) / 2;
    if (espacos < 0) espacos = 0;
    std::cout << std::string(espacos, ' ') << corAnsi << texto << (corAnsi.empty() ? "" : cor(Cor::RESET)) << "\n";
}

void SimplificacoesAparencia::imprimirCentralizadoMultilinha(const std::vector<std::string>& linhas, int larguraVisual, const std::string& corAnsi) {
    int larguraTerminal = obterLarguraTerminal();
    for (const std::string& linha : linhas) {
        if (larguraVisual > 0) {
            int espacos = (larguraTerminal - larguraVisual) / 2;
            if (espacos < 0) espacos = 0;
            std::cout << std::string(espacos, ' ') << corAnsi << linha << (corAnsi.empty() ? "" : cor(Cor::RESET)) << "\n";
        } else {
            imprimirCentralizado(linha, corAnsi);
        }
    }
}

void SimplificacoesAparencia::imprimirDigitando(const std::string& texto, int atrasoMs) {
    std::cout << "\033[s\033[80;1H" << cor(Cor::NEGRITO, Cor::CINZA) << "[Pressione 'k' para pular]" << cor(Cor::RESET) << "\033[u";
    for (size_t i = 0; i < texto.length(); ++i) {
        if (ControleDeInput::teclaPressionada()) { 
            char tecla = ControleDeInput::lerTecla(); 
            if (tecla == 'k' || tecla == 'K') { 
                std::cout << "\033[s\033[24;1H\033[K\033[u" << texto.substr(i) << std::flush; 
                return; 
            } 
        }
        std::cout << texto[i] << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(atrasoMs));
    }
    std::cout << std::endl;
}
