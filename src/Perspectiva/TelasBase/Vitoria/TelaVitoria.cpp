#include "TelaVitoria.h"
#include <map>
#include "../../GerenciadorPerspectiva.h"
#include "../../PerspectivaIDE/TelasIDE/Vitoria/TelaVitoriaIDE.h"
#include "../../../Sistemas/Combate/Combate.h"
#include "../../../Entidades/Personagem.h"
#include "../Combate/TelaCombate.h"

void TelaVitoria::exibir(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido,
    int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate,
    const std::vector<std::string>& itensObtidos, const std::vector<std::string>& inimigosDerrotados,
    int parriesPerfeitos, int maiorDano)
{
    std::map<std::string, int> contagem;
    for (const std::string& item : itensObtidos) contagem[item]++;
    std::vector<std::pair<std::string, int>> dropsUnicos;
    for (auto const& [nome, qtd] : contagem) dropsUnicos.push_back({nome, qtd});

    bool podeSubirNivel = jogadorAtual->podeSubirDeNivel();
    const std::vector<std::string>& novasDescobertas = Combate::obterNovasDescobertas();
    const std::string& tituloMapa = TelaCombate::contexto.tituloMapaAtual;

    if (GerenciadorPerspectiva::obterInstancia().isVisao3DAtiva()) {
        GerenciadorPerspectiva::obterVitoriaUI().exibir(jogadorAtual, quantidadeDeOuroObtido, quantidadeDeXpObtido,
            totalDeDanoCausado, totalDeDanoRecebido, curaTotalRecebida, turnosCombate,
            inimigosDerrotados, parriesPerfeitos, maiorDano, dropsUnicos, podeSubirNivel, novasDescobertas, tituloMapa);
    } else {
        TelaVitoriaIDE::exibir(jogadorAtual, quantidadeDeOuroObtido, quantidadeDeXpObtido,
            totalDeDanoCausado, totalDeDanoRecebido, curaTotalRecebida, turnosCombate,
            inimigosDerrotados, parriesPerfeitos, maiorDano, dropsUnicos, podeSubirNivel, novasDescobertas, tituloMapa);
    }
}
