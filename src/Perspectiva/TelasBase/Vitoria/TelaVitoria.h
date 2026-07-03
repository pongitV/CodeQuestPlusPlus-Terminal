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
        const std::vector<std::string>& itensObtidos);
};
