#include "ControleDeInput.h"
#include <iostream>
#include <string>

#ifdef _WIN32
    #include <conio.h>
#else
    #include <unistd.h>
    #include <termios.h>
    #include <fcntl.h>
#endif
#include "Aparencia.h"

bool ControleDeInput::teclaPressionada() 
{
#ifdef _WIN32
    return _kbhit() != 0;
#else
    return false; // Ambiente POSIX requer implementacao detalhada non-blocking
#endif
}

char ControleDeInput::lerTecla() 
{
#ifdef _WIN32
    return static_cast<char>(_getch());
#else
    return static_cast<char>(std::cin.get());
#endif
}

void ControleDeInput::limparBuffer() 
{
#ifdef _WIN32
    while (_kbhit()) _getch();
#endif
}

ComandoMapa ControleDeInput::traduzirTeclaParaComando(char tecla)
{
    if (tecla == 'w' || tecla == 'W' || tecla == 72) return ComandoMapa::Cima;
    if (tecla == 's' || tecla == 'S' || tecla == 80) return ComandoMapa::Baixo;
    if (tecla == 'a' || tecla == 'A' || tecla == 75) return ComandoMapa::Esquerda;
    if (tecla == 'd' || tecla == 'D' || tecla == 77) return ComandoMapa::Direita;
    if (tecla == 'i' || tecla == 'I') return ComandoMapa::Inventario;
    if (tecla == 'c' || tecla == 'C') return ComandoMapa::Ficha;
    if (tecla == 'b' || tecla == 'B') return ComandoMapa::Bestiario;
    return ComandoMapa::Nenhum;
}

std::string ControleDeInput::lerEntradaProtegida() {
    std::cout << "\033[s";
    std::string entrada;
    while (true) {
        if (!std::getline(std::cin, entrada)) std::cin.clear();
        
        if (!entrada.empty()) {
            entrada.erase(0, entrada.find_first_not_of(" \n\r\t"));
            entrada.erase(entrada.find_last_not_of(" \n\r\t") + 1);
        }
        
        if (entrada.empty()) {
            std::cout << "\033[u\033[J";
            continue;
        }
        return entrada;
    }
}

int ControleDeInput::lerInteiroComLimites(const std::string& promptMensagem, int minimo, int maximo, bool centralizarPrompt, const std::string& margemPersonalizada) {
    int valor;
    if (centralizarPrompt) Aparencia::exibirPrompt(promptMensagem);
    else std::cout << margemPersonalizada << promptMensagem;

    while (true) {
        std::string entrada = lerEntradaProtegida();
        try {
            valor = std::stoi(entrada);
            if (valor >= minimo && valor <= maximo) break;
        } catch (...) {}
        std::cout << "\033[u\033[J"; // Apenas limpa a entrada invalida
    }
    return valor;
}
