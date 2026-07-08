#pragma once

#include <string>

enum class LocalizacaoMapa;

class TelaMapaRaycaster {
public:
    static int exibir(LocalizacaoMapa localAtual, int progressoVila, int progressoFloresta, int progressoPonteReino, int progressoReino);
};
