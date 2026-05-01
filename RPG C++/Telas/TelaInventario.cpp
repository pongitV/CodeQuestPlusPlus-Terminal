#include <iostream>
#include <vector>

#include "TelaInventario.h"
#include "../Gerenciadores/GerenciadorMenu.h"
#include "../Utilidades/SimplificacoesAparencia.h"

void TelaInventario::exibir(SistemaPersonagem* jogadorAtual, bool mostrarPrecos) 
{
    if (jogadorAtual == nullptr) return;
    SimplificacoesAparencia::limparTela();
    
    int largura = SimplificacoesAparencia::obterLarguraTerminal();
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
    SimplificacoesAparencia::imprimirCentralizadoMultilinha(logoInventario, 121, SimplificacoesAparencia::cor(Cor::AMARELO)); 
    std::cout << "\n" << std::string(largura, '=') << "\n\n";
    jogadorAtual->obterInventario()->exibirInventario(jogadorAtual->obterArma(), jogadorAtual->obterEscudo(), jogadorAtual->obterArmadura(), mostrarPrecos); 
}