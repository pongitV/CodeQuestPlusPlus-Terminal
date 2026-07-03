#pragma once

#include <string>
#include "../../../Entidades/Personagem.h"

class TelaDerrotaIDE {
public:
    static void exibir(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate);
};
