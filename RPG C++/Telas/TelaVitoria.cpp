#include <iostream>
#include <map>

#include "TelaVitoria.h"
#include "../Gerenciadores/GerenciadorMenu.h"
#include "../Racas/RacaBase.h"
#include "../Utilidades/SimplificacoesAparencia.h"

void TelaVitoria::exibir(SistemaPersonagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, const std::vector<std::string>& itensObtidos)
{
    SimplificacoesAparencia::limparTela();
    int largura = SimplificacoesAparencia::obterLarguraTerminal();

    std::vector<std::string> logoVitoria = 
    {
       " █████   █████ █████ ███████████    ███████    ███████████   █████   █████████   ███ ",
       "░░███   ░░███ ░░███ ░█░░░███░░░█  ███░░░░░███ ░░███░░░░░███ ░░███   ███░░░░░███ ░███ ",
       " ░███    ░███  ░███ ░   ░███  ░  ███     ░░███ ░███    ░███  ░███  ░███    ░███ ░███ ",
       " ░███    ░███  ░███     ░███    ░███      ░███ ░██████████   ░███  ░███████████ ░███ ",
       " ░░███   ███   ░███     ░███    ░███      ░███ ░███░░░░░███  ░███  ░███░░░░░███ ░███ ",
       "  ░░░█████░    ░███     ░███    ░░███     ███  ░███    ░███  ░███  ░███    ░███ ░░░  ",
       "    ░░███      █████    █████    ░░░███████░   █████   █████ █████ █████   █████ ███ ",
       "     ░░░      ░░░░░    ░░░░░       ░░░░░░░    ░░░░░   ░░░░░ ░░░░░ ░░░░░   ░░░░░ ░░░  "
    };

    std::cout << "\n" << std::string(largura, '=') << "\n\n";
    SimplificacoesAparencia::imprimirCentralizadoMultilinha(logoVitoria, 85, SimplificacoesAparencia::cor(Cor::VERDE));
    std::cout << "\n" << std::string(largura, '=') << "\n\n";

    std::vector<std::string> linhas = 
    {
        "NOME:           " + jogadorAtual->obterNome(),
        "RACA:           " + jogadorAtual->obterRaca()->obterNomeRaca(),
        "CLASSE:         " + jogadorAtual->obterNomeClasse(),
        "HP RESTANTE:    " + std::to_string(jogadorAtual->obterVida()) + "/" + std::to_string(jogadorAtual->obterVidaMaxima()),
        "OURO TOTAL:     " + std::to_string(jogadorAtual->obterInventario()->obterOuro()) + "G",
        "NIVEL:          " + std::to_string(jogadorAtual->obterNivel()) + " (XP: " + std::to_string(jogadorAtual->obterXpAtual()) + "/" + std::to_string(jogadorAtual->obterXpParaSubir()) + ")",
        "",
        "--- ESTATISTICAS DA BATALHA ---",
        "OURO OBTIDO:   +" + std::to_string(quantidadeDeOuroObtido) + "G",
        "XP OBTIDO:     +" + std::to_string(quantidadeDeXpObtido) + " XP",
        "DANO CAUSADO:   " + std::to_string(totalDeDanoCausado),
        "DANO RECEBIDO:  " + std::to_string(totalDeDanoRecebido),
        ""
    };

    if (!itensObtidos.empty()) {
        linhas.push_back("ITENS OBTIDOS:");
        std::map<std::string, int> contagem;
        for (const std::string& item : itensObtidos) contagem[item]++;
        for (auto const& [nome, qtd] : contagem) linhas.push_back("  +" + std::to_string(qtd) + "x " + nome);
    } else linhas.push_back("ITENS OBTIDOS: Nenhum");

    SimplificacoesAparencia::imprimirCentralizadoMultilinha(linhas, 0, SimplificacoesAparencia::cor(Cor::VERDE));
    std::cout << "\n" << std::string(largura, '=') << "\n";
    SimplificacoesAparencia::aguardarEnter();
}