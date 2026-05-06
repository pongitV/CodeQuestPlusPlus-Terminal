#include <iostream>
#include <vector>

#include "TelaDerrota.h"
#include "../Racas/RacaBase.h"
#include "TelaCombate.h"
#include "../Utilidades/SimplificacoesAparencia.h"

void TelaDerrota::exibir(SistemaPersonagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate)
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

    std::cout << "\n";
    SimplificacoesAparencia::imprimirLinhaDivisoria();
    std::cout << "\n";
    SimplificacoesAparencia::imprimirCentralizadoMultilinha(logoDerrota, 101, SimplificacoesAparencia::cor(Cor::VERMELHO));
    std::cout << "\n";
    SimplificacoesAparencia::imprimirLinhaDivisoria();
    std::cout << "\n";

    std::vector<std::string> linhas = TelaCombate::comporEstatisticasBatalha(jogadorAtual, quantidadeDeOuroObtido, quantidadeDeXpObtido, totalDeDanoCausado, totalDeDanoRecebido, curaTotalRecebida, turnosCombate);
    SimplificacoesAparencia::imprimirCentralizadoMultilinha(linhas, 0, SimplificacoesAparencia::cor(Cor::VERMELHO));
    std::cout << "\n\n";
    SimplificacoesAparencia::imprimirLinhaDivisoria();
    SimplificacoesAparencia::aguardarEnter();
}