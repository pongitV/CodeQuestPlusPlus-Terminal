#pragma once
#include "../../../Mapas/MapaInteracao.h"

class Personagem;
enum class LocalizacaoMapa {
    VilaInicial,
    Floresta,
    PonteReino,
    Reino
};

class TelaMapaMundo {
public:
    static ProximaTransicaoMapa exibir(Personagem* jogadorAtual, LocalizacaoMapa localAtual, int progressoVila, int progressoFloresta, int progressoPonteReino, int progressoReino);
};
