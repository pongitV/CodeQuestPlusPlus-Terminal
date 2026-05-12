#include <iostream>
#include <map>

#include "TelaVitoria.h"
#include "../Racas/RacaBase.h"
#include "TelaBase.h"
#include "../Utilidades/Aparencia.h"

void TelaVitoria::exibir(SistemaPersonagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate, const std::vector<std::string>& itensObtidos)
{
    Aparencia::limparTela();
    int largura = Aparencia::obterLarguraTerminal();

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

    Aparencia::exibirLogoAscii(logoVitoria, 85, Cor::VERDE, "", 15);

    std::vector<std::string> linhas = TelaBase::comporEstatisticasBatalha(jogadorAtual, quantidadeDeOuroObtido, quantidadeDeXpObtido, totalDeDanoCausado, totalDeDanoRecebido, curaTotalRecebida, turnosCombate);
    linhas.push_back("");

    if (!itensObtidos.empty()) {
        linhas.push_back("ITENS OBTIDOS:");
        std::map<std::string, int> contagem;
        for (const std::string& item : itensObtidos) contagem[item]++;
        for (auto const& [nome, qtd] : contagem) linhas.push_back("  +" + std::to_string(qtd) + "x " + nome);
    } else linhas.push_back("ITENS OBTIDOS: Nenhum");

    Aparencia::imprimirBlocoCentralizado(linhas, Aparencia::cor(Cor::VERDE), 15);
    std::cout << "\n";
    Aparencia::imprimirLinhaDivisoria();

    if (jogadorAtual->podeSubirDeNivel())
    {
        std::cout << "\n";
        Aparencia::imprimirCentralizado("*** VOCE PODE SUBIR DE NIVEL! ***", Aparencia::cor(Cor::MAGENTA));
        std::cout << "\n";
    }

    Aparencia::aguardarEnter();
}