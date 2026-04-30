#pragma once

#include <vector>
#include <string>

#include "../Sistemas/SistemaPersonagem.h"

class TelaVitoria 
{
public:
    static void exibir(SistemaPersonagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, const std::vector<std::string>& itensObtidos);
};