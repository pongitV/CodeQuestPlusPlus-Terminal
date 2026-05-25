#pragma once
#include <string>
#include <vector>
#include "../../../Sistemas/Mundo/MapaInteracao.h"

enum class LocalizacaoMapa {
    VilaInicial,
    Floresta,
    Reino
};

class TelaMapaMundial {
public:
    // Recebe o enum representando onde o jogador está atualmente para colocar o marcador
    static ProximaTransicaoMapa exibir(LocalizacaoMapa localAtual, int progressoVila, int progressoFloresta, int progressoReino);
};
