#include <iostream>
#include <vector>

#ifdef _WIN32
    #include <windows.h>
#endif

#include "TelaInventario.h"
#include "../Sistema/Menu.h"

void TelaInventario::exibir(Personagem* jogadorAtual, bool mostrarPrecos) 
{
    if (jogadorAtual == nullptr) return;
    Menu::limparTelaDoTerminal();
    
#ifdef _WIN32
    SetConsoleOutputCP(65001); 
#endif

    int largura = Menu::obterLarguraDoTerminalEmColunas();
    std::vector<std::string> logoInventario = 
    {
      "  █████ ██████   █████ █████   █████ ██████████ ██████   █████ ███████████   █████████   ███████████   █████    ███████    ",
      " ░░███ ░░██████ ░░███ ░░███   ░░███ ░░███░░░░░█░░██████ ░░███ ░█░░░███░░░█  ███░░░░░███ ░░███░░░░░███ ░░███   ███░░░░░███  ",
      "  ░███  ░███░███ ░███  ░███    ░███  ░███  █ ░  ░███░███ ░███ ░   ░███  ░  ░███    ░███  ░███    ░███  ░███  ███     ░░███ ",
      "  ░███  ░███░░███░███  ░███    ░███  ░██████    ░███░░███░███     ░███     ░███████████  ░██████████   ░███ ░███      ░███ ",
      "  ░███  ░███ ░░██████  ░░███   ███   ░███░░█    ░███ ░░██████     ░███     ░███░░░░░███  ░███░░░░░███  ░███ ░███      ░███ ",
      "  ░███  ░███  ░░█████   ░░░█████░    ░███ ░   █ ░███  ░░█████     ░███     ░███    ░███  ░███    ░███  ░███ ░░███     ███  ",
      "  █████ █████  ░░█████    ░░███      ██████████ █████  ░░█████    █████    █████   █████ █████   █████ █████ ░░░███████░   ",
      " ░░░░░ ░░░░░    ░░░░░      ░░░      ░░░░░░░░░░ ░░░░░    ░░░░░    ░░░░░    ░░░░░   ░░░░░ ░░░░░   ░░░░░ ░░░░░    ░░░░░░░     "
    };

    std::cout << "\n" << std::string(largura, '=') << "\n\n";
    Menu::imprimirLinhasCentralizadasNaTela(logoInventario, 121, "\033[33m"); 
    std::cout << "\n" << std::string(largura, '=') << "\n\n";
    jogadorAtual->obterInventario()->exibirInventario(jogadorAtual->obterArma(), jogadorAtual->obterEscudo(), jogadorAtual->obterArmadura(), mostrarPrecos); 
}