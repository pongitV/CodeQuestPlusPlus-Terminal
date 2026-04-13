#pragma once

#include <string>

#include "../Sistema/Personagem.h"

class TelaDerrota 
{
public:
    static void exibir(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido);
};