#pragma once

#include <string>

enum class LocalizacaoMapa;

class TelaMapaIDE {
public:
    // Renderiza o mapa e retorna o indice selecionado (0-3) ou -1 para cancelar
    static int exibir(LocalizacaoMapa localAtual, int progressoVila, int progressoFloresta, int progressoPonteReino, int progressoReino, const std::string& msgExtra = "");
};
