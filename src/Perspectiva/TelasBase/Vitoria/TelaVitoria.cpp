#include "TelaVitoria.h"
#include <map>
#include "../../GerenciadorPerspectiva.h"
#include "../../../Sistemas/Combate/Combate.h"
#include "../../../Entidades/Personagem.h"
#include "../Combate/TelaCombate.h"

void TelaVitoria::exibir(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido,
    int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate,
    const std::vector<std::string>& itensObtidos, const std::vector<std::string>& inimigosDerrotados,
    int parriesPerfeitos, int maiorDano, int parriesTentados, int parriesEfetivos, int itensConsumidos, const std::vector<std::string>& novasDescobertas)
{
    std::map<std::string, int> contagem;
    for (const std::string& item : itensObtidos) contagem[item]++;
    std::vector<std::pair<std::string, int>> dropsUnicos;
    for (auto const& [nome, qtd] : contagem) dropsUnicos.push_back({nome, qtd});

    bool podeSubirNivel = jogadorAtual->podeSubirDeNivel();

    const std::string& tituloMapa = TelaCombate::contexto.tituloMapaAtual;

    GerenciadorPerspectiva::obterVitoriaUI().exibir(jogadorAtual, quantidadeDeOuroObtido, quantidadeDeXpObtido,
        totalDeDanoCausado, totalDeDanoRecebido, curaTotalRecebida, turnosCombate,
        inimigosDerrotados, parriesPerfeitos, maiorDano, parriesTentados, parriesEfetivos, itensConsumidos, dropsUnicos, podeSubirNivel, novasDescobertas, tituloMapa);
}
