#include "TelaVitoria.h"
#include <map>
#include "../../GerenciadorPerspectiva.h"
#include "../../PerspectivaIDE/TelasIDE/TelaVitoriaIDE.h"
#include "../../PerspectivaRaycaster/TelasRaycaster/TelaVitoriaRaycaster.h"
#include "../../../Sistemas/Combate/Combate.h"
#include "../../../Entidades/Personagem.h"
#include "../Combate/TelaCombate.h"

void TelaVitoria::exibir(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido,
    int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate,
    const std::vector<std::string>& itensObtidos)
{
    std::map<std::string, int> contagem;
    for (const std::string& item : itensObtidos) contagem[item]++;
    std::vector<std::pair<std::string, int>> dropsUnicos;
    for (auto const& [nome, qtd] : contagem) dropsUnicos.push_back({nome, qtd});

    bool podeSubirNivel = jogadorAtual->podeSubirDeNivel();
    const std::vector<std::string>& novasDescobertas = Combate::obterNovasDescobertas();
    const std::string& tituloMapa = TelaCombate::tituloMapaAtual;

    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        TelaVitoriaRaycaster::exibir(jogadorAtual, quantidadeDeOuroObtido, quantidadeDeXpObtido,
            totalDeDanoCausado, totalDeDanoRecebido, curaTotalRecebida, turnosCombate,
            dropsUnicos, podeSubirNivel, novasDescobertas, tituloMapa);
    } else {
        TelaVitoriaIDE::exibir(jogadorAtual, quantidadeDeOuroObtido, quantidadeDeXpObtido,
            totalDeDanoCausado, totalDeDanoRecebido, curaTotalRecebida, turnosCombate,
            dropsUnicos, podeSubirNivel, novasDescobertas, tituloMapa);
    }
}
