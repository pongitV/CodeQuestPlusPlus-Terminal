#pragma once

#include <string>
#include "../../../Entidades/Personagem.h"

class TelaDerrota3D {
public:
    static void exibir(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate);
};
