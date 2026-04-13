#pragma once

#include <vector>
#include <string>

#include "../Sistema/Personagem.h"

class TelaVitoria 
{
public:
    static void exibir(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, const std::vector<std::string>& itensObtidos);
};