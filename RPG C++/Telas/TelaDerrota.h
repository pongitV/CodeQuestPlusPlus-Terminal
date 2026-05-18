#pragma once

#include <string>

#include "../Sistemas/SistemaPersonagem.h"

class TelaDerrota 
{
public:
    static void exibir(SistemaPersonagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate);
};