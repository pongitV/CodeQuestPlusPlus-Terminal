#include <iostream>
#include <vector>

#include "TelaDerrota.h"
#include "../Sistema/Menu.h"
#include "../Raças/RacaBase.h"

void TelaDerrota::exibir(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido)
{
    Menu::limparTelaDoTerminal();
    int largura = Menu::obterLarguraDoTerminalEmColunas();

    std::vector<std::string> logoDerrota = 
    {
        " ██████████   ██████████ ███████████   ███████████      ███████    ███████████   █████████           ",
        "░░███░░░░███ ░░███░░░░░█░░███░░░░░███ ░░███░░░░░███   ███░░░░░███ ░█░░░███░░░█  ███░░░░░███          ",
        " ░███   ░░███ ░███  █ ░  ░███    ░███  ░███    ░███  ███     ░░███░   ░███  ░  ░███    ░███          ",
        " ░███    ░███ ░██████    ░██████████   ░██████████  ░███      ░███    ░███     ░███████████          ",
        " ░███    ░███ ░███░░█    ░███░░░░░███  ░███░░░░░███ ░███      ░███    ░███     ░███░░░░░███          ",
        " ░███    ███  ░███ ░   █ ░███    ░███  ░███    ░███ ░░███     ███     ░███     ░███    ░███          ",
        " ██████████   ██████████ █████   █████ █████   █████ ░░░███████░      █████    █████   █████ ██ ██ ██",
        "░░░░░░░░░░   ░░░░░░░░░░ ░░░░░   ░░░░░ ░░░░░   ░░░░░    ░░░░░░░       ░░░░░    ░░░░░   ░░░░░ ░░ ░░ ░░ "
    };

    std::cout << "\n" << std::string(largura, '=') << "\n\n";
    Menu::imprimirLinhasCentralizadasNaTela(logoDerrota, 101, "\033[31m");
    std::cout << "\n" << std::string(largura, '=') << "\n\n";

    std::vector<std::string> linhas = {
        "NOME:           " + jogadorAtual->obterNome(),
        "RACA:           " + jogadorAtual->obterRaca()->obterNomeRaca(),
        "CLASSE:         " + jogadorAtual->obterNomeClasse(),
        "HP RESTANTE:    " + std::to_string(jogadorAtual->obterVida()) + "/" + std::to_string(jogadorAtual->obterVidaMaxima()),
        "OURO TOTAL:     " + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + "G",
        "NIVEL:          " + std::to_string(jogadorAtual->obterNivel()) + " (XP: " + std::to_string(jogadorAtual->obterXpAtual()) + "/" + std::to_string(jogadorAtual->obterXpParaSubir()) + ")",
        "", "--- ESTATISTICAS DA BATALHA ---", "OURO OBTIDO:   +" + std::to_string(quantidadeDeOuroObtido) + "G", "XP OBTIDO:     +" + std::to_string(quantidadeDeXpObtido) + " XP", "DANO CAUSADO:   " + std::to_string(totalDeDanoCausado), "DANO RECEBIDO:  " + std::to_string(totalDeDanoRecebido)
    };
    Menu::imprimirLinhasCentralizadasNaTela(linhas, 0, "\033[31m");
    std::cout << "\n\n" << std::string(largura, '=') << "\n";
    Menu::aguardarPressionamentoDeEnter();
}