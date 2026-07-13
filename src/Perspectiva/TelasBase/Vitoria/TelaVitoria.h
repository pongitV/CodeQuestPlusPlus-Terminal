#pragma once

#include <vector>
#include <string>
#include <utility>

#include "../../../Entidades/Personagem.h"

class TelaVitoria 
{
public:
    static void exibir(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido,
        int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate,
        const std::vector<std::string>& itensObtidos, const std::vector<std::string>& inimigosDerrotados,
        int parriesPerfeitos, int maiorDano, int parriesTentados, int parriesEfetivos, int itensConsumidos, const std::vector<std::string>& novasDescobertas);
};
