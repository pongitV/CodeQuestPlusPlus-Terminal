#include <iostream>
#include <vector>

#include "TelaDerrota.h"
#include "../Gerenciadores/GerenciadorMenu.h"
#include "../Racas/RacaBase.h"
#include "../Utilidades/SimplificacoesAparencia.h"

void TelaDerrota::exibir(SistemaPersonagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido)
{
    SimplificacoesAparencia::limparTela();
    int largura = SimplificacoesAparencia::obterLarguraTerminal();

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
    SimplificacoesAparencia::imprimirCentralizadoMultilinha(logoDerrota, 101, SimplificacoesAparencia::cor(Cor::VERMELHO));
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
    SimplificacoesAparencia::imprimirCentralizadoMultilinha(linhas, 0, SimplificacoesAparencia::cor(Cor::VERMELHO));
    std::cout << "\n\n" << std::string(largura, '=') << "\n";
    SimplificacoesAparencia::aguardarEnter();
}