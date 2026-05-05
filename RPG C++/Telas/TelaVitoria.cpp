#include <iostream>
#include <map>

#include "TelaVitoria.h"
#include "../Racas/RacaBase.h"
#include "TelaMenu.h"
#include "../Utilidades/SimplificacoesAparencia.h"

void TelaVitoria::exibir(SistemaPersonagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate, const std::vector<std::string>& itensObtidos)
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

    std::vector<std::string> linhas = TelaMenu::comporEstatisticasBatalha(jogadorAtual, quantidadeDeOuroObtido, quantidadeDeXpObtido, totalDeDanoCausado, totalDeDanoRecebido, curaTotalRecebida, turnosCombate);
    linhas.push_back("");

    if (!itensObtidos.empty()) {
        linhas.push_back("ITENS OBTIDOS:");
        std::map<std::string, int> contagem;
        for (const std::string& item : itensObtidos) contagem[item]++;
        for (auto const& [nome, qtd] : contagem) linhas.push_back("  +" + std::to_string(qtd) + "x " + nome);
    } else linhas.push_back("ITENS OBTIDOS: Nenhum");

    SimplificacoesAparencia::imprimirCentralizadoMultilinha(linhas, 0, SimplificacoesAparencia::cor(Cor::VERDE));
    std::cout << "\n" << std::string(largura, '=') << "\n";

    if (jogadorAtual->podeSubirDeNivel())
    {
        std::cout << "\n" << SimplificacoesAparencia::cor(Cor::MAGENTA) 
                  << "  *** VOCE PODE SUBIR DE NIVEL! ***\n"
                  << SimplificacoesAparencia::cor(Cor::RESET) << "\n";
    }

    SimplificacoesAparencia::aguardarEnter();
}