#pragma once
#include "../Entidades/Personagem.h"

class IDerrotaUI {
public:
    virtual ~IDerrotaUI() = default;
    virtual void exibir(Personagem* jogadorAtual, int quantidadeDeOuroObtido, int quantidadeDeXpObtido, int totalDeDanoCausado, int totalDeDanoRecebido, int curaTotalRecebida, int turnosCombate) = 0;
};
