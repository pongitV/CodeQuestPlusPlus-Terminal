#pragma once

#include <string>
#include "../../../Entidades/Personagem.h"

class TelaDerrotaTerminal {
public:
    static void exibir(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate);
};
