#include "SimplificacoesAparencia.h"
#include <iostream>
#include <limits>
#include <chrono>
#include <thread>
#include <algorithm>
#include <cctype>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif
#include "ControleDeInput.h"

void SimplificacoesAparencia::inicializarConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(65001); // Configura UTF-8 globalmente apenas uma vez
#endif
}

std::string SimplificacoesAparencia::cor(Cor codigo) {
    if (codigo == Cor::RESET) return "\033[0m";
    if (codigo == Cor::LARANJA) return "\033[38;5;208m";
    return "\033[" + std::to_string(static_cast<int>(codigo)) + "m";
}

std::string SimplificacoesAparencia::cor(Cor estilo, Cor codigo) {
    if (codigo == Cor::LARANJA) return "\033[" + std::to_string(static_cast<int>(estilo)) + ";38;5;208m";
    return "\033[" + std::to_string(static_cast<int>(estilo)) + ";" + std::to_string(static_cast<int>(codigo)) + "m";
}

void SimplificacoesAparencia::maximizarJanelaTerminal() {
#ifdef _WIN32
    HWND hwnd = GetConsoleWindow();
    ShowWindow(hwnd, SW_MAXIMIZE);
#endif
}

void SimplificacoesAparencia::ocultarCursor() {
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

void SimplificacoesAparencia::limparTela() {
#ifdef _WIN32
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
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return csbi.srWindow.Right - csbi.srWindow.Left; // Removido o +1 para evitar line wrap duplo invisivel
    }
#endif
    return 119;
}

int SimplificacoesAparencia::obterAlturaTerminal() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }
#endif
    return 30;
}

void SimplificacoesAparencia::moverCursor(int x, int y) {
#ifdef _WIN32
    COORD coord = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
#else
    std::cout << "\033[" << (y + 1) << ";" << (x + 1) << "H";
#endif
}

int SimplificacoesAparencia::obterPosicaoCursorY() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        return csbi.dwCursorPosition.Y;
    }
#endif
    return 8;
}

std::string SimplificacoesAparencia::removerCoresANSI(const std::string& texto) {
    std::string resultado;
    resultado.reserve(texto.length());
    bool in_sequence = false;

    for (size_t i = 0; i < texto.length(); ++i) {
        if (in_sequence) {
            // As sequências de controle CSI terminam com um caractere no intervalo 0x40-0x7E ('@' a '~').
            if (texto[i] >= 0x40 && texto[i] <= 0x7E) {
                in_sequence = false;
            }
        } else {
            // Verifica o início de uma sequência CSI: ESC [
            if (texto[i] == '\x1b' && i + 1 < texto.length() && texto[i+1] == '[') {
                in_sequence = true;
                i++; // Pula também o caractere '['
            } else {
                resultado += texto[i];
            }
        }
    }
    return resultado;
}

std::string SimplificacoesAparencia::espacosParaCentralizar(int comprimentoTexto) {
    int espacos = (obterLarguraTerminal() - comprimentoTexto) / 2;
    if (espacos < 0) espacos = 0;
    return std::string(espacos, ' ');
}

void SimplificacoesAparencia::imprimirLinhaDivisoria(char caractere) {
    std::cout << std::string(obterLarguraTerminal(), caractere) << "\n";
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

void SimplificacoesAparencia::exibirCabecalho(const std::string& titulo, Cor corDoCabecalho) {
    std::string tituloUpper = titulo;
    std::transform(tituloUpper.begin(), tituloUpper.end(), tituloUpper.begin(), [](unsigned char c){ return std::toupper(c); });
    
    int largura = obterLarguraTerminal();
    std::cout << cor(corDoCabecalho) << std::string(largura, '=') << cor(Cor::RESET) << "\n\n";
    imprimirCentralizado(tituloUpper, cor(corDoCabecalho));
    std::cout << "\n" << cor(corDoCabecalho) << std::string(largura, '=') << cor(Cor::RESET) << "\n";
}

int SimplificacoesAparencia::imprimirLadoALado(const std::vector<std::string>& colunaEsquerda, const std::vector<std::string>& colunaDireita, int minLarguraEsquerda, int espacamento, Cor corEsquerda, Cor corDireita) {
    int larguraEsq = minLarguraEsquerda;
    for (const auto& s : colunaEsquerda) {
        if (static_cast<int>(removerCoresANSI(s).length()) > larguraEsq) {
            larguraEsq = removerCoresANSI(s).length();
        }
    }
    
    int larguraDir = 0;
    for (const auto& s : colunaDireita) {
        if (static_cast<int>(removerCoresANSI(s).length()) > larguraDir) {
            larguraDir = removerCoresANSI(s).length();
        }
    }

    int recuo = (obterLarguraTerminal() - (larguraEsq + espacamento + larguraDir)) / 2;
    if (recuo < 0) recuo = 0;

    size_t maxLinhas = std::max(colunaEsquerda.size(), colunaDireita.size());
    std::cout << "\n";
    for (size_t i = 0; i < maxLinhas; ++i) {
        std::cout << std::string(recuo, ' ');
        
        if (i < colunaEsquerda.size()) {
            std::string textoEsq = colunaEsquerda[i];
            int padding = larguraEsq - removerCoresANSI(textoEsq).length();
            std::cout << cor(corEsquerda) << textoEsq << std::string(padding > 0 ? padding : 0, ' ') << cor(Cor::RESET);
        } else {
            std::cout << std::string(larguraEsq, ' ');
        }
        
        std::cout << std::string(espacamento, ' ');
        
        if (i < colunaDireita.size()) {
            std::cout << cor(corDireita) << colunaDireita[i] << cor(Cor::RESET);
        }
        std::cout << "\n";
    }
    
    return recuo;
}
